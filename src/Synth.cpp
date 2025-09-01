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

    for (auto& note : notesBeingPlayed) {
        for (int i = 0; i < inputs; i++) {
            cout << processingOrder[i]->name << endl;
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
    : player([this](double elapsed) { return this->MakeSound(elapsed); })
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

void Synth::establishProcessingOrder() {
    cout << endl << "EPA" << endl;
    vector<SynthComponent *> allNodes = components;
    vector<SynthComponent*> noInputNodes = {};
    vector<SynthComponent*> order = {};

    inputs = 0;

    for (int i = allNodes.size() - 1; i >= 0; i--) {
        SynthComponent* node = allNodes[i];
        for (auto& conn : node->incomingConnections) {
            conn->visited = false;
        }
        cout << "checking in conns of node " << node->name << " | " << node->incomingConnections.size() << endl;
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
        cout << "current: " << current->name << endl;
        cout << "adding to order" << endl;
        order.push_back(current);
        noInputNodes.erase(noInputNodes.begin());

        cout << "checking current's outgoing connections" << endl;
        for (auto& conn : current->outgoingConnections) {
            cout << "visiting " << conn->destination->parent->name << endl;
            conn->visited = true;
            bool noIncoming = true;

            if (conn->destination->parent == nullptr) {
                continue;
            }

            for (auto& conn1 : conn->destination->parent->incomingConnections) {
                if (!conn1->visited) {
                    cout << "destination has other inputs: " << conn->destination->parent->name << endl;
                    noIncoming = false;
                    break;
                }
            }
            if (noIncoming) {
                SynthComponent* newNoInput = conn->destination->parent;
                cout << "new node with no input: " << newNoInput->name << endl;
                noInputNodes.push_back(newNoInput);

                vector<SynthComponent*>::iterator position = find(allNodes.begin(), allNodes.end(), newNoInput);

                cout << current->name << current->id << endl;
                cout << conn->destination->name << endl;
                cout << newNoInput->name << newNoInput->id << endl;

                allNodes.erase(position);
            }
        }
        cout << "noinputs size: " << noInputNodes.size() << endl;
    }

    processingOrder = std::move(order);

    cout << "\norder:\n";
    
    for (auto& c : processingOrder) {
        cout << c->name << c->id << endl;
    }
}