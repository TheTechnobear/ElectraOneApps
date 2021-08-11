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

void MainWindow::buttonUp(uint8_t buttonId) {
    buttonState_[buttonId] = false;
    if (isButtonHandled(buttonId) || !isVisible()) return;
    OWindow::buttonUp(buttonId);

    switch (buttonId) {
        case BUTTON_LEFT_TOP : {
            break;
        }
        case BUTTON_LEFT_MIDDLE : {
            break;
        }
        case BUTTON_LEFT_BOTTOM: {
            nextMode();
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
            break;
        }
        default: {
            break;
        }
    }
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

void MainWindow::buttonLongHold(uint8_t buttonId) {
    OWindow::buttonLongHold(buttonId);
    if (buttonId == BUTTON_RIGHT_BOTTOM) {
        buttonHandled(buttonId);

        // TODO : improve, don't use cast
        auto eapp = static_cast<ElectraApp *>(app);
        eapp->getAppWindows()->select(AppWindows::DEBUG);
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


