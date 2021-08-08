#pragma once

#include "OWindow.h"

#include "Components/ORack.h"

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

    void paint(void) override {
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
        if (racks_.size() > 0) {
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

    void buttonUp(uint8_t buttonId) override {
        switch (buttonId) {
            case BUTTON_LEFT_TOP : {
                if (getActiveRack()) {
                    getActiveRack()->prevDisplay();
                }
                break;
            }
            case BUTTON_LEFT_MIDDLE : {
                if (getActiveRack()) {
                    getActiveRack()->nextDisplay();
                }
                break;
            }
            case BUTTON_LEFT_BOTTOM: {
                break;
            }
            case BUTTON_RIGHT_TOP : {
                if (getActiveRack()->getActiveModule()) {
                    getActiveRack()->getActiveModule()->prevDisplay();
                }
                break;
            }
            case BUTTON_RIGHT_MIDDLE : {
                if (getActiveRack() && getActiveRack()->getActiveModule()) {
                    getActiveRack()->getActiveModule()->nextDisplay();
                }
                break;
            }
            case BUTTON_RIGHT_BOTTOM: {
                // note: masked by app
                break;
            }
        }
    }

private:
    std::map<Kontrol::EntityId, std::shared_ptr<OracRack>> racks_;
};