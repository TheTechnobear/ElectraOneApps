#pragma once

#include <window.h>



class DebugWindow : public Window {
public:
    explicit DebugWindow(uint16_t newX = 0,
        uint16_t newY = 20,
        uint16_t newWidth = 1023,
        uint16_t newHeight = 561,
    bool newPinOptionAvailable = false,
    bool newWindowPinned = false) :
    Window({Window::Type::page, newX, newY, newWidth, newHeight,
            newPinOptionAvailable, newWindowPinned}) {
        doNotUseControlSets();
    }
    void paint(void) override;

    static DebugWindow* debugWindow();
};

void dbgMessage(const char *format, ...);
void flushDebug();