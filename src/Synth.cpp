#include "Synth.hpp"
#include "Input.hpp"
#include "Connection.hpp"

#include <iostream>
#include <unordered_set>
#include <cmath>

double Synth::MakeSound(double elapsed) {
    mtx.lock();

    for (auto& input : getAllInputs()) {
        input->reset();
    }

    // cout << "nbp: " << notesBeingPlayed.size() << endl;
    // cout << "inputs count: " << inputs << endl;

    // auto inputsList = getAllInputs();
    // for (auto* in : inputsList) {
    //     cout << "Input " << in->name << " pairs before reset: " << in->pairs.size() << endl;
    // }
    
    for (auto& note : notesBeingPlayed) {
        for (int i = 0; i < inputs; i++) {
            components[i]->inputs.at(0)->pairs.push_back(make_pair(note, 0));
        }
    }
    for (auto& component : components) {
        // cout << "running " << component->name << component->id << endl;
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
        if (n->active) { // add something like || std::fabs(amplitude) > AMPLITUDE_THRESH
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
    return result * volume;
}

Synth::Synth(int octave)
    : player([this](double elapsed) { return this->MakeSound(elapsed); })
{
    mainOut = new Input("Main Out");
    processingOrder = establishProcessingOrder();
    player.Start();
    thread input([this, octave]() { this->ProcessInput(octave); });
    input.detach();
}


void Synth::addComponent(SynthComponent* comp) {
    components.push_back(comp);
    processingOrder = establishProcessingOrder();
}

vector<Input*> Synth::getAllInputs() {
    vector<Input*> result = {};

    result.push_back(mainOut);

    for (auto& comp : components) {
        for (auto& inp : comp->inputs) {
            result.push_back(inp);
        }
    }

    return result;
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

            if (conn->destination->parent == nullptr) {
                cout << "ignoring main out" << endl;
                continue;
            }

            for (auto& conn1 : conn->destination->parent->incomingConnections) {
                if (!conn1->visited) {
                    noIncoming = false; // chatgpt said it should be false but it was true before
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