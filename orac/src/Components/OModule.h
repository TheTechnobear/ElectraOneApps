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
        auto module = model_->getModule(rack, moduleId_);
        if (rack && module) {
            screen.printText(screenX, screenY + 2, module->id().c_str(), TextStyle::smallWhiteOnBlack, width, TextAlign::left);
            screen.printText(screenX, screenY + 50, module->displayName().c_str(), TextStyle::smallWhiteOnBlack, width, TextAlign::left);
        }
    }

    void addPage(const Kontrol::Rack &r, const Kontrol::Module &m, const Kontrol::Page &p) {
        if (r.id() != rackId_) { logMessage("ASSERT OracModule::addPage - invalid rack"); }
        if (m.id() != moduleId_) { logMessage("ASSERT OracModule::addPage - invalid module"); }

        static uint16_t clrs_[3] = {ElectraColours::getNumericRgb565(ElectraColours::red),
                                    ElectraColours::getNumericRgb565(ElectraColours::orange),
                                    ElectraColours::getNumericRgb565(ElectraColours::blue)};
        // uint16_t clrs_[3] = {rgbToRgb565(0xF, 0x0, 0x0), gbToRgb565(0x0, 0xF, 0x0),
        //                      rgbToRgb565(0x0, 0x0, 0xF)};



        auto i = pages_.find(p.id());
        if (i == pages_.end()) {

            auto page = std::make_shared<OracPage>(r, m, p, this);
            pages_[p.id()] = page;

            //TODO - position is dynamic
            unsigned constexpr NPAGE = 3;
            unsigned pos = pages_.size() - 1;
            unsigned posmod = pos % NPAGE;
            unsigned h = height - 4;
            unsigned w = (width - 60) / NPAGE;
            unsigned x = screenX + 50 + (w * posmod);
            unsigned y = screenY + 2;
            page->setFgColour(clrs_[posmod]);
            page->setId(pos);
            page->setBounds(x, y, w, h);
            page->setVisible(isVisible() && pos < NPAGE);
            page->setDimmed(pos != 0);
            page->setActive(pos < NPAGE); //TODO?

            getParentWindow()->addComponent(page.get());
            add(page);
            page->assignToWindow(getParentWindow());

            page->initParams();
            if (page->isVisible()) {
                page->repaint();
            }

        } else {
            //already exists
        }
    }

private:
    Kontrol::EntityId rackId_;
    Kontrol::EntityId moduleId_;

    std::shared_ptr<Kontrol::KontrolModel> model_;
    std::map<Kontrol::EntityId, std::shared_ptr<OracPage>> pages_;
};