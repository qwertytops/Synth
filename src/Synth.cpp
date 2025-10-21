#include "Synth.hpp"
#include "Input.hpp"
#include "Connection.hpp"

#include <iostream>
#include <unordered_set>
#include <cmath>

Synth::Synth(int octave)
    : eventBuffer(EVENT_CAP), player([this](float* buffer, UInt32 frames) { this->RenderAudioBlock(buffer, frames); })
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
                if (!eventBuffer.push(e)) {
                    std::this_thread::yield();
                }
            } else if (!current && prevKeysLocal[k]) {
                // key released: push NOTE_OFF event
                Event e;
                e.type = Event::NOTE_OFF;
                e.midi = static_cast<uint8_t>(k + 8*octave);
                e.time = player.GetTime();
                if (!eventBuffer.push(e)) {
                    std::this_thread::yield();
                }
            }

            prevKeysLocal[k] = current;
        }
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

void Synth::establishProcessingOrder() {
    cout << "epa" << endl;
    // pauseAudio = true;
    player.Pause();
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
            noInputNodes.push_back(node);
            inputs++;
        }
    }

    while (noInputNodes.size() > 0) {
        // cout << "here4" << endl;
        SynthComponent *current = noInputNodes.at(0);
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
    pauseAudio = false;
    player.Unpause();
}

void Synth::broadcastMidiEvent(Event e) {
    for (auto& comp : components) {
        comp->sendMidiEvent(e);
    }
}

void Synth::RenderAudioBlock(float* outBuffer, UInt32 frames) {
    // Pop and apply all pending events once per block (audio-thread, non-blocking)
    Event ev;
    while (eventBuffer.pop(ev)) {
        broadcastMidiEvent(ev);
    }

    // Process block
    for (UInt32 i = 0; i < frames; ++i) {
        double t = player.GetTime() + (double)i / 44100.0;
        double s = MakeSound(t);
        outBuffer[i] = static_cast<float>(std::clamp(s, -1.0, 1.0));
    }
}

double Synth::MakeSound(double elapsed) {
    if (pauseAudio) {
        return 0.0;
    }
    // Reset all inputs
    for (auto& input : allInputs) {
        input->reset();
    }

    // Run components in processing order
    for (auto& component : processingOrder) {
        // cout << "running " << component->id << endl;
        component->run(elapsed);
    }

    // Accumulate result
    double result = 0.0;

    for (auto& amplitude : mainOut->values) {
        result += amplitude;
    }
    return result * volume;
}