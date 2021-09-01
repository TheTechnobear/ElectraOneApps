#pragma once

#include <Component.h>
#include <Colours.h>

// static int16_t rgbToRgb565(uint8_t r, uint8_t g, uint8_t b) {
//   uint16_t rgb565 = 0;
//   rgb565 += uint16_t(r & 0b011111) << 11;
//   rgb565 += uint16_t(g & 0b011111) << 5;
//   rgb565 += (b & 0b011111);
//   return rgb565;
// }


// text height s=12, m = 16, L =18
class OComponent : public Component {
public:
    explicit OComponent(OComponent *parent = nullptr) :
        active_(false),
        dimmed_(false),
        parent_(parent) {
    }

    ~OComponent() override = default;

    std::function<bool(void)> onClick;

    // Component
    void setDimmed(bool d) override { dimmed_ = d; }

    bool isDimmed() const override { return dimmed_; }

    void setActive(bool a) override { active_ = a; }

    bool isActive() const override { return (active_); }

    void onPotTouchUp(handle_t handle) override {}

    void onPotTouchDown(handle_t handle) override {
        // if (onClick) {
        //   onClick();
        // }
    }

    void assignToWindow(Window *window) override {
        Component::assignToWindow(window);
        for (auto &c : children_) {
            c->assignToWindow(window);
        }
    }

    void paint(Graphics &g) override {
        if (parent_ && !parent_->isVisible()) return;

        if (isVisible()) {
            for (auto &c : children_) {
//                if (c->isVisible()) c->repaint();

                if (c->isVisible()) {
                    Rectangle bounds = c->getBounds();
                    g.setActiveWindowPosition(bounds.x, bounds.y);
                    g.setActiveWindowSize(bounds.width, bounds.height);
                    c->paint(g);
                }
            }

            Rectangle bounds = getBounds();
            g.setActiveWindowPosition(bounds.x, bounds.y);
            g.setActiveWindowSize(bounds.width, bounds.height);

        }
    }

    void visibilityChanged() override {
        Component::visibilityChanged();

        bool v = isVisible() && (parent_ == nullptr && parent_->isVisible());
        for (auto &c : children_) {
            c->setVisible(v);
        }
    }

    void resized() override {
        Component::resized();
        for (auto &c : children_) {
            c->resized();
        }
    }

    void moved() override {
        Component::moved();
        for (auto &c : children_) {
            c->moved();
        }
    }

    // -------------------
    void add(const std::shared_ptr<OComponent> &c) { children_.push_back(c); }

    void clearBackground(Graphics &g) {
        g.fillRect(0, 0, width, height, bgClr_);
    }

    void drawBorder(Graphics &g) {
        g.drawRect(0, 0, width, height, fgClr_);
    }

    void setFgColour(uint16_t clr) { fgClr_ = clr; }

    void setBgColour(uint16_t clr) { bgClr_ = clr; }

    void onTouchUp(const TouchEvent &touchEvent) override {
        TouchListener::onTouchUp(touchEvent);
        if (parent_) parent_->onTouchUp(touchEvent);
    }

    void onTouchDown(const TouchEvent &touchEvent) override {
        TouchListener::onTouchDown(touchEvent);
        if (parent_) parent_->onTouchDown(touchEvent);
    }

    void onTouchClick(const TouchEvent &touchEvent) override {
        TouchListener::onTouchClick(touchEvent);
        if (onClick) {
            onClick();
        } else {
            if (parent_) parent_->onTouchClick(touchEvent);
        }
    }

    void onTouchDoubleClick(const TouchEvent &touchEvent) override {
        TouchListener::onTouchDoubleClick(touchEvent);
        if (parent_) parent_->onTouchDoubleClick(touchEvent);
    }

    void onTouchMove(const TouchEvent &touchEvent) override {
        TouchListener::onTouchMove(touchEvent);
        if (parent_) parent_->onTouchMove(touchEvent);
    }

    void onTouchLongHold(const TouchEvent &touchEvent) override {
        TouchListener::onTouchLongHold(touchEvent);
        if (parent_) parent_->onTouchLongHold(touchEvent);
    }

protected:
    uint16_t bgClr_ = 0x0000;
    uint16_t fgClr_ = 0xffff;
    struct {
        bool active_: 1;
        bool dimmed_: 1;
    };

    std::shared_ptr<OComponent> parent_;
    std::vector<std::shared_ptr<OComponent>> children_;

};
