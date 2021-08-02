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

    void paint(void) {
        clearBackground();
        OComponent::paint();

        drawBorder();
        auto rack = model_->getRack(rackId_);
        if (rack) {
//            screen.printText(screenX , screenY + 2 , rack->displayName().c_str(), TextStyle::smallWhiteOnBlack, width , TextAlign::center);
        }
    }

    void onPotChange(int16_t relativeChange, handle_t handle = 0) override {
        if (displayIdx_ < displayOrder_.size()) {
            auto id = displayOrder_[displayIdx_];
            if (modules_.count(id) > 0) {
                auto module = modules_[id];
                module->onPotChange(relativeChange, handle);
            }
        }
    }

    void addModule(const Kontrol::Rack &r, const Kontrol::Module &m) {
        if (r.id() != rackId_) { dbgMessage("ASSERT OracRack::addModule"); }

        if (modules_.count(m.id()) == 0) {
            auto module = std::make_shared<OracModule>(r, m, this);
            modules_[m.id()] = module;
            displayOrder_.push_back(m.id());

            module->setVisible(false);
            module->setDimmed(false);
            module->setActive(false);

            unsigned h = ((height - 4) / MAX_DISPLAY) - 4;
            unsigned w = width - 4;
            module->setBounds(screenX, screenY, w, h);

            getParentWindow()->addComponent(module.get());
            add(module);
            module->assignToWindow(getParentWindow());

            reposition();
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

    void nextDisplay() {
        if ((displayIdx_ + 1) < displayOrder_.size()) {
            displayIdx_++;
            if ((displayIdx_ - displayOffset_) > MAX_DISPLAY) {
                displayOffset_ = displayIdx_ - MAX_DISPLAY;
                reposition();
            }
        }
    }

    void prevDisplay() {
        if (displayIdx_ > 0) {
            displayIdx_--;
            if ((displayOffset_ + displayIdx_) > MAX_DISPLAY) {
                displayOffset_ = displayIdx_;
                reposition();
            }
        }
    }

    void reposition() {
        unsigned idx = 0;
        unsigned h = ((height - 4) / MAX_DISPLAY) - 4;
//        unsigned w = width - 4;
        for (auto id: displayOrder_) {
            bool vis = isVisible()
                       && (idx >= displayOffset_ && (idx < displayOffset_ + MAX_DISPLAY));
            bool act = vis && (idx == displayIdx_);
            if (modules_.count(id) > 0) {
                auto module = modules_[id];
                if (vis) {
                    unsigned x = screenX + 2;
                    unsigned y = screenY + 2 + ((idx - displayOffset_) * h);
                    module->setPosition(x, y);
                }
                module->setVisible(vis);
                module->setDimmed(!act);
                module->setActive(act);
            }
            idx++;
        }
        repaint();
    }

private:
    Kontrol::EntityId rackId_;

    static constexpr unsigned MAX_DISPLAY = 3;
    unsigned displayOffset_ = 0, displayIdx_ = 0;
    std::vector<Kontrol::EntityId> displayOrder_;

    std::shared_ptr<Kontrol::KontrolModel> model_;
    std::map<Kontrol::EntityId, std::shared_ptr<OracModule>> modules_;
};