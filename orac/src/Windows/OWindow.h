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
    void touchDown(TouchEvent &touchEvent) override {}

    void touchUp(TouchEvent &touchEvent) override {}

    void touchHold(TouchEvent &touchEvent) override {}

    void touchLongHold(TouchEvent &touchEvent) override {}

    void touchClick(TouchEvent &touchEvent) override {}

    void touchDoubleClick(TouchEvent &touchEvent) override {}

    void potTouchDown(uint8_t potId) override {}

    void potTouchUp(uint8_t potId) override {}

    void potMove(uint8_t potId, int16_t relativeChange) override {}

    void buttonUp(uint8_t buttonId) override {}

    void buttonDown(uint8_t buttonId) override {}

    void buttonLongHold(uint8_t buttonId) override {};

    // Graphics
//    void visibilityChanged() override {
//        // nasty ;)
//        bool vis = isVisible();
//        for(auto components : getComponents()) {
//            components->setVisible(vis);
//        }
//
//    }


protected:
    ~OWindow() {}
};