#include "ElectraApp.h"

#include <vector>
#include <memory>

ElectraApp electraApp;


class E1KontrolCallback : public Kontrol::KontrolCallback {
public:
    E1KontrolCallback(ElectraApp *a)
        : app_(a), sysExOutStream_(OUTPUT_MAX_SZ) {
    }

    ~E1KontrolCallback() {}

    void rack(Kontrol::ChangeSource src,
              const Kontrol::Rack &r) override {
        if (src == Kontrol::CS_LOCAL) {
        } else {
            app_->mainWindow().addRack(r);
        }
    }

    void module(Kontrol::ChangeSource src,
                const Kontrol::Rack &r,
                const Kontrol::Module &m) override {
        if (src == Kontrol::CS_LOCAL) {
        } else {
            auto rack = app_->mainWindow().getRack(r.id());
            if (rack) {
                rack->addModule(r, m);
            }
        }

    }

    void page(Kontrol::ChangeSource src, const Kontrol::Rack &r,
              const Kontrol::Module &m, const Kontrol::Page &p) override {
        if (src == Kontrol::CS_LOCAL) {
        } else {
            auto rack = app_->mainWindow().getRack(r.id());
            if (rack) {
                auto module = rack->getModule(m.id());
                if (module) {
                    module->addPage(r, m, p);
                }
            }
        }
    }

    void param(Kontrol::ChangeSource, const Kontrol::Rack &,
               const Kontrol::Module &, const Kontrol::Parameter &) override {
    }

    void changed(Kontrol::ChangeSource src, const Kontrol::Rack &rack,
                 const Kontrol::Module &module,
                 const Kontrol::Parameter &p) override {

        if (src == Kontrol::CS_LOCAL) {
            // logMessage("E1KontrolCallback::changed");

            // if (!broadcastChange(src)) return;
            // if (!isActive()) return;
            auto &sysex = sysExOutStream_;
            sysex.begin();

            sysex.addHeader(E1_CHANGED_MSG);
            sysex.addUnsigned(stringToken(rack.id().c_str()));
            sysex.addUnsigned(stringToken(module.id().c_str()));
            sysex.addString(p.id().c_str());

            auto v = p.current();
            switch (v.type()) {
                case Kontrol::ParamValue::T_Float: {
                    sysex.addUnsigned(v.type());
                    sysex.addFloat(v.floatValue());
                    break;
                }
                case Kontrol::ParamValue::T_String:
                default: {
                    sysex.addUnsigned(Kontrol::ParamValue::T_String);
                    sysex.addString(v.stringValue().c_str());
                }
            }

            sysex.end();
            send(sysex);

        } else {
            auto rackpage = app_->mainWindow().getRack(rack.id());
            if (rackpage) {
                auto modulepage = rackpage->getModule(module.id());
                if (modulepage) {
                    // TODO - need to look for param, and only repaint that ?
//                    dbgMessage("E1KontrolCallback::changed");
                    if (modulepage->isVisible()) modulepage->repaint();
                }
            }
        }
    }

    void resource(Kontrol::ChangeSource, const Kontrol::Rack &,
                  const std::string &, const std::string &) override {
    }

    void deleteRack(Kontrol::ChangeSource, const Kontrol::Rack &) override {}

    // void activeModule(ChangeSource, const Rack &, const Module &) { ; }
    // void loadModule(ChangeSource, const Rack &, const EntityId &, const
    // std::string &) { ; }

    // void ping(ChangeSource src, const std::string &host, unsigned port,
    // unsigned keepAlive) { ; }

    // void assignMidiCC(ChangeSource, const Rack &, const Module &, const
    // Parameter &, unsigned midiCC) { ; } void unassignMidiCC(ChangeSource, const
    // Rack &, const Module &, const Parameter &, unsigned midiCC) { ; }

    // void assignModulation(ChangeSource, const Rack &, const Module &, const
    // Parameter &, unsigned bus) { ; } void unassignModulation(ChangeSource,
    // const Rack &, const Module &, const Parameter &, unsigned bus) { ; }

    void publishStart(Kontrol::ChangeSource, unsigned numRacks) override {}

    void publishRackFinished(Kontrol::ChangeSource, const Kontrol::Rack &) override {}

    // void savePreset(ChangeSource, const Rack &, std::string preset) { ; }

    // void loadPreset(ChangeSource, const Rack &, std::string preset) { ; }

    // void saveSettings(ChangeSource, const Rack &) { ; }

    // void midiLearn(ChangeSource src, bool b) { ; }
    // void modulationLearn(ChangeSource src, bool b) { ; }

    // void stop() { ; }
private:
    void send(SysExOutputStream &sysex) { app_->send(sysex); }

    unsigned stringToken(const char *str) { return app_->stringToken(str); }

    static constexpr int OUTPUT_MAX_SZ = 128;
    SysExOutputStream sysExOutStream_;
    ElectraApp *app_;
};


ElectraApp::ElectraApp() {
    windows_.add(&mainWindow_);
    if (DebugWindow::debugWindow()) windows_.add(DebugWindow::debugWindow());
}


void ElectraApp::setup(void) {
    auto cb = std::make_shared<E1KontrolCallback>(this);
    Kontrol::KontrolModel::model()->addCallback("e1", cb);
    context.setAppName(APP_NAME);
    enableMidi = true;

    LocalFile config(context.getCurrentConfigFile());

    if (!loadSetup(config)) {
        appSetup.useDefault();
    }

    auto window = windows_.getCurrentWindow();

    unsigned ypos = 30;

    window->addGraphics(new TextGraphics(Rectangle(0, ypos, 1024, 20), "Orac",
                                         TextStyle::largeWhiteOnBlack,
                                         TextAlign::center));

    window->repaint();

    logMessage("setup completed");
}


void ElectraApp::buttonDown(uint8_t buttonId) {
//    static unsigned ctr = 0;
//    ctr++;
//    dbgMessage("button counter %d", ctr);
    switch (buttonId) {
        case 1 : {
            if (windows_.getCurrentWindow() != &mainWindow()) return;
            if (mainWindow_.getActiveRack()) {
                mainWindow_.getActiveRack()->prevDisplay();
            }
            break;
        }
        case 2 : {
            if (windows_.getCurrentWindow() != &mainWindow()) return;
            if (mainWindow_.getActiveRack()) {
                mainWindow_.getActiveRack()->nextDisplay();
            }
            break;
        }
        case 3: {
            flushDebug();
            break;
        }
        case 4 : {
            if (windows_.getCurrentWindow() != &mainWindow()) return;
            if (mainWindow_.getActiveRack() && mainWindow_.getActiveRack()->getActiveModule()) {
                mainWindow_.getActiveRack()->getActiveModule()->prevDisplay();
            }
            break;
        }
        case 5 : {
            if (windows_.getCurrentWindow() != &mainWindow()) return;
            if (mainWindow_.getActiveRack() && mainWindow_.getActiveRack()->getActiveModule()) {
                mainWindow_.getActiveRack()->getActiveModule()->nextDisplay();
            }
            break;
        }
        case 6: {
            windows_.nextWindow();
            break;
        }
    }
}

void ElectraApp::potMove(uint8_t potId, int16_t relativeChange) {

}

void ElectraApp::potTouchDown(uint8_t potId) {
    auto window = currentWindow();
    window->processPotTouch(potId, true);
}

void ElectraApp::potTouchUp(uint8_t potId) {
    auto window = currentWindow();
    window->processPotTouch(potId, false);
}

void ElectraApp::touchDown(TouchEvent &touchEvent) {
    if (Component *component = touchEvent.getEventComponent()) {
        component->onTouchDown(touchEvent);
    }
}

void ElectraApp::touchHold(TouchEvent &touchEvent) {
    if (Component *component = touchEvent.getEventComponent()) {
        component->onTouchHold(touchEvent);
    }
}

void ElectraApp::touchUp(TouchEvent &touchEvent) {
    if (Component *component = touchEvent.getEventComponent()) {
        component->onTouchUp(touchEvent);
    }
}

void ElectraApp::touchLongHold(TouchEvent &touchEvent) {}

void ElectraApp::touchClick(TouchEvent &touchEvent) {}

void ElectraApp::touchDoubleClick(TouchEvent &touchEvent) {}

void ElectraApp::handleIncomingMidiMessage(MidiInput &midiInput,
                                           MidiMessage &midiMessage) {}

void ElectraApp::handleIncomingSysexMessage(MidiInput &midiInput,
                                            SysexMessage &midiMessage) {}

void ElectraApp::runUserTask(void) {
//    flushDebug();
}

void ElectraApp::handleIncomingControlMessage(MidiInput &, MidiMessage &) {}


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

        case E1_SYSEX_MAX:
        default: {
            return false;
        }
    }
    return true;
}
