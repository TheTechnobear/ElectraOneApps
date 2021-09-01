#pragma once

#include <window.h>

#include "Windows/OWindow.h"

class DebugWindow : public OWindow {
public:
    explicit DebugWindow(uint16_t newX = 0,
                         uint16_t newY = 20,
                         uint16_t newWidth = 1024,
                         uint16_t newHeight = 562,
                         bool newPinOptionAvailable = false,
                         bool newWindowPinned = false);

    void paint(Graphics& g) override;

    void buttonUp(uint8_t buttonId) override;
    static DebugWindow *debugWindow();
};

void dbgMessage(const char *format, ...);
void flushDebug();