#pragma once

#include "OComponent.h"

class TextComponent : public OComponent {
public:
  TextComponent() { *label = '\0'; }

  ~TextComponent() {}

  void setLabel(const char *newLabel) {
    if (strlen(newLabel) == 0) {
      *label = '\0';
    } else {
      copyString(label, newLabel, maxLabelLength);
    }
    repaint();
  }

  void paint(void) {
    OComponent::paint();
    // uint16_t w =    screen.getTextWidth(label, TextStyle::smallWhiteOnBlack);

    screen.printText(screenX, screenY, label, TextStyle::smallWhiteOnBlack,
                     width, TextAlign::center);

    // if (active_) {
    //   screen.drawLine(screenX + (width / 2) - (w / 2) - 1,
    //                   screenY + 15, w, 0, 0xFFFF);
    // }
  }

private:
  static const uint8_t maxLabelLength = 20;
  char label[maxLabelLength + 1];
};
