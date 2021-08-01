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

    void click(unsigned i) { logMessage("clik param %d", i); }

    void initParams() {
        auto rack = model_->getRack(rackId_);
        auto module = model_->getModule(rack, moduleId_);
        auto page = model_->getPage(module, pageId_);

        auto params = module->getParams(page);

        int pos = 0;
        for (auto param : params) {
            auto ctrl = std::make_shared<OracParam>(*rack, *module, *param, this);

            params_.push_back(ctrl);

            unsigned pageN = getId();
            unsigned enc = ((pos / 2) * 6) + (pos % 2) + (pageN * 2);
            ctrl->setId(enc);
            ctrl->onClick = [this, pos](void) {
                click(pos);
                return true;
            };


            unsigned h = (height - 9) / 2;
            unsigned w = (width - 9) / 2;
            unsigned x = screenX + 3 + ((pos % 2) * (w + 3));
            unsigned y = screenY + 3 + ((pos / 2) * (h + 3));

            ctrl->setBounds(x, y, w, h);
            ctrl->setVisible(parent_->isVisible());
//            ctrl->setDimmed(pos != 0);
//            ctrl->setActive(parent_->isActive());

            getParentWindow()->addComponent(ctrl.get());
            add(ctrl);
            ctrl->assignToWindow(getParentWindow());
            if (enc < 12) getParentWindow()->assignPot(enc, 0, ctrl.get());

            if (ctrl->isVisible()) {
                ctrl->repaint();
            }

            pos++;
        }
    }

    void paint(void) {
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

private:
    Kontrol::EntityId rackId_;
    Kontrol::EntityId moduleId_;
    Kontrol::EntityId pageId_;

    std::shared_ptr<Kontrol::KontrolModel> model_;

//    static constexpr unsigned MAX_PARAMS = 4;
    std::vector<std::shared_ptr<OracParam>> params_;

//    unsigned pageN_ = 0;
};
