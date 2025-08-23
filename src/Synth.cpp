#include "Synth.hpp"
#include "Input.hpp"
#include "Connection.hpp"

double Synth::MakeSound(double elapsed) {
    double output = 0.0;
    vector<Note> notesToKeep = {};
    mtx.lock();
    for (auto& note : notesBeingPlayed) {
        double result = 0;
        for (int i = 0; i < inputs; i++) {
            components[i]->inputs.at(0)->pairs.push_back(make_pair(&note, 0));
        }
    }
    for (auto& component : components) {
        // make sure something is connected to mainOut (which doesnt exist yet)
        component->run(elapsed);
    }
        // output = mainOut->value or something;
    // for (auto& note : notesBeingPlayed) {
    //     double noteRes = 0;
    //     for (auto osc : oscillators) {
    //         noteRes += osc->run(elapsed, note.midiNum);
    //     }
    //     bool keepNote = false;
    //     for (auto env : envelopes) {
    //         double amp = env->GetAmplitude(elapsed, &note);
    //         noteRes *= amp;
    //         if (note.active || amp > 0.0001) {
    //             keepNote = true;
    //         }
    //     }
    //     if (envelopes.empty()) {
    //         if (note.active) keepNote = true;
    //     }
    //     if (keepNote) {
    //         notesToKeep.push_back(note);
    //     }
    //     result += noteRes;
    // }
    // for (auto& filter : filters) {
    //     result = filter->run(result);
    // }
    // notesBeingPlayed = std::move(notesToKeep);
    mtx.unlock();
    return output * volume;
}

Synth::Synth(vector<SynthComponent*> components, int octave)
    : components(components),
      player([this](double elapsed) { return this->MakeSound(elapsed); })
{
    processingOrder = establishProcessingOrder();
    player.Start();
    thread input([this, octave]() { this->ProcessInput(octave); });
    input.detach();
}

void Synth::ProcessInput(int octave) {
    bool prevKeys[18] = {false};
    bool held[18] = {false};

    while (true) {
        mtx.lock();
        for (int k = 0; k < 18; k++) {
            if (keys[k] && !prevKeys[k]) {
                Note n(k + 8*octave, player.GetTime());
                notesBeingPlayed.push_back(n);
            } else if (!keys[k] && prevKeys[k]) {
                for (auto& note : notesBeingPlayed) {
                    if (note.midiNum == k + 8*octave && note.active) {
                        note.noteOff = player.GetTime();
                        note.active = false;
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

vector<SynthComponent*> Synth::establishProcessingOrder() {
    vector<SynthComponent*> allNodes = components;
    vector<SynthComponent*> noInputNodes = {};
    vector<SynthComponent*> processingOrder = {};

    inputs = 0;

    for (int i = allNodes.size() - 1; i >= 0; i--) {
        SynthComponent* node = allNodes[i];
        if (node->incomingConnections.size() == 0) {
            allNodes.erase(allNodes.begin() + i);
            noInputNodes.push_back(node);
            inputs++;
        }
    }

    while (noInputNodes.size() > 0) {
        SynthComponent* current = noInputNodes.at(0);
        processingOrder.push_back(current);
        noInputNodes.erase(noInputNodes.begin());

        for (auto& conn : current->outgoingConnections) {
            conn->visited = true;
            bool noIncoming = true;
            for (auto& conn1 : conn->destination->parent->incomingConnections) {
                if (!conn1->visited) {
                    noIncoming = true;
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

    return processingOrder;
}