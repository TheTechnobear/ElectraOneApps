#pragma once

#include "OWindow.h"

#include "Components/ORack.h"

#include "ElectraApp.h"

class MainWindow : public OWindow {
public:
    explicit MainWindow(uint16_t newX = 0,
                        uint16_t newY = 20,
                        uint16_t newWidth = 1023,
                        uint16_t newHeight = 561,
                        bool newPinOptionAvailable = false,
                        bool newWindowPinned = false) :
        OWindow(newX, newY, newWidth, newHeight,
                newPinOptionAvailable, newWindowPinned) {
        doNotUseControlSets();
    }

    void paint() override {
        screen.fillRect(screenX, screenY, width, height, COLOR_BLACK);
        Window::paint();
    }

    void addRack(const Kontrol::Rack &r) {
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

    std::shared_ptr<OracRack> getActiveRack() {
        if (!racks_.empty()) {
            return racks_.begin()->second;
        }
        return nullptr;
    }

    std::shared_ptr<OracRack> getRack(const Kontrol::EntityId &id) {
        auto i = racks_.find(id);
        if (i != racks_.end()) {
            return i->second;
        }
        return nullptr;
    }

    void buttonDown(uint8_t buttonId) override {
        OWindow::buttonDown(buttonId);
        buttonState_[buttonId] = true;
    }

    void buttonLongHold(uint8_t buttonId) override {
        OWindow::buttonLongHold(buttonId);
        if (buttonId == BUTTON_RIGHT_BOTTOM) {
            buttonHandled(buttonId);

            // TODO : improve, don't use cast
            auto eapp = static_cast<ElectraApp *>(app);
            eapp->getAppWindows()->select(AppWindows::DEBUG);
        }
    }

    void buttonUp(uint8_t buttonId) override {
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

    void visibilityChanged() override {
        if (getActiveRack()) getActiveRack()->setVisible(isVisible());
    }

    void potTouchDown(uint8_t potId) override {
        if (potId == 0) {
            if (buttonState_[BUTTON_LEFT_TOP]) {
                return;
            } else if (buttonState_[BUTTON_LEFT_MIDDLE]) {
                return;
            }
        }
        OWindow::potTouchDown(potId);
    }

    void potTouchUp(uint8_t potId) override {
        if (potId == 0) {
            if (buttonState_[BUTTON_LEFT_TOP]) {
                return;
            } else if (buttonState_[BUTTON_LEFT_MIDDLE]) {
                return;
            }
        }
        OWindow::potTouchUp(potId);
    }

    void potMove(uint8_t potId, int16_t relativeChange) override {
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

private:
    bool buttonState_[NR_OF_HW_BUTTONS] = {false, false, false, false, false, false};
    std::map<Kontrol::EntityId, std::shared_ptr<OracRack>> racks_;
};