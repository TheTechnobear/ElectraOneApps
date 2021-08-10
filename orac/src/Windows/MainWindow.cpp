#include "MainWindow.h"

#include "../Debug.h"

#include "../ElectraApp.h"


void MainWindow::potMove(uint8_t potId, int16_t relativeChange) {
    if (potId == 0) {
        if (buttonState_[BUTTON_LEFT_TOP]) {
            buttonHandled(BUTTON_LEFT_TOP);
            if (!getActiveRack()) return;

            if (relativeChange > 0)
                getActiveRack()->nextDisplay();
            else
                getActiveRack()->prevDisplay();
            return;
        } else if (buttonState_[BUTTON_LEFT_MIDDLE]) {
            buttonHandled(BUTTON_LEFT_MIDDLE);
            if (!getActiveRack() || !getActiveRack()->getActiveModule()) return;

            if (relativeChange > 0)
                getActiveRack()->getActiveModule()->nextDisplay();
            else
                getActiveRack()->getActiveModule()->prevDisplay();
            return;
        }
    }
    dbgMessage("MainWindows::potMove %d, %d, %d", potId, relativeChange, buttonState_[BUTTON_LEFT_TOP]);
    OWindow::potMove(potId, relativeChange);
}

void MainWindow::potTouchUp(uint8_t potId) {
    if (potId == 0) {
        if (buttonState_[BUTTON_LEFT_TOP]) {
            return;
        } else if (buttonState_[BUTTON_LEFT_MIDDLE]) {
            return;
        }
    }
    OWindow::potTouchUp(potId);
}

void MainWindow::potTouchDown(uint8_t potId) {
    if (potId == 0) {
        if (buttonState_[BUTTON_LEFT_TOP]) {
            return;
        } else if (buttonState_[BUTTON_LEFT_MIDDLE]) {
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
            if (getActiveRack()) getActiveRack()->prevDisplay();
            break;
        }
        case BUTTON_LEFT_MIDDLE : {
            if (getActiveRack()) getActiveRack()->nextDisplay();
            break;
        }
        case BUTTON_LEFT_BOTTOM: {
            break;
        }
        case BUTTON_RIGHT_TOP : {
            if (getActiveRack() && getActiveRack()->getActiveModule()) getActiveRack()->getActiveModule()->prevDisplay();
            break;
        }
        case BUTTON_RIGHT_MIDDLE : {
            if (getActiveRack() && getActiveRack()->getActiveModule()) getActiveRack()->getActiveModule()->nextDisplay();
            break;
        }
        case BUTTON_RIGHT_BOTTOM: {
            // TODO : improve, don't use cast
            auto eapp = static_cast<ElectraApp *>(app);
            eapp->getAppWindows()->select(AppWindows::MENU);
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

            rack->setBounds(screenX, screenY + header, width, height - header);
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
