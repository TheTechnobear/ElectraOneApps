#include "OWindow.h"

#include "../Debug.h"

void OWindow::buttonReset(uint8_t buttonId) {
    int mask = 1 << buttonId;
    processRelease_ |= mask;
}

void OWindow::buttonHandled(uint8_t buttonId) {
    int mask = 1 << buttonId;
    processRelease_ &= ~mask;
}

bool OWindow::isButtonHandled(uint8_t buttonId) {
    int mask = 1 << buttonId;
    return !(processRelease_ & mask);
}

void OWindow::potMove(uint8_t potId, int16_t relativeChange) {
    Window::processPot(potId, relativeChange);
}

void OWindow::processPot(uint8_t potId, int16_t relativeChange) {
    // TODO: TaskReadPots - should NOT directly use this, rather use potMove
    // !! NOP !!
}
