#pragma once

#include <utility>
#include <memory>
#include <string>

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
            if (i->name_ == n) return i;
        }
        return nullptr;
    }

    int findIdx(const std::string &n) {
        unsigned idx = 0;
        for (const auto &i : items_) {
            if (i->name_ == n) {
                return idx;
            }
            idx++;
        }
        return -1;
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
                        uint16_t newWidth = 1024,
                        uint16_t newHeight = 562,
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

    void paintMenu(MenuItem *m);

    std::shared_ptr<MenuItem> getMenu() { return topMenu_; }

    void next();

    void prev();

    void select();

    void back();

    void potMove(uint8_t potId, int16_t relativeChange) override;

    void buttonUp(uint8_t buttonId) override;

    void clearMenu(std::shared_ptr<MenuItem> &item);

protected:
    static constexpr unsigned MAX_DISPLAY = 15;
    std::shared_ptr<MenuItem> topMenu_;
    MenuItem *curMenu_;
};


class ModuleWindow : public MenuWindow {
public:
    explicit ModuleWindow() = default;
    void select(const std::string &m);
};

class PresetWindow : public MenuWindow {
public:
    explicit PresetWindow() = default;
};
