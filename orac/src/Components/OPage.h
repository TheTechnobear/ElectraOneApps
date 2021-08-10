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


    ~OracPage() override = default;


    void initParams();

    void paint() override;

    void onPotChange(int16_t relativeChange, handle_t enc) override {
        if (enc < params_.size()) {
            auto &param = params_[enc];
            param->onPotChange(relativeChange, enc);
        }
    }

    void moved() override;

    void resized() override {
        OComponent::resized();
    }

    void visibilityChanged() override {
//        OComponent::visibilityChanged();
        bool vis = isVisible();
        bool act = vis && isActive();

        int pos = 0;
        for (auto ctrl : params_) {
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
