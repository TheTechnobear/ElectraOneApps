#pragma once

#include "Component.h"
#include <Colours.h>

// text height s=12, m = 16, L =18
class OComponent : public Component {
public:
  OComponent()
      : disable_(false), active_(false), dimmed_(false), bgClr_(0x0000),
        fgClr_(0xffff) {}

  ~OComponent() {}

  std::function<bool(void)> onClick;
  void setfgColour(uint16_t clr) { fgClr_ = clr; }
  void setBgColour(uint16_t clr) { bgClr_ = clr; }
  void setDisabled(bool d) { disable_ = d; }

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

  // Graphics
  void paintUpdate(void) override { paint(); }
  void paint(void) override {
    screen.fillRect(screenX, screenY, width - 1, height - 1, bgClr_);
  }

protected:
  uint16_t bgClr_;
  uint16_t fgClr_;
  struct {
    bool disable_ : 1;
    bool active_ : 1;
    bool dimmed_ : 1;
  };
};
