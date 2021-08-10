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

    void paint() override;

    void onPotChange(int16_t relativeChange, handle_t handle) override;


    void addModule(const Kontrol::Rack &r, const Kontrol::Module &m);

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

    void scrollView();

    // rack doesnt get moved/resized
    void moved() override;

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