#include "OPage.h"

#include "../Debug.h"

void OracPage::paint() {
//        dbgMessage("paint page %s", (moduleId_+":"+pageId_).c_str());
    clearBackground();

    OComponent::paint();

    auto rack = model_->getRack(rackId_);
    auto module = model_->getModule(rack, moduleId_);
    auto page = model_->getPage(module, pageId_);

//        drawBorder();

    if (page) {
        auto &name = page->displayName();

        screen.printText(screenX + 1, screenY + 1, name.c_str(),
                         TextStyle::smallWhiteOnBlack, width - 2,
                         TextAlign::center);
        if (parent_->isActive() && isActive()) {
            screen.drawLine(screenX + 30, screenY + 18, width - 60, 0, COLOR_WHITE);
        }
    }

    if (active_) {
        // screen.drawLine(screenX - 1, screenY - 1, width, 1, 0xFFFF);
    }
}

void OracPage::initParams() {
    auto rack = model_->getRack(rackId_);
    auto module = model_->getModule(rack, moduleId_);
    auto page = model_->getPage(module, pageId_);

    auto params = module->getParams(page);

    int pos = 0;
    const unsigned w = (width - 25) / 2;
    const unsigned h = (height - 20) / 2;
    for (auto param : params) {
        auto ctrl = std::make_shared<OracParam>(*rack, *module, *param, this);
        params_.push_back(ctrl);
//            unsigned x = screenX + 5 + ((pos % 2) * (w + 5));
//            unsigned y = screenY + 20 + ((pos / 2) * (h + 5));

        ctrl->setFgColour(fgClr_);
        ctrl->setBounds(0, 0, w, h);
        ctrl->setVisible(false);
        ctrl->setDimmed(false);
        ctrl->setActive(false);

        getParentWindow()->addComponent(ctrl.get());
        add(ctrl);
        ctrl->assignToWindow(getParentWindow());
        pos++;
    }

    moved();
    visibilityChanged();
}

void OracPage::moved() {
//        OComponent::moved();

    const unsigned w = (width - 25) / 2;
    const unsigned h = (height - 30) / 2;

    int pos = 0;
    for (auto ctrl : params_) {
        unsigned x = screenX + 5 + ((pos % 2) * (w + 5));
        unsigned y = screenY + 20 + ((pos / 2) * (h + 5));
        ctrl->setPosition(x, y);
        pos++;
    }
}
