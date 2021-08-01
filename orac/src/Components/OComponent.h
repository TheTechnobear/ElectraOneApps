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
        dimmed_(false) {
    }

    ~OComponent() {
    }

    std::function<bool(void)> onClick;

    // Component
    void setDimmed(bool d) override { dimmed_ = d; }

    bool isDimmed(void) const override { return dimmed_; }

    void setActive(bool a) override { active_ = a; }

    bool isActive(void) const override { return (active_); }

    void onPotTouchUp(handle_t handle) override {}

    void onPotTouchDown(handle_t handle) override {
        // if (onClick) {
        //   onClick();
        // }
    }

    void onTouchUp(TouchEvent &touchEvent) override {
        if (onClick) {
            onClick();
        }
    }


    void assignToWindow(Window *window) {
        Component::assignToWindow(window);
        for (auto &c : children_) {
            c->assignToWindow(window);
        }
    }


    // Graphics
    void paintUpdate(void) override { paint(); }

    void paint(void) override {
        if(isVisible()) {
            for (auto &c : children_) {
//                if(c->isVisible()) c->paint();
                c->paint();
            }
        }
    }

    void visibilityChanged() override {
        bool v = isVisible();
        for (auto &c : children_) {
            c->setVisible(v);
        }
    }

    void resized() override {
        for (auto &c : children_) {
            c->resized();
        }
    }

    // -------------------
    void add(std::shared_ptr<OComponent> c) { children_.push_back(c); }

    void clearBackground() {
        screen.fillRect(screenX, screenY, width - 1, height - 1, bgClr_);
    }

    void drawBorder() {
        screen.drawRect(screenX, screenY, width - 1, height - 1, fgClr_);
    }

    void setFgColour(uint16_t clr) { fgClr_ = clr; }

    void setBgColour(uint16_t clr) { bgClr_ = clr; }

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
