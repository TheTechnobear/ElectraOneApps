#include "Debug.h"
#include <cstdarg>
#include <cstdio>

#include <Buttons.h>

//TODO : avoid using electrapp !
#include "ElectraApp.h"

//logMessage
#include <helpers.h>

#define LOG_MESSAGE_MAX_SIZE 200

static DebugWindow debugWindow_;


struct dbgRingBuffer {
    dbgRingBuffer() {
        reset();
    }

    char *getReadPos() {
        if (rdPos_ == wrPos_) return nullptr;

        char *v = buffer_[rdPos_];
        rdPos_ = (rdPos_ + 1) % MAX_MSGS;
        return v;
    }

    char *getWritePos() {
        char *v = buffer_[wrPos_];
        wrPos_ = (wrPos_ + 1) % MAX_MSGS;
        return v;
    }

    bool full() const {
        return ((wrPos_ + 1) % MAX_MSGS == rdPos_);
    }

    unsigned n() const {
        if (wrPos_ < rdPos_) { return (wrPos_ + MAX_MSGS) - rdPos_; }
        return wrPos_ - rdPos_;
    }

    void reset() {
        for (auto &i : buffer_) {
            i[0] = 0;
        }
        wrPos_ = 0;
        rdPos_ = 0;
    }

    friend class DebugWindow;

    static unsigned constexpr MAX_MSGS = 50;
    volatile unsigned wrPos_ = 0;
    volatile unsigned rdPos_ = 0;
    char buffer_[MAX_MSGS][LOG_MESSAGE_MAX_SIZE];
} dbgBuf_;


DebugWindow *DebugWindow::debugWindow() {
    return &debugWindow_;
}

DebugWindow::DebugWindow(uint16_t newX, uint16_t newY, uint16_t newWidth, uint16_t newHeight, bool newPinOptionAvailable, bool newWindowPinned) :
    OWindow(newX, newY, newWidth, newHeight,
            newPinOptionAvailable, newWindowPinned) {
    doNotUseControlSets();
}

void DebugWindow::paint(Graphics& g) {
    g.fillRect(screenX, screenY, width, height, COLOR_BLACK);
    Window::paint(g);
    static const unsigned th = 20;
    static const unsigned nMsgDisp = (height - 50) / th;
    unsigned sidx = (dbgBuf_.wrPos_ + dbgRingBuffer::MAX_MSGS - nMsgDisp) % dbgRingBuffer::MAX_MSGS;
    for (unsigned i = 0; i < nMsgDisp; i++) {
        unsigned idx = (sidx + i) % dbgRingBuffer::MAX_MSGS;
        const char *msg = dbgBuf_.buffer_[idx];
        g.printText(screenX + 10, screenY + 10 + (i * th), msg,
                         TextStyle::smallWhiteOnBlack, width,
                         TextAlign::left);
    }
}

void DebugWindow::buttonUp(uint8_t buttonId) {
    if(isButtonHandled(buttonId) || !isVisible()) return;
    OWindow::buttonUp(buttonId);

    switch (buttonId) {
        case BUTTON_LEFT_TOP: {
            dbgBuf_.reset();
            repaint();
            break;
        }
        case BUTTON_LEFT_BOTTOM: {
            flushDebug();
            break;
        }
        case BUTTON_RIGHT_BOTTOM : {
            // TODO : improve, don't use cast
            auto eapp = static_cast<ElectraApp *>(app);
            eapp->getAppWindows()->select(AppWindows::MAIN);
            break;
        }
        default: {
        }
    }
}



void dbgMessage(const char *format, ...) {
    char *buf = dbgBuf_.getWritePos();
    va_list ap;
    va_start(ap, format);
    vsnprintf(buf, LOG_MESSAGE_MAX_SIZE, format, ap);
    va_end(ap);

    if (DebugWindow::debugWindow()) DebugWindow::debugWindow()->repaint();
}

void flushDebug() {
    char *msg;
    while ((msg = dbgBuf_.getReadPos())) {
        logMessage(msg);
    }
}

