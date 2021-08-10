#pragma once

#include <Window.h>

#include "../IUICallback.h"

#include "../Debug.h"

class OWindow : public Window, public IUICallback {
public:
    explicit OWindow(uint16_t newX = 0,
                     uint16_t newY = 20,
                     uint16_t newWidth = 1023,
                     uint16_t newHeight = 561,
                     bool newPinOptionAvailable = false,
                     bool newWindowPinned = false) :
        Window(Window::Type::base, newX, newY, newWidth, newHeight,
               newPinOptionAvailable, newWindowPinned) {
        doNotUseControlSets();
    }

    //IUICallback
    void touchDown(TouchEvent &touchEvent) override {
        if (auto c = touchEvent.getEventComponent()) {
            c->onTouchDown(touchEvent);
        }
    }

    void touchUp(TouchEvent &touchEvent) override {
        if (auto c = touchEvent.getEventComponent()) {
            c->onTouchHold(touchEvent);
        }
    }

    void touchHold(TouchEvent &touchEvent) override {
        if (auto c = touchEvent.getEventComponent()) {
            c->onTouchUp(touchEvent);
        }
    }

    void touchLongHold(TouchEvent &touchEvent) override {
        if (auto c = touchEvent.getEventComponent()) {
            c->onTouchLongHold(touchEvent);
        }
    }

    void touchClick(TouchEvent &touchEvent) override {
        if (auto c = touchEvent.getEventComponent()) {
            c->onTouchClick(touchEvent);
        }
    }

    void touchDoubleClick(TouchEvent &touchEvent) override {
        if (auto c = touchEvent.getEventComponent()) {
            c->onTouchDoubleClick(touchEvent);
        }
    }

    void potTouchDown(uint8_t potId) override {
        processPotTouch(potId, true);
    }

    void potTouchUp(uint8_t potId) override {
        processPotTouch(potId, false);
    }

    void potMove(uint8_t potId, int16_t relativeChange) override {
        processPot(potId, relativeChange);
    }

    void buttonUp(uint8_t buttonId) override {
        buttonHandled(buttonId);
    }

    void buttonDown(uint8_t buttonId) override {
        buttonReset(buttonId);
    }

    void buttonLongHold(uint8_t buttonId) override {
    }

    // Graphics
    void visibilityChanged() override {
        bool vis = isVisible();
        for (auto components : getComponents()) {
            components->setVisible(vis);
        }
    }

    // button handling
    void buttonReset(uint8_t buttonId) {
        int mask = 1 << buttonId;
        processRelease_ |= mask;
    }

    void buttonHandled(uint8_t buttonId) {
        int mask = 1 << buttonId;
        processRelease_ &= ~mask;
    }

    bool isButtonHandled(uint8_t buttonId) {
        int mask = 1 << buttonId;
        return !(processRelease_ & mask);
    }

protected:
    int processRelease_ = 0x0;

    ~OWindow() {}
};