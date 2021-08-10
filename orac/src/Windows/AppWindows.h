#pragma once

#include <PageWindow.h>
#include <Windows.h>

#include <vector>


#include "OWindow.h"

class AppWindows : public Windows {
public:
    explicit AppWindows() = default;

    enum WindowType {
        MAIN,
        MODULE,
        PRESET,
        DEBUG,
        MAX_WINDOW
    };

    void add(OWindow *w) {
        windows_.push_back(w);
        if (windows_.size() == 1) {
            idx_ = 0;
            w->setVisible(true);

        } else {
            w->setVisible(false);
        }
    }

    Window *getCurrentWindow() override {
        if (idx_ < windows_.size()) {
            return windows_[idx_];
        }
        return nullptr;
    }


    OWindow *current() {
        if (idx_ < windows_.size()) {
            return windows_[idx_];
        }
        return nullptr;
    }

    void repaintCurrent() override {
        auto w = getCurrentWindow();
        if (w) w->repaint();
    }

    void select(enum WindowType wt);

    void nextWindow() {
        auto sz = windows_.size();
        if (sz > 0) {
            auto ow = getCurrentWindow();
            idx_ = idx_ == 0xFF ? 0 : ((idx_ + 1) % sz);
            auto w = getCurrentWindow();
            if (ow != w) {
                if (ow) ow->setVisible(false);
                if (w) w->setVisible(true);
                repaintCurrent();
            }
        }
    }

private:
    std::vector<OWindow *> windows_;
    unsigned idx_ = 0xFF;
};
