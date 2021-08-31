#pragma once

#include "OComponent.h"

class TextComponent : public OComponent {
public:
    explicit TextComponent() {
        *label = '\0';
    }

    ~TextComponent() override = default;

    void setLabel(const char *newLabel) {
        if (strlen(newLabel) == 0) {
            *label = '\0';
        } else {
            copyString(label, newLabel, maxLabelLength);
        }
        repaint();
    }

    void paint(Graphics& g) override {
        OComponent::paint(g);
        g.printText(screenX, screenY, label, TextStyle::smallWhiteOnBlack,
                         width, TextAlign::center);
    }

private:
    static const uint8_t maxLabelLength = 20;
    char label[maxLabelLength + 1];
};
