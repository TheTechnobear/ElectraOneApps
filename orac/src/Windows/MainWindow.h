#pragma once

#include <Window.h>


#include "Components/ORack.h"

class MainWindow : public Window {
public:
    explicit MainWindow(uint16_t newX = 0,
               uint16_t newY = 20,
               uint16_t newWidth = 1023,
               uint16_t newHeight = 561,
               bool newPinOptionAvailable = false,
               bool newWindowPinned = false) :
        Window({Window::Type::page, newX, newY, newWidth, newHeight,
                newPinOptionAvailable, newWindowPinned}) {
        doNotUseControlSets();
    }

    void paint(void) override {
        screen.fillRect(screenX, screenY, width - 1, height - 1, COLOR_BLACK);

        Window::paint();
    }

    void addRack(const Kontrol::Rack &r) {
        auto i = racks_.find(r.id());
        if (i == racks_.end()) {
            auto rack = std::make_shared<OracRack>(r);
            racks_[r.id()] = rack;

            if (racks_.size() == 1) {
                // we only ever have one rack!
                unsigned header = 30; // status bar = 15
                rack->setBounds(screenX, screenY + header, width, height - header);
                addAndMakeVisible(rack.get());
                rack->repaint();

                for (unsigned enc = 0; enc < 12; enc++) {
                    assignPot(enc, 0, rack.get(), enc);
                }
            }
        }
    }


    std::shared_ptr<OracRack> getRack(const Kontrol::EntityId &id) {
        auto i = racks_.find(id);
        if (i != racks_.end()) {
            return i->second;
        }
        return nullptr;
    }

private:
    std::map<Kontrol::EntityId, std::shared_ptr<OracRack>> racks_;

};