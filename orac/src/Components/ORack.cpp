#include "ORack.h"

#include "../Debug.h"

void OracRack::addModule(const Kontrol::Rack &r, const Kontrol::Module &m) {
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

void OracRack::paint() {
    clearBackground();
    OComponent::paint();

//        drawBorder();
    auto rack = model_->getRack(rackId_);
    if (rack) {
//            screen.printText(screenX , screenY + 2 , rack->displayName().c_str(), TextStyle::smallWhiteOnBlack, width , TextAlign::center);
    }
}

void OracRack::scrollView() {
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

void OracRack::moved() {
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

void OracRack::onPotChange(int16_t relativeChange, handle_t handle) {
    if (displayIdx_ < displayOrder_.size()) {
        auto id = displayOrder_[displayIdx_];
        if (modules_.count(id) > 0) {
            auto module = modules_[id];
            module->onPotChange(relativeChange, handle);
        }
    }
}
