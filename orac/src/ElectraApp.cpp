#include "ElectraApp.h"

#include <sstream>
#include <string>
#include <vector>
#include <memory>

ElectraApp electraApp;

ElectraApp::ElectraApp() { windows_.setCurrentWindow(&defaultWindow_); }

class E1KontrolCallback : public Kontrol::KontrolCallback {
public:
    E1KontrolCallback(ElectraApp *app)
        : app_(app), sysExOutStream_(OUTPUT_MAX_SZ) {
        ;
    }

    ~E1KontrolCallback() { ; }

    void rack(Kontrol::ChangeSource, const Kontrol::Rack &r) override {
        app_->rack_.setLabel(r.host().c_str());
    }

    void module(Kontrol::ChangeSource, const Kontrol::Rack &r,
                const Kontrol::Module &m) override {
        app_->module_.setLabel(m.type().c_str());
    }

    void page(Kontrol::ChangeSource src, const Kontrol::Rack &r,
              const Kontrol::Module &m, const Kontrol::Page &p) override {
        if (src == Kontrol::CS_LOCAL) { ;
        } else {
            auto model = Kontrol::KontrolModel::model();
            auto rack = model->getRack(r.id());
            auto module = model->getModule(rack, m.id());
            auto page = model->getPage(module, p.id());

            app_->page_.setLabel(p.displayName().c_str());
            for (unsigned i = 0; i < 3; i++) {
                if (app_->pages_[i].pageId().size() == 0) {
                    app_->pages_[i].setPage(r.id(), m.id(), p.id());
                    return;
                }
            }
        }
    }

    void param(Kontrol::ChangeSource, const Kontrol::Rack &,
               const Kontrol::Module &, const Kontrol::Parameter &) override {
        ;
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
            for (unsigned i = 0; i < 3; i++) {
                if (app_->pages_[i].moduleId() == module.id()) {
                    app_->pages_[i].repaint();
                }
            }
        }
    }

    void resource(Kontrol::ChangeSource, const Kontrol::Rack &,
                  const std::string &, const std::string &) override {
        ;
    }

    void deleteRack(Kontrol::ChangeSource, const Kontrol::Rack &) override { ; }

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

    void publishStart(Kontrol::ChangeSource, unsigned numRacks) {}

    void publishRackFinished(Kontrol::ChangeSource, const Kontrol::Rack &) {}

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

// static int16_t rgbToRgb565(uint8_t r, uint8_t g, uint8_t b) {
//   uint16_t rgb565 = 0;
//   rgb565 += uint16_t(r & 0b011111) << 11;
//   rgb565 += uint16_t(g & 0b011111) << 5;
//   rgb565 += (b & 0b011111);
//   return rgb565;
// }

void ElectraApp::setup(void) {
    auto cb = std::make_shared<E1KontrolCallback>(this);
    Kontrol::KontrolModel::model()->addCallback("e1", cb);
    context.setAppName(APP_NAME);
    enableMidi = true;

    LocalFile config(context.getCurrentConfigFile());

    if (!loadSetup(config)) {
        appSetup.useDefault();
    }

    auto window = currentWindow();

    unsigned ypos = 30;

    window->addGraphics(new TextGraphics(Rectangle(0, ypos, 1024, 20), "Orac",
                                         TextStyle::largeWhiteOnBlack,
                                         TextAlign::center));

    unsigned sp = 20;
    unsigned xpos = sp;
    unsigned bsz = 240;

    uint16_t clrs_[3] = {ElectraColours::getNumericRgb565(ElectraColours::red),
                         ElectraColours::getNumericRgb565(ElectraColours::orange),
                         ElectraColours::getNumericRgb565(ElectraColours::blue)};
    // uint16_t clrs_[3] = {rgbToRgb565(0xF, 0x0, 0x0), gbToRgb565(0x0, 0xF, 0x0),
    //                      rgbToRgb565(0x0, 0x0, 0xF)};

    for (unsigned i = 0; i < 3; i++) {
        pages_[i].setBounds(xpos, 60, bsz, bsz);
        pages_[i].setfgColour(clrs_[i]);
        xpos += bsz + sp;
        window->addAndMakeVisible(&pages_[i]);
        pages_[i].init();
    }

    // pages_[0].onClick = [this]() {
    //   logMessage("page 0 touch");
    //   return true;
    // };

    // labels
    ypos = 400;
    xpos = 700;
    rack_.setBounds(xpos, ypos, 300, 30);
    rack_.setLabel("Rack");
    window->addAndMakeVisible(&rack_);

    ypos += 30;
    module_.setBounds(xpos, ypos, 300, 30);
    module_.setLabel("Module");
    window->addAndMakeVisible(&module_);

    ypos += 30;
    page_.setBounds(xpos, ypos, 300, 30);
    page_.setLabel("Page");
    window->addAndMakeVisible(&page_);

    window->repaint();

    logMessage("setup completed");
}

void ElectraApp::execute(const char *filename) {}

void ElectraApp::buttonDown(uint8_t buttonId) {
    if (buttonId == 1) {
        rack_.setLabel("");
        page_.setLabel("");
        module_.setLabel("");
    }
}

void ElectraApp::potMove(uint8_t potId, int16_t relativeChange) {}

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

void ElectraApp::runUserTask(void) {}

void ElectraApp::handleIncomingControlMessage(MidiInput &, MidiMessage &) {}

bool ElectraApp::handleCtrlFileReceived(LocalFile,
                                        ElectraCommand::Object object) {
    return (true);
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
    if (!valid) {
        logMessage("invalid sysex header");
    };
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

            // logMessage("E1_MODULE_MSG %s %s %s %s", rackId.c_str(), moduleId.c_str(),
            //            displayName.c_str(), type.c_str());
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
