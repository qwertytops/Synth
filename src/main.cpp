#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QSlider>
#include <QWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QScrollArea>

#include <ApplicationServices/ApplicationServices.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <vector>
#include <termios.h>
#include <unistd.h>
#include <cstdio>

#include "UI/EmptySlot.hpp"

#include "UI/Components/OscillatorControlWidget.hpp"
#include "UI/Components/SynthControlWidget.hpp"
#include "UI/Components/ADSRControlWidget.hpp"
#include "UI/Components/BiquadFilterControlWidget.hpp"

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

Synth synth({&o1, &o2, &env1, &filter1}, 0);

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

    QMainWindow window;
    window.setWindowTitle("SHA-A Archangel");
    window.showMaximized();

    // Scrollable area
    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);

    // Central scrollable content
    QWidget* centralWidget = new QWidget;
    QGridLayout* gridLayout = new QGridLayout;

    const int rows = 10;
    const int cols = 3;

    for (int r = 0; r < rows; ++r) {
        gridLayout->setRowStretch(r, 1);
        for (int c = 0; c < cols; ++c) {
            gridLayout->setColumnStretch(c, 1);

            EmptySlot* slot = new EmptySlot(&synth);
            gridLayout->addWidget(slot, r, c);
        }
    }

    centralWidget->setLayout(gridLayout);
    scrollArea->setWidget(centralWidget);

    // Set scroll area as central widget
    window.setCentralWidget(scrollArea);

    QObject::connect(&app, &QApplication::aboutToQuit, [&]() {
        inputThread.detach();
    });

    window.show();
    return app.exec();
}
