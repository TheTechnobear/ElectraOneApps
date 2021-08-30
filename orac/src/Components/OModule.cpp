#include "OModule.h"

#include "../Debug.h"

void OracModule::paint() {
    clearBackground();
    OComponent::paint();

    if (isActive()) drawBorder();

    auto rack = model_->getRack(rackId_);
    auto module = model_->getModule(rack, moduleId_);
    if (rack && module) {
        screen.printText(10, 10, module->id().c_str(), TextStyle::smallWhiteOnBlack, width, TextAlign::left);

        if (module->displayName().length() < 6) {
            screen.printText(10, 50, module->displayName().c_str(), TextStyle::smallWhiteOnBlack, width, TextAlign::left);
        } else {
            std::string s1 = module->displayName().substr(0, 5);
            std::string s2 = module->displayName().substr(5);
            screen.printText(10, 50, s1.c_str(), TextStyle::smallWhiteOnBlack, width, TextAlign::left);
            screen.printText(10, 70, s2.c_str(), TextStyle::smallWhiteOnBlack, width, TextAlign::left);
        }
    }
}

void OracModule::onPotChange(int16_t relativeChange, handle_t handle) {
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

void OracModule::addPage(const Kontrol::Rack &r, const Kontrol::Module &m, const Kontrol::Page &p) {
    if (r.id() != rackId_) { dbgMessage("ASSERT OracModule::addPage - invalid rack"); }
    if (m.id() != moduleId_) { dbgMessage("ASSERT OracModule::addPage - invalid module"); }

    static uint16_t clrs_[MAX_DISPLAY] = {ElectraColours::getNumericRgb565(ElectraColours::red),
                                          ElectraColours::getNumericRgb565(ElectraColours::orange),
                                          ElectraColours::getNumericRgb565(ElectraColours::blue)};
    // uint16_t clrs_[3] = {rgbToRgb565(0xF, 0x0, 0x0), gbToRgb565(0x0, 0xF, 0x0),
    //                      rgbToRgb565(0x0, 0x0, 0xF)};

    if (pages_.count(p.id()) == 0) {
//            dbgMessage("addPage %s %s", moduleId_.c_str(), p.id().c_str());
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
        // TODO : use moved/visibility changed?
        scrollView();
    } else {
//            dbgMessage("existing Page %s %s", moduleId_.c_str(), p.id().c_str());
    }
}

void OracModule::scrollView() {
    // TODO : use moved/visibility changed?
    unsigned idx = 0;
//        unsigned h = height - 20;
    unsigned w = (width - 60) / MAX_DISPLAY;
    for (const auto &id: displayOrder_) {
        bool vis = idx >= displayOffset_ && (idx < displayOffset_ + MAX_DISPLAY);
        bool act = vis && (idx == displayIdx_);
        if (pages_.count(id) > 0) {
            auto page = pages_[id];
            page->setVisible(isVisible() && vis);
            page->setDimmed(!act);
            page->setActive(act);
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
