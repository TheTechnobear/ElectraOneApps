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

//        drawBorder();

        auto rack = model_->getRack(rackId_);
        auto module = model_->getModule(rack, moduleId_);
        auto param = model_->getParam(module, paramId_);


        if (param) {
            unsigned bw = width - 15;
            screen.fillRoundRect(screenX + 7, screenY + 5, bw, 24, 3, COLOR_WHITE);

            float value = param->asFloat(param->current());
            unsigned vw = float(bw) * value;
            static constexpr unsigned minW = 6;
            if (vw <= minW) vw = minW;

            screen.fillRoundRect(screenX + 7, screenY + 5, vw, 24, 3, fgClr_);

            screen.printText(screenX, screenY + height - 20,
                             param->displayName().c_str(),
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
            Kontrol::ParamValue calc = param->calcRelative(chg);
            model_->changeParam(Kontrol::CS_LOCAL, rackId_, moduleId_, paramId_, calc);
            repaint();
        }
    }

private:
    std::shared_ptr<Kontrol::KontrolModel> model_;
    Kontrol::EntityId rackId_;
    Kontrol::EntityId moduleId_;
    Kontrol::EntityId paramId_;
};
