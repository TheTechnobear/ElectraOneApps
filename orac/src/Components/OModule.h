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

    ~OracModule() {}

    void paint(void) {
        clearBackground();
        OComponent::paint();

        if (isActive()) drawBorder();

        auto rack = model_->getRack(rackId_);
        auto module = model_->getModule(rack, moduleId_);
        if (rack && module) {
            screen.printText(screenX + 10, screenY + 10, module->id().c_str(), TextStyle::smallWhiteOnBlack, width, TextAlign::left);

            if(module->displayName().length()<6) {
                screen.printText(screenX + 10, screenY + 50, module->displayName().c_str(), TextStyle::smallWhiteOnBlack, width, TextAlign::left);
            } else {
                std::string s1 = module->displayName().substr(0,5);
                std::string s2 = module->displayName().substr(5);
                screen.printText(screenX + 10, screenY + 50, s1.c_str(), TextStyle::smallWhiteOnBlack, width, TextAlign::left);
                screen.printText(screenX + 10, screenY + 70, s2.c_str(), TextStyle::smallWhiteOnBlack, width, TextAlign::left);

            }
        }
    }

    void onPotChange(int16_t relativeChange, handle_t handle = 0) override {
        if (isVisible() && isActive()) {
            unsigned pageN = ((handle % 6) / 2) + displayOffset_;
            if (pageN < displayOrder_.size()) {
                auto id = displayOrder_[pageN];
                if (pages_.count(id) > 0) {
                    auto page = pages_[id];
                    unsigned c = (handle % 2);
                    unsigned r = (handle / 6);
                    unsigned enc = (r * 2) + c;
                    page->onPotChange(relativeChange, enc);
                }
            }
        }
    }

    void visibilityChanged() override {}

    void addPage(const Kontrol::Rack &r, const Kontrol::Module &m, const Kontrol::Page &p) {

        if (r.id() != rackId_) { dbgMessage("ASSERT OracModule::addPage - invalid rack"); }
        if (m.id() != moduleId_) { dbgMessage("ASSERT OracModule::addPage - invalid module"); }

        static uint16_t clrs_[MAX_DISPLAY] = {ElectraColours::getNumericRgb565(ElectraColours::red),
                                              ElectraColours::getNumericRgb565(ElectraColours::orange),
                                              ElectraColours::getNumericRgb565(ElectraColours::blue)};
        // uint16_t clrs_[3] = {rgbToRgb565(0xF, 0x0, 0x0), gbToRgb565(0x0, 0xF, 0x0),
        //                      rgbToRgb565(0x0, 0x0, 0xF)};

        if (pages_.count(p.id()) == 0) {
            auto page = std::make_shared<OracPage>(r, m, p, this);
            pages_[p.id()] = page;

            // TODO use page order?
            displayOrder_.push_back(p.id());

            unsigned pidx = displayOrder_.size() - 1;
            page->setFgColour(clrs_[pidx % MAX_DISPLAY]);
            page->setId(pidx);
            page->setVisible(false);
            page->setDimmed(false);
            page->setActive(false);

            unsigned h = height - 20;
            unsigned w = (width - 60) / MAX_DISPLAY;
            page->setBounds(screenX, screenY, w, h);

            getParentWindow()->addComponent(page.get());
            add(page);

            page->assignToWindow(getParentWindow());
            page->initParams();
            reposition();
        } else {
            //already exists
//            auto page = pages_[p.id()];
//            if (page->isVisible()) {
//                page->repaint();
//            }
//            repaint();
        }
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
                reposition();
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
                reposition();
            } else {
                getActivePage()->setActive(true);
                repaint();
            }
        }
    }


    void reposition() {
        unsigned idx = 0;
//        unsigned h = height - 20;
        unsigned w = (width - 60) / MAX_DISPLAY;
        for (auto id: displayOrder_) {
            bool vis = isVisible()
                       && (idx >= displayOffset_ && (idx < displayOffset_ + MAX_DISPLAY));
            bool act = vis && (idx == displayIdx_);
            if (pages_.count(id) > 0) {
                auto page = pages_[id];
                if (vis) {
                    unsigned x = screenX + 50 + ((idx - displayOffset_) * w);
                    unsigned y = screenY + 10;
                    page->setPosition(x, y);
                }
                page->setVisible(vis);
                page->setDimmed(!act);
                page->setActive(act);

                page->reposition();// TODO: resized?
            }
            idx++;
        }
        repaint();
    }


private:

    Kontrol::EntityId rackId_;
    Kontrol::EntityId moduleId_;

    static constexpr unsigned MAX_DISPLAY = 3;
    unsigned displayOffset_ = 0, displayIdx_ = 0;
    std::vector<Kontrol::EntityId> displayOrder_;

    std::shared_ptr<Kontrol::KontrolModel> model_;
    std::map<Kontrol::EntityId, std::shared_ptr<OracPage>> pages_;
};