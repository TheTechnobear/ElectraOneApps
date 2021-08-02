#include "Debug.h"
#include <stdarg.h>
#include <stdio.h>

//logMessage
#include <helpers.h>

#define LOG_MESSAGE_MAX_SIZE 200

static DebugWindow debugWindow_;


struct dbgRingBuffer {
    dbgRingBuffer() {
        for (unsigned i = 0; i < MAX_MSGS; i++) {
            buffer_[i][0] = 0;
        }
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

    bool full() {
        return ((wrPos_ + 1) % MAX_MSGS == rdPos_);
    }

    unsigned n() {
        if (wrPos_ < rdPos_) { return (wrPos_ + MAX_MSGS) - rdPos_; }
        return wrPos_ - rdPos_;
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

void DebugWindow::paint(void) {
    screen.fillRect(screenX, screenY, width, height, COLOR_BLACK);
    Window::paint();
    static const unsigned th = 20;
    static const unsigned nMsgDisp = (height - 50) / th;
    unsigned sidx = (dbgBuf_.wrPos_ + dbgRingBuffer::MAX_MSGS - nMsgDisp) % dbgRingBuffer::MAX_MSGS;
    for (unsigned i = 0; i < nMsgDisp; i++) {
        unsigned idx = (sidx + i) % dbgRingBuffer::MAX_MSGS;
        const char *msg = dbgBuf_.buffer_[idx];
        screen.printText(screenX + 10, screenY + 10 + (i * th), msg,
                         TextStyle::smallWhiteOnBlack, width,
                         TextAlign::left);
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
    char *msg = nullptr;
    while ((msg = dbgBuf_.getReadPos())) {
        logMessage(msg);
    }
}

