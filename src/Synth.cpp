#include "Synth.hpp"
#include "Input.hpp"
#include "Connection.hpp"

#include <iostream>
#include <unordered_set>
#include <cmath>
#include <algorithm> // for clamp

// UNSUSED BUT KEEPING FOR A BIT JUST IN CASE
double Synth::MakeSound(double elapsed) {
    mtx.lock();

    for (auto& input : allInputs) {
        input->reset();
    }

    for (auto& note : notesBeingPlayed) {
        for (int i = 0; i < inputs; i++) {
            // cout << processingOrder[i]->name << endl;
            processingOrder[i]->inputs.at(0)->pairs.push_back(make_pair(note, 0));
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
    : player([this](float* buffer, UInt32 frames) { this->RenderAudioBlock(buffer, frames); }) // changed signature
{
    mainOut = new Input("Main Out");
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
    vector<Input*> result = {};

    result.push_back(mainOut);

    for (auto& comp : components) {
        for (auto& inp : comp->inputs) {
            result.push_back(inp);
        }
    }

    allInputs = result;
}

void Synth::ProcessInput(int octave) {
    bool prevKeys[18] = {false};
    bool held[18] = {false};

    while (true) {
        mtx.lock();
        for (int k = 0; k < 18; k++) {
            if (keys[k] && !prevKeys[k]) {
                Note* n = new Note(k + 8*octave, player.GetTime()); // check memory leaks
                notesBeingPlayed.push_back(n);
            } else if (!keys[k] && prevKeys[k]) {
                for (auto& note : notesBeingPlayed) {
                    if (note->midiNum == k + 8*octave && note->active) {
                        note->noteOff = player.GetTime();
                        note->active = false;
                        break;
                    }
                }
            }
            prevKeys[k] = keys[k];
        }
        mtx.unlock();
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

    cout << "\norder:\n";
    
    for (auto& c : processingOrder) {
        cout << c->name << c->id << endl;
    }
}

void Synth::RenderAudioBlock(float* outBuffer, UInt32 frames) {
    // Lock once for the whole block to avoid per-sample mutex overhead
    mtx.lock();
    for (UInt32 i = 0; i < frames; ++i) {
        // compute sample time for this frame
        double t = player.GetTime() + (double)i / 44100.0; // approximate; player will advance after block
        double s = MakeSoundLocked(t);
        // clamp and write
        outBuffer[i] = static_cast<float>(std::clamp(s, -1.0, 1.0));
    }
    mtx.unlock();
}

double Synth::MakeSoundLocked(double elapsed) {
    // Reset all inputs (reuse existing Input objects)
    for (auto& input : allInputs) {
        input->pairs.clear(); // keep capacity to avoid realloc
    }

    // Push active notes into input nodes (use processingOrder[0..inputs-1])
    for (auto* note : notesBeingPlayed) {
        for (int i = 0; i < inputs && i < (int)processingOrder.size(); ++i) {
            processingOrder[i]->inputs.at(0)->pairs.push_back(std::make_pair(note, 0.0));
        }
    }

    // Run components in processing order
    for (auto& component : processingOrder) {
        component->run(elapsed); // crashes?
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