#pragma once

#include <KontrolModel.h>

#include "OComponent.h"

class OracParam : public OComponent {
public:
    OracParam(
        const Kontrol::Rack &r,
        const Kontrol::Module &m,
        const Kontrol::Parameter &p,
        OComponent *parent) :
        OComponent(parent),
        rackId_(r.id()),
        moduleId_(m.id()),
        paramId_(p.id()) {
        model_ = Kontrol::KontrolModel::model();
    }

    ~OracParam() {}

    void paint(void) {
//        dbgMessage("paint param %s", (moduleId_+":"+paramId_).c_str());
        clearBackground();
        OComponent::paint();

        auto rack = model_->getRack(rackId_);
        auto module = model_->getModule(rack, moduleId_);
        auto param = model_->getParam(module, paramId_);

        drawBorder();

        if (param) {
            screen.fillRoundRect(screenX + 25, screenY + 10, width - 50, 16, 3, COLOR_WHITE);

            float value = param->asFloat(param->current());
            unsigned vw = float(width - 50) * value;
            static constexpr unsigned minW = 6;
            if (vw <= minW) vw = minW;

            screen.fillRoundRect(screenX + 25, screenY + 10, vw, 16, 3, ElectraColours::getNumericRgb565(ElectraColours::red));

            screen.printText(screenX, screenY + 50, param->displayName().c_str(),
                             TextStyle::smallWhiteOnBlack, width,
                             TextAlign::center);
        }
    }

    void onPotChange(int16_t relativeChange, handle_t handle = 0) override {
        OComponent::onPotChange(relativeChange, handle);

        auto rack = model_->getRack(rackId_);
        auto module = model_->getModule(rack, moduleId_);
        auto param = model_->getParam(module, paramId_);

        if (rack && module && param) {
            float chg = float(relativeChange / 256.0f);
            logMessage("change %d %f", relativeChange, chg);
            Kontrol::ParamValue calc = param->calcRelative(chg);
            model_->changeParam(Kontrol::CS_LOCAL, rackId_, moduleId_, paramId_,
                                calc);
            repaint();
        }
    }

private:
    std::shared_ptr<Kontrol::KontrolModel> model_;
    Kontrol::EntityId rackId_;
    Kontrol::EntityId moduleId_;
    Kontrol::EntityId paramId_;
};
