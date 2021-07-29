#pragma once

#include <PageWindow.h>
#include <Windows.h>

class AppWindows : public Windows {
public:
    explicit AppWindows() : currentWindow_(nullptr) {}

    void setCurrentWindow(Window *newCurrentWindow) {
        currentWindow_ = newCurrentWindow;
    }

    Window *getCurrentWindow(void) override { return currentWindow_; }

    void repaintCurrent(void) override {
        if (currentWindow_)
            currentWindow_->repaint();
    }

private:
    Window *currentWindow_;
};
