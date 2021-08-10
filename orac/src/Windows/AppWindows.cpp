#include "AppWindows.h"

void AppWindows::select(AppWindows::WindowType wt) {
    if (wt < windows_.size()) {
        auto ow = getCurrentWindow();
        idx_ = (unsigned) wt;
        auto w = getCurrentWindow();
        if (ow != w) {
            if (ow) ow->setVisible(false);
            if (w) w->setVisible(true);
            repaintCurrent();
        }
    }
}
