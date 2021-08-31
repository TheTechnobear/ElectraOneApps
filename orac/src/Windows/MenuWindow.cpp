#include "MenuWindow.h"

#include "../Debug.h"

#include "../ElectraApp.h"

#include <Buttons.h>

void MenuWindow::buttonUp(uint8_t buttonId) {
    if (isButtonHandled(buttonId) || !isVisible()) return;
    OWindow::buttonUp(buttonId);

    switch (buttonId) {
        case BUTTON_LEFT_TOP : {
            prev();
            break;
        }
        case BUTTON_LEFT_MIDDLE : {
            next();
            break;
        }
        case BUTTON_RIGHT_TOP : {
            select();
            break;
        }
        case BUTTON_RIGHT_MIDDLE : {
            back();
            break;
        }
        case BUTTON_RIGHT_BOTTOM : {
            // TODO : improve, don't use cast
            auto eapp = static_cast<ElectraApp *>(app);
            eapp->getAppWindows()->select(AppWindows::MAIN);
            break;
        }
        default: {
            // ignore
            break;
        }
    }
}

void MenuWindow::clearMenu(std::shared_ptr<MenuItem> &item) {
    // reset to top menu
    curMenu_ = topMenu_.get();
    topMenu_->idx_ = 0;
    topMenu_->offset_ = 0;

    // clear given menu
    item->items_.clear();
}

void MenuWindow::potMove(uint8_t potId, int16_t relativeChange) {
    if (potId == 0) {
        if (relativeChange > 0) {
            next();
        } else {
            prev();
        }
    }
}

void MenuWindow::next() {
    if (curMenu_->idx_ < (curMenu_->items_.size() - 1)) {
        curMenu_->idx_++;
        if ((curMenu_->idx_ - curMenu_->offset_) >= MAX_DISPLAY) {
            curMenu_->offset_ = curMenu_->idx_ - (MAX_DISPLAY - 1);
        }

        repaint();
    }
}

void MenuWindow::prev() {
    if (curMenu_->idx_ > 0) {
        curMenu_->idx_--;
        if ((curMenu_->idx_ - curMenu_->offset_) >= MAX_DISPLAY) {
            curMenu_->offset_ = curMenu_->idx_;
        }
        repaint();
    }
}

void MenuWindow::select() {
    if (curMenu_->idx_ >= 0 && curMenu_->idx_ < curMenu_->items_.size()) {
        auto i = curMenu_->items_[curMenu_->idx_];
        if (i->action_) {
            i->action_();
        } else {
            if (!i->items_.empty()) {
                curMenu_ = i.get();
            }
        }
        repaint();
    }
}

void MenuWindow::paintMenu(Graphics& g,MenuItem *m) {
    unsigned x = screenX + 40;
    unsigned y = screenY + 40;

    unsigned last = (m->offset_ + MAX_DISPLAY) > m->items_.size() ? m->items_.size() : m->offset_ + MAX_DISPLAY;
    for (unsigned idx = m->offset_; idx < last; idx++) {
        auto i = m->items_[idx];
        if (idx == m->idx_) {
            g.printText(x, y, i->name_.c_str(), TextStyle::mediumBlackOnWhite, 200, TextAlign::left);
        } else {
            g.printText(x, y, i->name_.c_str(), TextStyle::mediumWhiteOnBlack, 200, TextAlign::left);
        }
        y += 30;
    }
}

void MenuWindow::back() {
    if (curMenu_->parent_) {
        curMenu_ = curMenu_->parent_;
        repaint();
    }
}


void ModuleWindow::select(const std::string &m) {
    curMenu_ = topMenu_.get();
    curMenu_->idx_ = 0;
    curMenu_->offset_ = 0;
    size_t pos = m.find('/');
//    dbgMessage("ModuleWindow::select %s", m.c_str());
    if (pos == std::string::npos) {
        // module at top level
        int idx = curMenu_->findIdx(m);
        if (idx >= 0) {
            curMenu_->idx_ = idx;
            if (curMenu_->idx_ >= MAX_DISPLAY) curMenu_->offset_ = curMenu_->idx_;
        }
    } else {
        std::string cat = m.substr(0, pos + 1);
        std::string mod = m.substr(pos + 1);
//        dbgMessage("ModuleWindow::select %s %s %d", cat.c_str(), mod.c_str(), pos);

        auto catmenu = curMenu_->find(cat);
        if (!catmenu) {
            return;
        }

        // select cat
        int idx = curMenu_->findIdx(cat);
//        dbgMessage("ModuleWindow::select cat  %s %s %d", cat.c_str(), mod.c_str(), idx);
        if (idx >= 0) {
            curMenu_->idx_ = idx;
            if (curMenu_->idx_ >= MAX_DISPLAY) curMenu_->offset_ = curMenu_->idx_;
        }
        curMenu_ = catmenu.get();

        // select module
        idx = curMenu_->findIdx(mod);
//        dbgMessage("ModuleWindow::select mod %s %s %d", cat.c_str(), mod.c_str(), idx);
        if (idx >= 0) {
            curMenu_->idx_ = idx;
            if (curMenu_->idx_ >= MAX_DISPLAY) curMenu_->offset_ = curMenu_->idx_;
        } else {
            curMenu_->idx_ = 0;
        }
    }
}