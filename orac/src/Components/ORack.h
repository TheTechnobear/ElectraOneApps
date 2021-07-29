#pragma once

#include "OModule.h"

#include <KontrolModel.h>

class OracRack : public OComponent {
public:
    OracRack(const Kontrol::Rack &r, OComponent *parent = nullptr) :
        OComponent(parent),
        rackId_(r.id()) {
        model_ = Kontrol::KontrolModel::model();
    }

    ~OracRack() {}

//    void assignToWindow(Window *window) override {
//        // assign sub components
//
//    }

//    void visibilityChanged() override {
//    }

//    void resized() override {
//        // resize sub components
//    }

    void paint(void) {
        clearBackground();
        OComponent::paint();

        drawBorder();
        auto rack = model_->getRack(rackId_);
        if (rack) {
//            screen.printText(screenX , screenY + 2 , rack->displayName().c_str(), TextStyle::smallWhiteOnBlack, width , TextAlign::center);
        }
    }

    void addModule(const Kontrol::Rack &r, const Kontrol::Module &m) {
        if (r.id() != rackId_) { logMessage("ASSERT OracRack::addModule"); }

        auto i = modules_.find(m.id());
        if (i == modules_.end()) {
            auto module = std::make_shared<OracModule>(r, m, this);
            modules_[m.id()] = module;

            //TODO - position is dynamic
            unsigned constexpr NMOD = 3;
            unsigned pos = modules_.size() - 1;
            unsigned posmod = pos % NMOD;
            unsigned h = ((height - 4) / NMOD) - 4;
            unsigned w = width - 4;
            unsigned x = screenX + 2;
            unsigned y = screenY + 2 + (h * posmod);
            module->setBounds(x, y, w, h);
            module->setVisible(isVisible() && pos < NMOD);
            module->setDimmed(pos != 0);
            module->setActive(pos < NMOD); //TODO?

            getParentWindow()->addComponent(module.get());
            add(module);
            module->assignToWindow(getParentWindow());
            if (module->isVisible()) {
                module->repaint();
            }

        } else {
            //already exists
        }
    }

    std::shared_ptr<OracModule> getModule(const Kontrol::EntityId &id) {
        auto i = modules_.find(id);
        if (i != modules_.end()) {
            return i->second;
        }
        return nullptr;
    }

private:
    Kontrol::EntityId rackId_;
    std::shared_ptr<Kontrol::KontrolModel> model_;
    std::map<Kontrol::EntityId, std::shared_ptr<OracModule>> modules_;
};