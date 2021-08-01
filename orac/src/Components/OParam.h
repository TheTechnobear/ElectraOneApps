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
        clearBackground();
        OComponent::paint();

        auto rack = model_->getRack(rackId_);
        auto module = model_->getModule(rack, moduleId_);
        auto param = model_->getParam(module, paramId_);

        drawBorder();

        if (param) {
            screen.fillRoundRect(screenX + 25, screenY + 10, width - 50, 16, 5, COLOR_WHITE);

            float value = param->asFloat(param->current());
            if (value > 1.0f || value < 0.0f) value = 0.5f;

            screen.fillRoundRect(screenX + 25, screenY + 10, float(width - 50) * value, 16, 5, ElectraColours::getNumericRgb565(ElectraColours::red));

            screen.printText(screenX, screenY + 50, param->displayName().c_str(),
                             TextStyle::smallWhiteOnBlack, width,
                             TextAlign::center);
        }


//        uint16_t nameH = 12;
//        int r = (height - 1 - nameH) / 2;
//        // screen.drawRect(screenX, screenY, width - 1, height - 1, COLOR_WHITE);
//
//        int xc = screenX + 1 + r + (nameH / 2);
//        int yc = screenY + 1 + r;
//
//        screen.drawCircle(xc, yc, r, COLOR_WHITE);
//
//        float value = 0.0f;
//
//        if (param) {
//            auto &name = param->displayName();
//            int y = screenY + height - 2 - nameH;
//            screen.printText(screenX + 1, y, name.c_str(),
//                             TextStyle::smallWhiteOnBlack, width - 2,
//                             TextAlign::center);
//            value = param->asFloat(param->current());
//        }
//
////        float ph = value + 0.25f;
//        float ph = (value * 0.9f) + 0.05f + 0.25f;
//        int xe = (cosf(ph * 2 * PI) * float(r));
//        int ye = (sinf(ph * 2 * PI) * float(r));
//        screen.drawLine(xc, yc, xe, ye, COLOR_WHITE);
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
