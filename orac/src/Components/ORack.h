#pragma once

#include "OModule.h"

#include <KontrolModel.h>

class OracRack : public OComponent {
public:
    explicit OracRack(const Kontrol::Rack &r, OComponent *parent = nullptr) :
        OComponent(parent),
        rackId_(r.id()) {
        model_ = Kontrol::KontrolModel::model();
    }

    ~OracRack() override = default;


    Kontrol::EntityId rackId() { return rackId_; }

    void paint() override {
        clearBackground();
        OComponent::paint();

//        drawBorder();
        auto rack = model_->getRack(rackId_);
        if (rack) {
//            screen.printText(screenX , screenY + 2 , rack->displayName().c_str(), TextStyle::smallWhiteOnBlack, width , TextAlign::center);
        }
    }

    void onPotChange(int16_t relativeChange, handle_t handle) override {
        dbgMessage("ASSERT OracRack::onPotChange %d, %d", handle, relativeChange);
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
//            dbgMessage("add module %s %s", rackId_.c_str(), m.id().c_str());
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

            scrollView();
        } else {
            //already exists
//            dbgMessage("existing module %s %s", rackId_.c_str(), m.id().c_str());
            auto module = modules_[m.id()];
            // module needs to be reset, so parameters and pages change
            module->reset();
        }
    }

    std::shared_ptr<OracModule> getModule(const Kontrol::EntityId &id) {
        auto i = modules_.find(id);
        if (i != modules_.end()) {
            return i->second;
        }
        return nullptr;
    }


    std::shared_ptr<OracModule> getActiveModule() {
        if (displayIdx_ < displayOrder_.size()) {
            auto id = displayOrder_[displayIdx_];
            if (modules_.count(id) > 0) {
                return modules_[id];
            }
        }
        return nullptr;
    }


    void nextDisplay() {
        if ((displayIdx_ + 1) < displayOrder_.size()) {
            getActiveModule()->setActive(false);
            displayIdx_++;
            if ((displayIdx_ - displayOffset_) >= MAX_DISPLAY) {
                displayOffset_ = displayIdx_ - (MAX_DISPLAY - 1);
                scrollView();
            } else {
                getActiveModule()->setActive(true);
                repaint();
            }
        }
    }

    void prevDisplay() {
        if (displayIdx_ > 0) {
            getActiveModule()->setActive(false);
            displayIdx_--;
            if ((displayIdx_ - displayOffset_) >= MAX_DISPLAY) {
                displayOffset_ = displayIdx_;
                scrollView();
            } else {
                getActiveModule()->setActive(true);
                repaint();
            }
        }
    }

    void scrollView() {
        // TODO : use moved/visibility changed?
        unsigned idx = 0;
        unsigned h = ((height - 4) / MAX_DISPLAY) - 4;
//        unsigned w = width - 4;
        for (auto id: displayOrder_) {
            bool vis = idx >= displayOffset_ && (idx < displayOffset_ + MAX_DISPLAY);
            bool act = vis && (idx == displayIdx_);
            if (modules_.count(id) > 0) {
                auto module = modules_[id];
                module->setVisible(isVisible() && vis);
                module->setDimmed(!act);
                module->setActive(act);
                if (vis) {
                    unsigned x = screenX + 5;
                    unsigned y = screenY + 5 + ((idx - displayOffset_) * h);
                    module->setPosition(x, y);
                }
            }
            idx++;
        }
        repaint();
    }

    // rack doesnt get moved/resized
    void moved() override {
//        OComponent::moved();
        unsigned idx = 0;
        unsigned h = ((height - 4) / MAX_DISPLAY) - 4;
//        unsigned w = width - 4;
        for (auto id: displayOrder_) {
            bool vis = idx >= displayOffset_ && (idx < displayOffset_ + MAX_DISPLAY);
            if (modules_.count(id) > 0) {
                auto module = modules_[id];
                if (vis) {
                    unsigned x = screenX + 5;
                    unsigned y = screenY + 5 + ((idx - displayOffset_) * h);
                    module->setPosition(x, y);
                }
            }
            idx++;
        }
        repaint();
    }

    void resized() override {
        OComponent::resized();
    }

    void visibilityChanged() override {
//        OComponent::visibilityChanged();
        unsigned idx = 0;
        for (auto id: displayOrder_) {
            bool vis = idx >= displayOffset_ && (idx < displayOffset_ + MAX_DISPLAY);
            if (modules_.count(id) > 0) {
                auto module = modules_[id];
                module->setVisible(isVisible() && vis);
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