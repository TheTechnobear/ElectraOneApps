#pragma once

#include <utility>

#include "OWindow.h"


struct MenuItem;

struct MenuItem {
    MenuItem(std::string n, MenuItem *p)
        : name_(std::move(n)), action_(nullptr), parent_(p) {
    }

    MenuItem(std::string n, std::function<void(void)> fn)
        : name_(std::move(n)), action_(std::move(fn)), parent_(nullptr) {
    }

    void addItem(const std::string &n, std::function<void(void)> fn) {
        auto i = std::make_shared<MenuItem>(n, fn);
        items_.push_back(i);
    }

    std::shared_ptr<MenuItem> addMenu(const std::string &n) {
        auto i = std::make_shared<MenuItem>(n, this);
        items_.push_back(i);
        return i;
    }


    std::shared_ptr<MenuItem> find(const std::string &n) {
        for (auto i : items_) {
            if (i->name_ == n) {
                return i;
            }
        }
        return nullptr;
    }

    std::string name_;
    std::function<void(void)> action_;
    std::vector<std::shared_ptr<MenuItem>> items_;

    MenuItem *parent_;
    unsigned offset_ = 0;
    unsigned idx_ = 0;
};

class MenuWindow : public OWindow {
public:
    explicit MenuWindow(uint16_t newX = 0,
                        uint16_t newY = 20,
                        uint16_t newWidth = 1023,
                        uint16_t newHeight = 561,
                        bool newPinOptionAvailable = false,
                        bool newWindowPinned = false) :
        OWindow(newX, newY, newWidth, newHeight,
                newPinOptionAvailable, newWindowPinned) {
        doNotUseControlSets();
        topMenu_ = std::make_shared<MenuItem>("Main", nullptr);
        curMenu_ = topMenu_.get();
    }


    void paint() override {
        screen.fillRect(screenX, screenY, width, height, COLOR_BLACK);
        Window::paint();

        paintMenu(curMenu_);
    }

    void paintMenu(MenuItem *m) {
        unsigned x = screenX + 40;
        unsigned y = screenY + 40;

        unsigned last = (m->offset_ + MAX_DISPLAY) > m->items_.size() ? m->items_.size() : m->offset_ + MAX_DISPLAY;
        for (unsigned idx = m->offset_; idx < last; idx++) {
            auto i = m->items_[idx];
            if (idx == m->idx_) {
                screen.printText(x, y, i->name_.c_str(), TextStyle::mediumBlackOnWhite, 200, TextAlign::left);
            } else {
                screen.printText(x, y, i->name_.c_str(), TextStyle::mediumWhiteOnBlack, 200, TextAlign::left);
            }
            y += 30;
        }
    }

    std::shared_ptr<MenuItem> getMenu() { return topMenu_; }

    void next() {
        if (curMenu_->idx_ < (curMenu_->items_.size() - 1)) {
            curMenu_->idx_++;
            if ((curMenu_->idx_ - curMenu_->offset_) >= MAX_DISPLAY) {
                curMenu_->offset_ = curMenu_->idx_ - (MAX_DISPLAY - 1);
            }

            repaint();
        }
    }

    void prev() {
        if (curMenu_->idx_ > 0) {
            curMenu_->idx_--;
            if ((curMenu_->idx_ - curMenu_->offset_) >= MAX_DISPLAY) {
                curMenu_->offset_ = curMenu_->idx_;
            }
            repaint();
        }
    }

    void select() {
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

    void back() {
        if (curMenu_->parent_) {
            curMenu_ = curMenu_->parent_;
            repaint();
        }
    }

    void potMove(uint8_t potId, int16_t relativeChange) override {
        if (potId == 0) {
            if (relativeChange > 0) {
                next();
            } else {
                prev();
            }
        }
    }

    void buttonUp(uint8_t buttonId) override {
        if(isButtonHandled(buttonId) || !isVisible()) return;
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

    void clearMenu(std::shared_ptr<MenuItem> &item) {
        // reset to top menu
        curMenu_ = topMenu_.get();
        topMenu_->idx_ = 0;
        topMenu_->offset_ = 0;

        // clear given menu
        item->items_.clear();
    }

private:
    static constexpr unsigned MAX_DISPLAY = 15;
    std::shared_ptr<MenuItem> topMenu_;
    MenuItem *curMenu_;
};
