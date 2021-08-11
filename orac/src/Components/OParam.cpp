#include "OParam.h"

void OracParam::paint() {
//        dbgMessage("paint param %s", (moduleId_+":"+paramId_).c_str());
    clearBackground();
    OComponent::paint();

//        drawBorder();

    auto rack = model_->getRack(rackId_);
    auto module = model_->getModule(rack, moduleId_);
    auto param = model_->getParam(module, paramId_);


    if (param) {
        unsigned bw = width - 15;
        unsigned bh = 24;
        screen.fillRoundRect(screenX + 7, screenY + 5, bw, bh, 3, ElectraColours::getNumericRgb565Darker(ElectraColours::white));

        float value = param->asFloat(param->current());
        auto vw = static_cast<unsigned>(float(bw) * value);
        static constexpr unsigned minW = 6;
        if (vw <= minW) vw = minW;

        screen.fillRoundRect(screenX + 7, screenY + 5, vw, bh, 3, fgClr_);

        auto vstr = param->displayValue();
        screen.printText(screenX + 17, screenY + 10, vstr.c_str(), TextStyle::mediumTransparent, bw - 20, TextAlign::left);

        screen.printText(screenX, screenY + height - 20,
                         param->displayName().c_str(),
                         TextStyle::smallWhiteOnBlack, width, TextAlign::center);
    }
}

void OracParam::onPotChange(int16_t relativeChange, handle_t handle) {
    OComponent::onPotChange(relativeChange, handle);

    auto rack = model_->getRack(rackId_);
    auto module = model_->getModule(rack, moduleId_);
    auto param = model_->getParam(module, paramId_);

    if (rack && module && param) {
        auto chg = float(relativeChange) / 256.0f;
        Kontrol::ParamValue calc = param->calcRelative(chg);
        model_->changeParam(Kontrol::CS_LOCAL, rackId_, moduleId_, paramId_, calc);
        repaint();
    }
}
