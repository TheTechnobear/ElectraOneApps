#include "ElectraApp.h"

#include <vector>
#include <memory>

#include "Windows/MainWindow.h"
#include "Windows/MenuWindow.h"
#include "Debug.h"


#include "E1KontrolCallback.h"

ElectraApp electraApp;

ElectraApp::ElectraApp() {
    mainWindow_ = std::make_shared<MainWindow>();
    moduleWindow_ = std::make_shared<ModuleWindow>();
    presetWindow_ = std::make_shared<PresetWindow>();
    windows_.add(mainWindow_.get());
    windows_.add(moduleWindow_.get());
    windows_.add(presetWindow_.get());
    if (DebugWindow::debugWindow()) windows_.add(DebugWindow::debugWindow());
}

void ElectraApp::setup(void) {
    windows_.select(AppWindows::DEBUG);
    dbgMessage("Orac Firmware");
    dbgMessage("Waiting for Orac and MEC to start on host");

    auto cb = std::make_shared<E1KontrolCallback>(this);
    Kontrol::KontrolModel::model()->addCallback("e1", cb);
    context.setAppName(APP_NAME);
    enableMidi = true;

    moduleMenuItems_ = moduleWindow_->getMenu();
    presetMenuItems_ = presetWindow_->getMenu();

    //TODO midi/mod learn and save
//    menu->addItem("Midi Learn", [this](void) { menuMidiLearn(); });
//    menu->addItem("Mod Learn", [this](void) { menuModLearn(); });
//    menu->addItem("Save", [this](void) { menuSaveSettings(); });

    LocalFile config(context.getCurrentConfigFile());

    if (!loadSetup(config)) {
        appSetup.useDefault();
    }

//    auto window = windows_.getCurrentWindow();
//    unsigned ypos = 30;
//
//    window->addGraphics(new TextGraphics(Rectangle(0, ypos, 1024, 20), "Orac",
//                                         TextStyle::largeWhiteOnBlack,
//                                         TextAlign::center));
//
//    window->repaint();

}

void ElectraApp::buttonUp(uint8_t buttonId) {
    if (auto w = buttonDownWin_[buttonId]) w->buttonUp(buttonId);
    buttonDownWin_[buttonId] = nullptr;
}

void ElectraApp::buttonLongHold(uint8_t buttonId) {
    if (auto w = buttonDownWin_[buttonId]) w->buttonLongHold(buttonId);
}

void ElectraApp::buttonDown(uint8_t buttonId) {
    auto w = currentWindow();
    if (w) w->buttonDown(buttonId);
    buttonDownWin_[buttonId] = w;
}

void ElectraApp::potMove(uint8_t potId, int16_t relativeChange) {
    if (auto w = currentWindow()) w->potMove(potId, relativeChange);
}

void ElectraApp::potTouchDown(uint8_t potId) {
    if (auto w = currentWindow()) w->potTouchDown(potId);
}

void ElectraApp::potTouchUp(uint8_t potId) {
    if (auto w = currentWindow()) w->potTouchUp(potId);
}

void ElectraApp::touchDown(TouchEvent &touchEvent) {
    if (auto w = currentWindow()) w->touchDown(touchEvent);
}

void ElectraApp::touchMove(TouchEvent &touchEvent) {
    if (auto w = currentWindow()) w->touchMove(touchEvent);
}

void ElectraApp::touchUp(TouchEvent &touchEvent) {
    if (auto w = currentWindow()) w->touchUp(touchEvent);
}

void ElectraApp::touchLongHold(TouchEvent &touchEvent) {
    if (auto w = currentWindow()) w->touchLongHold(touchEvent);
}

void ElectraApp::touchClick(TouchEvent &touchEvent) {
    if (auto w = currentWindow()) w->touchClick(touchEvent);
}

void ElectraApp::touchDoubleClick(TouchEvent &touchEvent) {
    if (auto w = currentWindow()) w->touchDoubleClick(touchEvent);
}


void ElectraApp::handleIncomingMidiMessage(MidiInput &midiInput, MidiMessage &midiMessage) {
}

void ElectraApp::handleIncomingSysexMessage(MidiInput &midiInput, SysexMessage &midiMessage) {
}

void ElectraApp::runUserTask(void) {
}

void ElectraApp::handleIncomingControlMessage(MidiInput &, MidiMessage &) {
}


MidiBase *ElectraApp::getMidi(void) { return (&midi_); }

/** Load setup. TODO: setup is still having parts related to the ctrlv2 app
 *
 */
bool ElectraApp::loadSetup(LocalFile file) {
    if (!appSetup.load(file.getFilepath())) {
        return (false);
    }

    /*
     * Use setup patterns to assign USB host devices to MIDI ports
     */
    assignUSBdevicesToPorts();

    /*
     * configure MIDI IO thru
     */
    if (appSetup.router.midiIo1Thru) {
        midiDINInterfaces[0]->turnThruOn();
    } else {
        midiDINInterfaces[0]->turnThruOff();
    }

    if (appSetup.router.midiIo2Thru) {
        midiDINInterfaces[1]->turnThruOn();
    } else {
        midiDINInterfaces[1]->turnThruOff();
    }

    return (true);
}

const std::string &ElectraApp::tokenString(unsigned tkn) {
    static const std::string invalid("INVALID");
    auto str = tokenToString_.find(tkn);
    if (str == tokenToString_.end()) {
        return invalid;
    }
    return str->second;
}

unsigned ElectraApp::stringToken(const char *str) {
    auto tkn = strToToken_.find(str);
    if (tkn == strToToken_.end()) {
        logMessage("ElectraApp::stringToken - cannot find token %s", str);
        return 0;
    }
    return tkn->second;
}

#include "sysex.h"

void ElectraApp::send(SysExOutputStream &sysex) {

    sendSysData(sysex.buffer(), sysex.size(), USB_MIDI_PORT_CTRL);
}

void ElectraApp::handleElectraSysex(const SysexBlock &sysexBlock) {
    // logMessage ("handleElectraSysex");
    Kontrol::ChangeSource src =
        Kontrol::ChangeSource(Kontrol::ChangeSource::SrcType::REMOTE, "E1");
    SysExInputStream sysex(sysexBlock);
    auto model = Kontrol::KontrolModel::model();
    bool ok = handleE1SysEx(src, sysex, model);

    if (!ok) {
        auto len = sysexBlock.getLength();
        logMessage("unknown sysex msg ");
        for (size_t i = 0; i < len; i++) {
            unsigned char sysexByte = sysexBlock.peek(i);
            logMessage("%d> %X (%c)", i, sysexByte, sysexByte);
        }
    }
}

bool ElectraApp::handleE1SysEx(Kontrol::ChangeSource src,
                               SysExInputStream &sysex,
                               std::shared_ptr<Kontrol::KontrolModel> model) {
    bool valid = sysex.readHeader();
    if (!sysex.isValid()) {
        dbgMessage("handleE1SysEx :: invalid sysex header");
    }
    if (!valid) {
        dbgMessage("invalid sysex header");
    }

    char msgid = sysex.read();

    switch (msgid) {
        case E1_STRING_TKN: {
            unsigned tkn = sysex.readUnsigned();
            std::string str = sysex.readString();
            tokenToString_[tkn] = str;
            strToToken_[std::string(str)] = tkn;
            // logMessage("New String tkn=%d str=%s", tkn, str.c_str());
            break;
        }
        case E1_START_PUB: {
            tokenToString_.clear();
            unsigned numRacks = sysex.readUnsigned();
            // logMessage("E1_START_PUB %d", numRacks);
            model->publishStart(src, numRacks);
            break;
        }
        case E1_RACK_END: {
            Kontrol::EntityId rackId = tokenString(sysex.readUnsigned());
            // logMessage("E1_RACK_END %s", rackId.c_str());
            model->publishRackFinished(src, rackId);
            break;
        }
        case E1_RACK_MSG: {
            Kontrol::EntityId rackId = tokenString(sysex.readUnsigned());
            std::string host = sysex.readString();
            unsigned port = sysex.readUnsigned();

            // logMessage("E1_RACK_MSG %s %s %u", rackId.c_str(), host.c_str(), port);
            model->createRack(src, rackId, host, port);
            break;
        }
        case E1_MODULE_MSG: {
            Kontrol::EntityId rackId = tokenString(sysex.readUnsigned());
            Kontrol::EntityId moduleId = tokenString(sysex.readUnsigned());
            std::string displayName = sysex.readString();
            std::string type = sysex.readString();
//            dbgMessage("E1_MODULE_MSG %s %s %s %s", rackId.c_str(), moduleId.c_str(),
//                       displayName.c_str(), type.c_str());
            model->createModule(src, rackId, moduleId, displayName, type);
            break;
        }
        case E1_PAGE_MSG: {
            Kontrol::EntityId rackId = tokenString(sysex.readUnsigned());
            Kontrol::EntityId moduleId = tokenString(sysex.readUnsigned());
            Kontrol::EntityId pageId = tokenString(sysex.readUnsigned());
            std::string displayName = sysex.readString();

            std::vector<Kontrol::EntityId> paramIds;
            while (!sysex.atEnd()) {
                std::string val = sysex.readString();
                paramIds.push_back(val);
            }

            // logMessage("E1_PAGE_MSG %s %s %s %s", rackId.c_str(), moduleId.c_str(),
            //            pageId.c_str(), displayName.c_str());
            model->createPage(src, rackId, moduleId, pageId, displayName, paramIds);

            break;
        }
        case E1_PARAM_MSG: {
            std::vector<Kontrol::ParamValue> params;
            Kontrol::EntityId rackId = tokenString(sysex.readUnsigned());
            Kontrol::EntityId moduleId = tokenString(sysex.readUnsigned());
            while (!sysex.atEnd()) {
                auto type = (Kontrol::ParamValue::Type) sysex.readUnsigned();
                switch (type) {
                    case Kontrol::ParamValue::T_Float: {
                        float val = sysex.readFloat();
                        params.push_back(Kontrol::ParamValue(val));
                        break;
                    }
                    case Kontrol::ParamValue::T_String:
                    default: {
                        std::string val = sysex.readString();
                        params.push_back(Kontrol::ParamValue(val));
                        break;
                    }
                }
            }

            // logMessage("E1_PARAM_MSG %s %s", rackId.c_str(), moduleId.c_str());
            model->createParam(src, rackId, moduleId, params);
            break;
        }
        case E1_CHANGED_MSG: {
            Kontrol::EntityId rackId = tokenString(sysex.readUnsigned());
            Kontrol::EntityId moduleId = tokenString(sysex.readUnsigned());
            Kontrol::EntityId paramId = sysex.readString();

            if (!sysex.atEnd()) {
                auto type = (Kontrol::ParamValue::Type) sysex.readUnsigned();
                switch (type) {
                    case Kontrol::ParamValue::T_Float: {
                        float val = sysex.readFloat();
                        // logMessage("E1_CHANGED_MSG %s %s %s %f" , rackId.c_str(),
                        // moduleId.c_str(),paramId.c_str(), val);
                        model->changeParam(src, rackId, moduleId, paramId,
                                           Kontrol::ParamValue(val));
                        break;
                    }

                    case Kontrol::ParamValue::T_String:
                    default: {
                        std::string val = sysex.readString();
                        // logMessage("E1_CHANGED_MSG %s %s %s %s", rackId.c_str(),
                        // moduleId.c_str(),paramId.c_str(), val.c_str());
                        model->changeParam(src, rackId, moduleId, paramId,
                                           Kontrol::ParamValue(val));
                        break;
                    }
                }
            }
            break;
        }
        case E1_RESOURCE_MSG: {
            Kontrol::EntityId rackId = tokenString(sysex.readUnsigned());
            Kontrol::EntityId resType = tokenString(sysex.readUnsigned());
            std::string resValue = sysex.readString();
            // logMessage("E1_RESOURCE_MSG %s %s %s", rackId.c_str(),
            // resType.c_str(),resValue.c_str());
            model->createResource(src, rackId, resType, resValue);
            break;
        }
        case E1_LOAD_MODULE_MSG : {
            Kontrol::EntityId rackId = tokenString(sysex.readUnsigned());
            Kontrol::EntityId modId = tokenString(sysex.readUnsigned());
            std::string modType = sysex.readString();
            model->loadModule(src, rackId, modId, modType);
            break;
        }
        case E1_SAVE_PRESET_MSG : {
            Kontrol::EntityId rackId = tokenString(sysex.readUnsigned());
            std::string preset = sysex.readString();
            model->savePreset(src, rackId, preset);
            break;
        }
        case E1_LOAD_PRESET_MSG : {
            Kontrol::EntityId rackId = tokenString(sysex.readUnsigned());
            std::string preset = sysex.readString();
            model->loadPreset(src, rackId, preset);
            break;
        }
        case E1_SAVE_SETTINGS_MSG : {
            Kontrol::EntityId rackId = tokenString(sysex.readUnsigned());
            model->saveSettings(src, rackId);
            break;
        }
        case E1_MIDI_LEARN_MSG : {
            bool b = sysex.readUnsigned() > 0;
            model->midiLearn(src, b);
            break;
        }
        case E1_MOD_LEARN_MSG : {
            bool b = sysex.readUnsigned() > 0;
            model->modulationLearn(src, b);
            break;
        }
        case E1_SYSEX_MAX:
        default: {
            return false;
        }
    }
    return true;
}

// start end of setup {
void ElectraApp::publishStart() {
    dbgMessage("Initialising from Orac");
    moduleWindow_->clearMenu(moduleMenuItems_);
    presetWindow_->clearMenu(presetMenuItems_);

    presetMenuItems_->addItem("Save Preset", [this](void) { menuPresetSave(); });
    presetMenuItems_->addItem("New Preset", [this](void) { menuPresetNew(); });

    //TODO , clear model?
}

void ElectraApp::publishEnd() {
    dbgMessage("Initialisation from Orac complete");
    windows_.select(AppWindows::MAIN);
}

// menu management
void ElectraApp::addPreset(const std::string &p) {
    if (!presetMenuItems_->find(p)) {
        presetMenuItems_->addItem(p, [this]() { menuPresetSelect(); });
    }
}

void ElectraApp::addModule(const std::string &m) {
    size_t pos = m.find('/');
    if (pos == std::string::npos) {
        if (!moduleMenuItems_->find(m)) {
            moduleMenuItems_->addItem(m, [this]() { menuModuleSelect(); });
        }
    } else {
        std::string cat = m.substr(0, pos + 1);
        std::string mod = m.substr(pos + 1);
        auto catmenu = moduleMenuItems_->find(cat);
        if (!catmenu) {
            catmenu = moduleMenuItems_->addMenu(cat);
        }
        if (!catmenu->find(mod)) {
            catmenu->addItem(mod, [this]() { menuModuleSelect(); });
        }
    }
}

void ElectraApp::selectModule() {
    auto orack = mainWindow()->getActiveRack();
    if (orack) {
        auto omodule = orack->getActiveModule();
        if (omodule) {
            auto model = Kontrol::KontrolModel::model();
            auto rack = model->getRack(omodule->rackId());
            auto module = model->getModule(rack, omodule->moduleId());
            if (module) {
                auto modtype = module->type();
                moduleWindow_->select(modtype);
            }
        }
    }
}


void ElectraApp::midiLearn(bool b) {
    midiLearnActive_ = b;
}

void ElectraApp::modulationLearn(bool b) {
    modulationLearnActive_ = b;
}

// menu items
void ElectraApp::menuPresetNew() {
    auto model = Kontrol::KontrolModel::model();
    auto orack = mainWindow()->getActiveRack();
    if (orack) {
        auto rackId = orack->rackId();
        auto pRack = model->getRack(rackId);
        if (pRack) {
            auto presets = pRack->getResources("preset");
            int sz = presets.size();
            std::string newPreset = "new-" + std::to_string(sz);
            dbgMessage("create new preset : %s", newPreset.c_str());
            model->savePreset(Kontrol::CS_LOCAL, rackId, newPreset);
            windows_.select(AppWindows::MAIN);
        }
    }
}

void ElectraApp::menuPresetSave() {
    auto model = Kontrol::KontrolModel::model();
    auto orack = mainWindow()->getActiveRack();
    if (orack) {
        auto rackId = orack->rackId();
        auto pRack = model->getRack(rackId);
        if (pRack) {
            auto preset = pRack->currentPreset();
            model->savePreset(Kontrol::CS_LOCAL, rackId, preset);
            windows_.select(AppWindows::MAIN);
        }
    }
}

void ElectraApp::menuModLearn() {
    auto model = Kontrol::KontrolModel::model();
    bool b = !modulationLearn();
    model->modulationLearn(Kontrol::CS_LOCAL, b);
    windows_.select(AppWindows::MAIN);
}

void ElectraApp::menuMidiLearn() {
    auto model = Kontrol::KontrolModel::model();
    bool b = !midiLearn();
    model->midiLearn(Kontrol::CS_LOCAL, b);
    windows_.select(AppWindows::MAIN);
}

void ElectraApp::menuSaveSettings() {
    auto model = Kontrol::KontrolModel::model();
    auto orack = mainWindow()->getActiveRack();
    if (orack) {
        model->saveSettings(Kontrol::CS_LOCAL, orack->rackId());
        windows_.select(AppWindows::MAIN);
    }
}


void ElectraApp::menuModuleSelect() {
    auto model = Kontrol::KontrolModel::model();
    unsigned cidx = moduleMenuItems_->idx_;
    if (cidx < moduleMenuItems_->items_.size()) {
        auto catItem = moduleMenuItems_->items_[cidx];
        if (catItem->items_.size() > 0) {
            // normal case, category has items
            unsigned midx = catItem->idx_;
            if (midx < catItem->items_.size()) {
                auto modItem = catItem->items_[midx];
                std::string module = catItem->name_ + modItem->name_;
                dbgMessage("moduleSelect : module %s", module.c_str());

                auto orack = mainWindow()->getActiveRack();
                if (orack) {
                    auto omod = orack->getActiveModule();
                    if (omod) {
                        model->loadModule(Kontrol::CS_LOCAL, omod->rackId(), omod->moduleId(), module);
                        windows_.select(AppWindows::MAIN);
                    }
                }
            }
        } else {
            // for some reason we have module in top level
            std::string module = catItem->name_;
            dbgMessage("moduleSelect : module %s (no cat) ", module.c_str());
            auto orack = mainWindow()->getActiveRack();
            if (orack) {
                auto omod = orack->getActiveModule();
                if (omod) {
                    model->loadModule(Kontrol::CS_LOCAL, omod->rackId(), omod->moduleId(), module);
                    windows_.select(AppWindows::MAIN);
                }
            }
        }
    }
}

void ElectraApp::menuPresetSelect() {
    unsigned pidx = presetMenuItems_->idx_;
    if (pidx < presetMenuItems_->items_.size()) {
        auto pItem = presetMenuItems_->items_[pidx];
        std::string preset = pItem->name_;
//        dbgMessage("presetSelect : preset %s", preset.c_str());

        auto rackId = mainWindow()->getActiveRack()->rackId();
        auto model = Kontrol::KontrolModel::model();
        model->loadPreset(Kontrol::CS_LOCAL, rackId, preset);
        windows_.select(AppWindows::MAIN);
    }
}