#pragma once

#include "OWindow.h"


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
        menu_ = std::make_shared<MenuItem>("Main");
    }

    struct MenuItem;

    struct MenuItem {
        MenuItem(const std::string &n)
            : name_(n), action_(nullptr) {
        }

        MenuItem(const std::string &n, std::function<void(void)> fn)
            : name_(n), action_(fn) {
        }

        void addItem(const std::string &n, std::function<void(void)> fn) {
            auto i = std::make_shared<MenuItem>(n, fn);
            items_.push_back(i);
        }

        std::shared_ptr<MenuItem> addMenu(const std::string &n) {
            auto i = std::make_shared<MenuItem>(n);
            items_.push_back(i);
            return i;
        }

        void next() {
            if (idx_ < (items_.size() - 1)) {
                idx_++;
            }
        }

        void prev() {
            if (idx_ > 0) {
                idx_--;
            }
        }

        void select() {
            if (idx_ >= 0 && idx_ < items_.size()) {
                auto i = items_[idx_];
                if (i->action_) {
                    i->action_();
                }
            }
        }

        void back() {
            if (items_.size()) {
            }
        }

        std::string name_;
        std::function<void(void)> action_;
        std::vector<std::shared_ptr<MenuItem>> items_;

        unsigned idx_ = 0;
    };

    void paint(void) override {
        screen.fillRect(screenX, screenY, width, height, COLOR_BLACK);
        Window::paint();

        paintMenu(menu_.get());
    }

    void paintMenu(MenuItem *m) {
        unsigned x = screenX + 40;
        unsigned y = screenY + 40;

        unsigned idx = 0;

        for (auto i: m->items_) {
            if (idx == m->idx_) {
                screen.printText(x, y, i->name_.c_str(), TextStyle::mediumBlackOnWhite, 200, TextAlign::left);
            } else {
                screen.printText(x, y, i->name_.c_str(), TextStyle::mediumWhiteOnBlack, 200, TextAlign::left);
            }
            y += 30;
            idx++;
        }
    }

    std::shared_ptr<MenuItem> getMenu() { return menu_; }

    void buttonUp(uint8_t buttonId) override {
        switch (buttonId) {
            case BUTTON_LEFT_TOP : {
                menu_->prev();
                repaint();
                break;
            }
            case BUTTON_LEFT_MIDDLE : {
                menu_->next();
                repaint();
                break;
            }
            case BUTTON_LEFT_BOTTOM : {
                menu_->select();
                repaint();
                break;
            }
            default: {
                // ignore
                break;
            }
        }
    }

private:


    std::shared_ptr<MenuItem> menu_;
};
