#include "Synth.hpp"
#include "NoteInput.hpp"
#include "Connection.hpp"

#include <iostream>
#include <unordered_set>
#include <cmath>
#include <algorithm> // for clamp
#include <cassert>

// UNSUSED BUT KEEPING FOR A BIT JUST IN CASE
double Synth::MakeSound(double elapsed) {
    mtx.lock();

    for (auto& input : allInputs) {
        input->reset();
    }

    for (auto& note : notesBeingPlayed) {
        for (int i = 0; i < inputs; i++) {
            // cout << processingOrder[i]->name << endl;
            processingOrder[i]->inputs.at(0)->add(make_pair(note, 0));
        }
        // cout << endl;
    }
    for (auto& component : processingOrder) {
        // cout << "running " << component->name << endl;
        component->run(elapsed);
    }

    double result = 0.0;

    // Rebuild notesBeingPlayed by inspecting mainOut->pairs.
    // Keep notes that are still active or have non-negligible amplitude,
    // and deduplicate so the same Note* isn't added multiple times.
    const double AMPLITUDE_THRESH = 0.0001;
    std::unordered_set<Note*> seen;
    vector<Note*> keptNotes;
    for (auto& pair : mainOut->pairs) {
        Note* n = pair.first;
        double amplitude = pair.second;
        
        if (n == nullptr) continue;
        if (!n->finished) {
            if (seen.find(n) == seen.end()) {
                seen.insert(n);
                keptNotes.push_back(n);
            }
        }
        result += amplitude;
    }
    notesBeingPlayed = std::move(keptNotes);

    // if (result != 0) {
    //     cout << result << endl;
    // }
    mtx.unlock();
    // cout << result << endl;
    return result * volume;
}

Synth::Synth(int octave)
    : player([this](float* buffer, UInt32 frames) { this->RenderAudioBlock(buffer, frames); })
{
    // initialize lock-free ring buffer
    assert((EVENT_CAP & (EVENT_CAP - 1)) == 0); // must be power of two
    eventBuffer.resize(EVENT_CAP);
    evMask = EVENT_CAP - 1;

    mainOut = new NoteInput("Main Out");
    establishProcessingOrder();
    player.Start();
    thread input([this, octave]() { this->ProcessInput(octave); });
    input.detach();
}


void Synth::addComponent(SynthComponent* comp) {
    components.push_back(comp);
    establishProcessingOrder();
    getAllInputs();
}

void Synth::getAllInputs() {
    vector<NoteInput*> result = {};

    result.push_back(mainOut);

    for (auto& comp : components) {
        for (auto& inp : comp->inputs) {
            result.push_back(inp);
        }
    }

    allInputs = result;
}

bool Synth::pushEvent(const Event& e) {
    size_t tail = evTail.load(std::memory_order_relaxed);
    size_t next = (tail + 1) & evMask;
    size_t head = evHead.load(std::memory_order_acquire);
    if (next == head) {
        // full — drop the event to remain non-blocking
        return false;
    }
    eventBuffer[tail] = e;
    evTail.store(next, std::memory_order_release);
    return true;
}

bool Synth::popEvent(Event& out) {
    size_t head = evHead.load(std::memory_order_relaxed);
    size_t tail = evTail.load(std::memory_order_acquire);
    if (head == tail) {
        return false; // empty
    }
    out = eventBuffer[head];
    evHead.store((head + 1) & evMask, std::memory_order_release);
    return true;
}

void Synth::ProcessInput(int octave) {
    bool prevKeysLocal[18] = {false};

    while (true) {
        for (int k = 0; k < 18; k++) {
            bool current = Synth::keys[k].load(std::memory_order_relaxed); // read atomic

            if (current && !prevKeysLocal[k]) {
                // key pressed: push NOTE_ON event (no mutex, no allocation)
                Event e;
                e.type = Event::NOTE_ON;
                e.midi = static_cast<uint8_t>(k + 8*octave);
                e.time = player.GetTime();
                pushEvent(e);
            } else if (!current && prevKeysLocal[k]) {
                // key released: push NOTE_OFF event
                Event e;
                e.type = Event::NOTE_OFF;
                e.midi = static_cast<uint8_t>(k + 8*octave);
                e.time = player.GetTime();
                pushEvent(e);
            }

            prevKeysLocal[k] = current;
        }
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

void Synth::establishProcessingOrder() {
    // cout << endl << "EPA" << endl;
    vector<SynthComponent *> allNodes = components;
    vector<SynthComponent*> noInputNodes = {};
    vector<SynthComponent*> order = {};

    inputs = 0;

    for (int i = allNodes.size() - 1; i >= 0; i--) {
        SynthComponent* node = allNodes[i];
        for (auto& conn : node->incomingConnections) {
            conn->visited = false;
        }
        if (node->incomingConnections.size() == 0) {
            allNodes.erase(allNodes.begin() + i);
            // cout << "NIN: " << node->name << node->id << endl;
            noInputNodes.push_back(node);
            inputs++;
        }
    }
    // cout << "inputs: " << inputs << endl;

    while (noInputNodes.size() > 0) {
        SynthComponent* current = noInputNodes.at(0);
        order.push_back(current);
        noInputNodes.erase(noInputNodes.begin());

        for (auto& conn : current->outgoingConnections) {
            conn->visited = true;
            bool noIncoming = true;

            if (conn->destination->parent == nullptr) {
                continue;
            }


            for (auto& conn1 : conn->destination->parent->incomingConnections) {
                if (!conn1->visited) {
                    noIncoming = false;
                    break;
                }
            }
            if (noIncoming) {
                SynthComponent* newNoInput = conn->destination->parent;
                noInputNodes.push_back(newNoInput);

                vector<SynthComponent*>::iterator position = find(allNodes.begin(), allNodes.end(), newNoInput);

                allNodes.erase(position);
            }
        }
    }

    processingOrder = std::move(order);

    // cout << "\norder:\n";
    
    // for (auto& c : processingOrder) {
    //     cout << c->name << c->id << endl;
    // }
}

void Synth::RenderAudioBlock(float* outBuffer, UInt32 frames) {
    // Pop and apply all pending events once per block (audio-thread, non-blocking)
    Event ev;
    while (popEvent(ev)) {
        if (ev.type == Event::NOTE_ON) {
            Note* n = new Note(static_cast<int>(ev.midi), ev.time);
            notesBeingPlayed.push_back(n);
        } else if (ev.type == Event::NOTE_OFF) {
            // mark matching active note off
            for (auto& note : notesBeingPlayed) {
                if (note->midiNum == ev.midi && note->active) {
                    note->noteOff = ev.time;
                    note->active = false;
                    break;
                }
            }
        }
    }

    // Process block without taking mtx (audio thread must not block)
    for (UInt32 i = 0; i < frames; ++i) {
        double t = player.GetTime() + (double)i / 44100.0;
        double s = MakeSoundLocked(t);
        outBuffer[i] = static_cast<float>(std::clamp(s, -1.0, 1.0));
    }
}

double Synth::MakeSoundLocked(double elapsed) {
    // Reset all inputs (reuse existing NoteInput objects)
    for (auto& input : allInputs) {
        input->reset();
    }

    // Push active notes into input nodes (use processingOrder[0..inputs-1])
    for (auto* note : notesBeingPlayed) {
        for (int i = 0; i < inputs && i < (int)processingOrder.size(); ++i) {
            processingOrder[i]->inputs.at(0)->add(std::make_pair(note, 0.0));
        }
    }

    // Run components in processing order
    for (auto& component : processingOrder) {
        component->run(elapsed);
    }

    // Accumulate result and rebuild notesBeingPlayed (deduplicate)
    double result = 0.0;
    const double AMPLITUDE_THRESH = 0.0001;
    std::unordered_set<Note*> seen;
    vector<Note*> keptNotes;
    keptNotes.reserve(notesBeingPlayed.size());

    for (auto& pair : mainOut->pairs) {
        Note* n = pair.first;
        double amplitude = pair.second;
        if (n == nullptr) continue;
        if (!n->finished || std::fabs(amplitude) > AMPLITUDE_THRESH) {
            if (seen.insert(n).second) {
                keptNotes.push_back(n);
            }
        }
        result += amplitude;
    }
    notesBeingPlayed = std::move(keptNotes);
    return result * volume;
}