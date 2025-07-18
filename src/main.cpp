#include <QApplication>
#include <QLabel>
#include <QSlider>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

#include <ApplicationServices/ApplicationServices.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <vector>
#include <termios.h>
#include <unistd.h>
#include <cstdio>

#include "UI/OscillatorControlWidget.hpp"
#include "UI/SynthControlWidget.hpp"
#include "UI/ADSRControlWidget.hpp"
#include "UI/BiquadFilterControlWidget.hpp"

#include "CoreAudioWaveMaker.hpp"
#include "Synth.hpp"
#include "Note.hpp"

using namespace std;

CGKeyCode keyCodes[18] = {
    0, 13, 1, 14, 2, 3, 17, 5, 16,
    4, 32, 38, 40, 31, 37, 35, 41, 39
};

Oscillator o1(WaveType::SINE, 4);
Oscillator o2(WaveType::SINE, 3);

ADSR env1(0.01, 0.01, 1.0, 0.01);

BiquadFilter filter1(FilterMode::PEAK, 440, 1, 3);

Synth synth({&o1, &o2}, {&env1}, {&filter1}, 0);

bool isKeyPressed(CGKeyCode keycode) {
    return CGEventSourceKeyState(kCGEventSourceStateHIDSystemState, keycode);
}

void ProcessInput() {
    while (true) {
        synth.mtx.lock();
        for (int k = 0; k < 18; k++) {
            synth.keys[k] = isKeyPressed(keyCodes[k]);
        }
        synth.mtx.unlock();
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

struct TerminalEchoGuard {
    termios oldt;
    TerminalEchoGuard() {
        tcgetattr(STDIN_FILENO, &oldt);
        termios newt = oldt;
        newt.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    }
    ~TerminalEchoGuard() {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }
};

int main(int argc, char* argv[]) {
    TerminalEchoGuard guard;

    // Start input thread
    thread inputThread(ProcessInput);

    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("Synth Control");
    window.setMinimumWidth(250*synth.oscillators.size());

    QVBoxLayout* layout = new QVBoxLayout;
    QHBoxLayout* oscillatorPanels = new QHBoxLayout;

    QLabel* label = new QLabel("Synth");

    layout->addWidget(label);

    SynthControlWidget* synthControls = new SynthControlWidget(&synth);
    layout->addWidget(synthControls);

    layout->addLayout(oscillatorPanels);

    for (size_t i = 0; i < synth.oscillators.size(); i++) {
        Oscillator* osc = synth.oscillators[i];

        OscillatorControlWidget* oscControls = new OscillatorControlWidget(osc);

        oscillatorPanels->addWidget(oscControls);
    }

    for (size_t i = 0; i < synth.envelopes.size(); i++) {
        ADSR* adsr = synth.envelopes[i];

        ADSRControlWidget* adsrControls = new ADSRControlWidget(adsr);

        layout->addWidget(adsrControls);
    }

    for (size_t i = 0; i < synth.filters.size(); i++) {
        BiquadFilter* filter = synth.filters[i];

        BiquadFilterControlWidget* filterControls = new BiquadFilterControlWidget(filter);

        layout->addWidget(filterControls);
    }

    QPushButton* quitButton = new QPushButton("Quit");

    layout->addWidget(quitButton);
    window.setLayout(layout);
    
    QObject::connect(quitButton, &QPushButton::clicked, [&]() {
        app.quit();
    });

    window.show();

    int result = app.exec();

    inputThread.detach();
    return result;
}
