#include "MainWindow.h"

#include "../Debug.h"

#include "../ElectraApp.h"


void MainWindow::potMove(uint8_t potId, int16_t relativeChange) {
    if (buttonState_[BUTTON_LEFT_TOP]) {
        if (potId == 0) {
            if (!getActiveRack()) return;
            buttonHandled(BUTTON_LEFT_TOP);

            if (relativeChange > 0)
                getActiveRack()->nextDisplay();
            else
                getActiveRack()->prevDisplay();
            return;
        } else if (potId == 1) {
            if (!getActiveRack() || !getActiveRack()->getActiveModule()) return;
            buttonHandled(BUTTON_LEFT_TOP);

            if (relativeChange > 0)
                getActiveRack()->getActiveModule()->nextDisplay();
            else
                getActiveRack()->getActiveModule()->prevDisplay();
            return;
        }
    }
    OWindow::potMove(potId, relativeChange);
}

void MainWindow::potTouchUp(uint8_t potId) {
    if (potId == 0 || potId == 1) {
        if (buttonState_[BUTTON_LEFT_TOP]) {
            return;
        }
    }
    OWindow::potTouchUp(potId);
}

void MainWindow::potTouchDown(uint8_t potId) {
    if (potId == 0 || potId == 1) {
        if (buttonState_[BUTTON_LEFT_TOP]) {
            return;
        }
    }
    OWindow::potTouchDown(potId);
}


void MainWindow::buttonDown(uint8_t buttonId) {
    OWindow::buttonDown(buttonId);
    buttonState_[buttonId] = true;
    buttonLongHold_[buttonId] = false;
}

void MainWindow::buttonLongHold(uint8_t buttonId) {
    OWindow::buttonLongHold(buttonId);
    buttonLongHold_[buttonId] = true;
}


void MainWindow::buttonUp(uint8_t buttonId) {

    buttonState_[buttonId] = false;
    if (isButtonHandled(buttonId) || !isVisible()) return;
    OWindow::buttonUp(buttonId);

    if(sleepMode_ && buttonId!=BUTTON_RIGHT_BOTTOM) {
        return;
    }

    switch (buttonId) {
        case BUTTON_LEFT_TOP : {
            break;
        }
        case BUTTON_LEFT_MIDDLE : {
            break;
        }
        case BUTTON_LEFT_BOTTOM: {
            if (buttonLongHold_[BUTTON_RIGHT_BOTTOM]) {
                // standby mode
                buttonHandled(BUTTON_RIGHT_BOTTOM);
                buttonLongHold_[BUTTON_RIGHT_BOTTOM]=false;
                enableSleepMode();
            } else {
                nextMode();
            }
            break;
        }
        case BUTTON_RIGHT_TOP : {
            // TODO : improve, don't use cast
            auto eapp = static_cast<ElectraApp *>(app);
            eapp->selectModule();
            eapp->getAppWindows()->select(AppWindows::MODULE);
            break;
        }
        case BUTTON_RIGHT_MIDDLE : {
            auto eapp = static_cast<ElectraApp *>(app);
            eapp->getAppWindows()->select(AppWindows::PRESET);
            break;
            break;
        }
        case BUTTON_RIGHT_BOTTOM: {
            if (buttonLongHold_[buttonId]) {
                if(sleepMode_) {
                    disableSleepMode();
                }
                else {
                    auto eapp = static_cast<ElectraApp *>(app);
                    eapp->getAppWindows()->select(AppWindows::DEBUG);
                }
            }
            break;
        }
        default: {
            break;
        }
    }
    buttonLongHold_[buttonId] = false;
}

void MainWindow::addRack(const Kontrol::Rack &r) {
    auto i = racks_.find(r.id());
    if (i == racks_.end()) {
        auto rack = std::make_shared<OracRack>(r);
        racks_[r.id()] = rack;

        if (racks_.size() == 1) {
            // we only ever have one rack!
            unsigned header = 40; // status bar = 15

            rack->setBounds(screenX, screenY + header, width, height - header - 30);
            addComponent(rack.get());
            for (unsigned enc = 0; enc < 12; enc++) {
                assignPot(enc, 0, rack.get(), enc);
            }
            rack->setVisible(isVisible());
            rack->repaint();
        }
    }
}


void MainWindow::paint() {
    screen.fillRect(screenX, screenY, width, height, COLOR_BLACK);
    Window::paint();

    static std::string mode_str[MAX_MODE] = {
        "",
        "MIDI LEARN",
        "MOD LEARN",
    };

    auto &mode = mode_str[editMode_];
    screen.printText(screenX + 20, screenY + height - 30, mode.c_str(), TextStyle::mediumWhiteOnBlack, width, TextAlign::left);
}

void MainWindow::nextMode() {
    editMode_ = static_cast<EditMode>((editMode_ + 1) % MAX_MODE);
    auto eapp = static_cast<ElectraApp *>(app);

    switch (editMode_) {
        case NORMAL: {
            eapp->midiLearn(false);
            eapp->modulationLearn(false);
            break;
        }
        case MIDI_LEARN : {
            eapp->modulationLearn(false);
            eapp->midiLearn(true);
            break;
        }

        case MOD_LEARN : {
            eapp->midiLearn(false);
            eapp->modulationLearn(true);
            break;
        }
        default: {
            eapp->midiLearn(false);
            eapp->modulationLearn(false);
            break;
            break;
        }
    }
    repaint();
}

void MainWindow::resetMode() {
    if (editMode_ != NORMAL) {
        editMode_ = NORMAL;

        auto eapp = static_cast<ElectraApp *>(app);
        eapp->midiLearn(false);
        eapp->modulationLearn(false);
        repaint();
    }
}


void MainWindow::enableSleepMode() {
    for (int i = 0; i < 16000; i += 128) {
        screen.setBacklightbrightness(i);
        delay(10);
    }
    screen.setBacklightbrightness(65535);
    sleepMode_=true;
}

void MainWindow::disableSleepMode() {
    logMessage("hardwareButtonLambda: switch LCD on");
    sleepMode_=false;
    repaint();

    delay(50);
    screen.setBacklightbrightness(0);
    delay(150);

    capSense.reset();

    if (!capSense.init()) {
        logMessage("capacitive sensing: cannot calibrate. Check input USB voltage");
    }
}