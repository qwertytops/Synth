#include "Synth.hpp"

double Synth::MakeSound(double elapsed) {
    double result = 0.0;
    vector<Note> notesToKeep = {};
    mtx.lock();
    for (auto& note : notesBeingPlayed) {
        double noteRes = 0;
        for (auto osc : oscillators) {
            noteRes += osc->run(elapsed, note.midiNum);
        }
        bool keepNote = false;
        for (auto env : envelopes) {
            double amp = env->GetAmplitude(elapsed, &note);
            noteRes *= amp;
            if (note.active || amp > 0.0001) {
                keepNote = true;
            }
        }
        if (envelopes.empty()) {
            if (note.active) keepNote = true;
        }
        if (keepNote) {
            notesToKeep.push_back(note);
        }
        result += noteRes;
    }
    for (auto& filter : filters) {
        result = filter->run(result);
    }
    notesBeingPlayed = std::move(notesToKeep);
    mtx.unlock();
    return result * volume;
}

Synth::Synth(std::vector<Oscillator*> oscs, std::vector<ADSR*> envs, vector<LPF*> filters, int octave)
    : oscillators(oscs),
      envelopes(envs),
      filters(filters),
      player([this](double elapsed) { return this->MakeSound(elapsed); })
{
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
                for (auto &note : notesBeingPlayed) {
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
