#pragma once

#include "OWindow.h"
#include "../Components/ORack.h"

#include <Buttons.h>


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

    void addRack(const Kontrol::Rack &r);

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

    void buttonLongHold(uint8_t buttonId) override;

    void buttonUp(uint8_t buttonId) override;

    void visibilityChanged() override {
        if (getActiveRack()) getActiveRack()->setVisible(isVisible());
    }

    void potTouchDown(uint8_t potId) override;

    void potTouchUp(uint8_t potId) override;

    void potMove(uint8_t potId, int16_t relativeChange) override;

private:
    bool buttonState_[NR_OF_HW_BUTTONS] = {false, false, false, false, false, false};
    std::map<Kontrol::EntityId, std::shared_ptr<OracRack>> racks_;
};