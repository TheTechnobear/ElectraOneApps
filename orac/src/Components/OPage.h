#pragma once

#include "OParam.h"

#include <KontrolModel.h>

class OracPage : public OComponent {
public:
    OracPage(const Kontrol::Rack &r,
             const Kontrol::Module &m,
             const Kontrol::Page &p,
             OComponent *parent) :
        OComponent(parent),
        rackId_(r.id()),
        moduleId_(m.id()),
        pageId_(p.id()) {
        model_ = Kontrol::KontrolModel::model();
    }


    ~OracPage() {}


    void initParams() {
        auto rack = model_->getRack(rackId_);
        auto module = model_->getModule(rack, moduleId_);
        auto page = model_->getPage(module, pageId_);

        auto params = module->getParams(page);

        int pos = 0;
        for (auto param : params) {
            auto ctrl = std::make_shared<OracParam>(*rack, *module, *param, this);
            params_.push_back(ctrl);
            unsigned h = (height - 9) / 2;
            unsigned w = (width - 9) / 2;
//            unsigned x = screenX + 3 + ((pos % 2) * (w + 3));
//            unsigned y = screenY + 3 + ((pos / 2) * (h + 3));

            ctrl->setBounds(0, 0, w, h);
            ctrl->setVisible(false);
            ctrl->setDimmed(false);
            ctrl->setActive(false);

            getParentWindow()->addComponent(ctrl.get());
            add(ctrl);
            ctrl->assignToWindow(getParentWindow());
            pos++;
        }

        reposition();
    }

    void paint(void) {
//        dbgMessage("paint page %s", (moduleId_+":"+pageId_).c_str());
        clearBackground();

        OComponent::paint();

        auto rack = model_->getRack(rackId_);
        auto module = model_->getModule(rack, moduleId_);
        auto page = model_->getPage(module, pageId_);

        drawBorder();

        if (page) {
            auto &name = page->displayName();

            screen.printText(screenX + 1, screenY + 1, name.c_str(),
                             TextStyle::smallWhiteOnBlack, width - 2,
                             TextAlign::center);
        }

        if (active_) {
            // screen.drawLine(screenX - 1, screenY - 1, width, 1, 0xFFFF);
        }
    }

    void onPotChange(int16_t relativeChange, handle_t handle = 0) override {
        unsigned enc = handle;
        if (enc < params_.size()) {
            auto param = params_[enc];
            param->onPotChange(relativeChange, enc);
        }
    }


    void reposition() {
        // reposition ctrls, due to this page being moved
        bool vis = isVisible();
        bool act = vis;

        unsigned h = (height - 9) / 2;
        unsigned w = (width - 9) / 2;

        int pos = 0;
        for (auto ctrl : params_) {
            if (vis) {
                unsigned x = screenX + 3 + ((pos % 2) * (w + 3));
                unsigned y = screenY + 3 + ((pos / 2) * (h + 3));
                ctrl->setPosition(x, y);
            }
            ctrl->setVisible(vis);
            ctrl->setDimmed(!act);
            ctrl->setActive(act);
            pos++;
        }
    }

private:
    Kontrol::EntityId rackId_;
    Kontrol::EntityId moduleId_;
    Kontrol::EntityId pageId_;

    std::shared_ptr<Kontrol::KontrolModel> model_;
    std::vector<std::shared_ptr<OracParam>> params_;
};
