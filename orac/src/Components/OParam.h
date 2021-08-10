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

    ~OracParam() override = default;

    void paint() override;

    void onPotChange(int16_t relativeChange, handle_t handle) override;

private:
    std::shared_ptr<Kontrol::KontrolModel> model_;
    Kontrol::EntityId rackId_;
    Kontrol::EntityId moduleId_;
    Kontrol::EntityId paramId_;
};
