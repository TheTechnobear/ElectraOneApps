#pragma once

#include <PageWindow.h>
#include <Windows.h>

#include <vector>

#include "Debug.h"

class AppWindows : public Windows {
public:
    explicit AppWindows() {}

    void add(Window *w) {
        windows_.push_back(w);
        if (windows_.size() == 1) {
            idx_ = 0;
            w->setVisible(true);

        } else {
            w->setVisible(false);
        }
    }

    Window *getCurrentWindow(void) override {
        if (idx_ < windows_.size()) {
            return windows_[idx_];
        }
        return nullptr;
    }

    void repaintCurrent(void) override {
        auto w = getCurrentWindow();
        if (w) w->repaint();
    }

    void nextWindow() {
        auto sz = windows_.size();
        if (sz > 0) {
            auto ow = getCurrentWindow();
            idx_ = (idx_ + 1) % sz;
            auto w = getCurrentWindow();
            if (ow != w) {
                if (ow) ow->setVisible(false);
                if (w) w->setVisible(true);
                repaintCurrent();
            }
        }
    }

private:
    std::vector<Window *> windows_;
    unsigned idx_ = 0xFF;
};
