#pragma once

#include "OPage.h"

#include <KontrolModel.h>

class OracModule : public OComponent {
public:
    OracModule(const Kontrol::Rack &r,
               const Kontrol::Module &m,
               OComponent *parent) :
        OComponent(parent),
        rackId_(r.id()),
        moduleId_(m.id()) {
        model_ = Kontrol::KontrolModel::model();
    }

    ~OracModule() override = default;

    Kontrol::EntityId rackId() { return rackId_; }

    Kontrol::EntityId moduleId() { return moduleId_; }

    void paint() override;

    void onPotChange(int16_t relativeChange, handle_t handle) override;

    void addPage(const Kontrol::Rack &r, const Kontrol::Module &m, const Kontrol::Page &p);

    void reset() {
//        dbgMessage("reset %s", moduleId_.c_str());
        pages_.clear();
        displayOrder_.clear();
        displayIdx_ = 0;
        displayOffset_ = 0;
    }


    std::shared_ptr<OracPage> getActivePage() {
        if (displayIdx_ < displayOrder_.size()) {
            auto id = displayOrder_[displayIdx_];
            if (pages_.count(id) > 0) {
                return pages_[id];
            }
        }
        return nullptr;
    }

    void nextDisplay() {
        if ((displayIdx_ + 1) < displayOrder_.size()) {
            getActivePage()->setActive(false);
            displayIdx_++;
            if ((displayIdx_ - displayOffset_) >= MAX_DISPLAY) {
                displayOffset_ = displayIdx_ - (MAX_DISPLAY - 1);
                scrollView();
            } else {
                getActivePage()->setActive(true);
                repaint();
            }
        }
    }

    void prevDisplay() {
        if (displayIdx_ > 0) {
            getActivePage()->setActive(false);
            displayIdx_--;
            if ((displayIdx_ - displayOffset_) >= MAX_DISPLAY) {
                displayOffset_ = displayIdx_;
                scrollView();
            } else {
                getActivePage()->setActive(true);
                repaint();
            }
        }
    }


    void scrollView();

    void moved() override {
//        OComponent::moved();
        unsigned idx = 0;
//        unsigned h = height - 20;
        unsigned w = (width - 60) / MAX_DISPLAY;
        for (auto id: displayOrder_) {
            bool vis = idx >= displayOffset_ && (idx < displayOffset_ + MAX_DISPLAY);
            if (pages_.count(id) > 0) {
                auto page = pages_[id];
                if (vis) {
                    unsigned x = screenX + 50 + ((idx - displayOffset_) * w);
                    unsigned y = screenY + 10;
                    page->setPosition(x, y);
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
//        unsigned h = height - 20;
//        unsigned w = (width - 60) / MAX_DISPLAY;
        for (auto id: displayOrder_) {
            bool vis = idx >= displayOffset_ && (idx < displayOffset_ + MAX_DISPLAY);
            if (pages_.count(id) > 0) {
                auto page = pages_[id];
                page->setVisible(isVisible() && vis);
            }
            idx++;
        }
        repaint();
    }

private:

    Kontrol::EntityId rackId_{};
    Kontrol::EntityId moduleId_{};

    static constexpr unsigned MAX_DISPLAY = 3;
    unsigned displayOffset_ = 0, displayIdx_ = 0;
    std::vector<Kontrol::EntityId> displayOrder_{};

    std::shared_ptr<Kontrol::KontrolModel> model_{};
    std::map<Kontrol::EntityId, std::shared_ptr<OracPage>> pages_;
};