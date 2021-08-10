#pragma once

#include "ElectraApp.h"
#include "SysExStream.h"


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
            app_->mainWindow()->addRack(r);
        }
    }

    void module(Kontrol::ChangeSource src,
                const Kontrol::Rack &r,
                const Kontrol::Module &m) override {
        if (src == Kontrol::CS_LOCAL) {
        } else {
            auto rack = app_->mainWindow()->getRack(r.id());
            if (rack) {
                rack->addModule(r, m);
            }
        }
    }

    void page(Kontrol::ChangeSource src, const Kontrol::Rack &r,
              const Kontrol::Module &m, const Kontrol::Page &p) override {
        if (src == Kontrol::CS_LOCAL) {
        } else {
            auto rack = app_->mainWindow()->getRack(r.id());
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
        // TODO : check, do I need param handling?
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
            auto rackpage = app_->mainWindow()->getRack(rack.id());
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

    void deleteRack(Kontrol::ChangeSource, const Kontrol::Rack &) override {
        // not using but pure virtual
    }

    void resource(Kontrol::ChangeSource src, const Kontrol::Rack &,
                  const std::string &t, const std::string &v) override {
        if (src != Kontrol::CS_LOCAL) {
            if (t == "module") {
                app_->addModule(v);
            } else if (t == "preset") {
                app_->addPreset(v);
            }
        }
    }

    void loadModule(Kontrol::ChangeSource src, const Kontrol::Rack &rack, const Kontrol::EntityId &modId, const std::string &modType) override {
        if (src == Kontrol::CS_LOCAL) {
            auto &sysex = sysExOutStream_;
            sysex.begin();

            sysex.addHeader(E1_LOAD_MODULE_MSG);
            sysex.addUnsigned(stringToken(rack.id().c_str()));
            sysex.addUnsigned(stringToken(modId.c_str()));
            sysex.addString(modType.c_str());

            sysex.end();
            send(sysex);
//        } else {
        }
    }

    void publishStart(Kontrol::ChangeSource src, unsigned numRacks) override {
        if (src != Kontrol::CS_LOCAL) {
            app_->publishStart();
        }
    }

    void publishRackFinished(Kontrol::ChangeSource src, const Kontrol::Rack &rack) override {
        if (src != Kontrol::CS_LOCAL) {
            app_->publishEnd();
        }
    }

    void savePreset(Kontrol::ChangeSource src, const Kontrol::Rack &rack, std::string preset) override {
        if (src == Kontrol::CS_LOCAL) {
            auto &sysex = sysExOutStream_;
            sysex.begin();

            sysex.addHeader(E1_SAVE_PRESET_MSG);
            sysex.addUnsigned(stringToken(rack.id().c_str()));
            sysex.addString(preset.c_str());
            sysex.end();
            send(sysex);
//        } else {
        }
    }

    void loadPreset(Kontrol::ChangeSource src, const Kontrol::Rack &rack, std::string preset) override {
        if (src == Kontrol::CS_LOCAL) {
            auto &sysex = sysExOutStream_;
            sysex.begin();

            sysex.addHeader(E1_LOAD_PRESET_MSG);
            sysex.addUnsigned(stringToken(rack.id().c_str()));
            sysex.addString(preset.c_str());
            sysex.end();
            send(sysex);
//        } else {
        }
    }

    void saveSettings(Kontrol::ChangeSource src, const Kontrol::Rack &rack) override {
        if (src == Kontrol::CS_LOCAL) {
            auto &sysex = sysExOutStream_;
            sysex.begin();

            sysex.addHeader(E1_SAVE_SETTINGS_MSG);
            sysex.addUnsigned(stringToken(rack.id().c_str()));
            sysex.end();
            send(sysex);
//        } else {
        }
    }

    void midiLearn(Kontrol::ChangeSource src, bool b) override {
        if (src == Kontrol::CS_LOCAL) {
            auto &sysex = sysExOutStream_;
            sysex.begin();

            sysex.addHeader(E1_MIDI_LEARN_MSG);
            sysex.addUnsigned(b);
            sysex.end();
            send(sysex);
//        } else {
        }
    }

    void modulationLearn(Kontrol::ChangeSource src, bool b) override {
        if (src == Kontrol::CS_LOCAL) {
            auto &sysex = sysExOutStream_;
            sysex.begin();

            sysex.addHeader(E1_MOD_LEARN_MSG);
            sysex.addUnsigned(b);
            sysex.end();
            send(sysex);
        } else {
            app_->modulationLearn(b);
        }
    }

/*
 * Unused

    void activeModule(Kontrol::ChangeSource, const Kontrol::Rack &, const Kontrol::Module &) {
    }


    void ping(Kontrol::ChangeSource src,
              const std::string &host, unsigned port,
              unsigned keepAlive) override {
    }

    void assignMidiCC(Kontrol::ChangeSource,
                      const Kontrol::Rack &, const Kontrol::Module &, const Kontrol::Parameter &,
                      unsigned midiCC) override {
    }

    void unassignMidiCC(Kontrol::ChangeSource, const Kontrol::Rack &,
                        const Kontrol::Module &, const Kontrol::Parameter &,
                        unsigned midiCC) override {
    }

    void assignModulation(Kontrol::ChangeSource,
                          const Kontrol::Rack &, const Kontrol::Module &, const Kontrol::Parameter &,
                          unsigned bus) override {
    }

    void unassignModulation(Kontrol::ChangeSource,
                            const Kontrol::Rack &, const Kontrol::Module &, const Kontrol::Parameter &,
                            unsigned bus) override {
    }


    void stop() {}
*/


private:
    void send(SysExOutputStream &sysex) { app_->send(sysex); }

    unsigned stringToken(const char *str) { return app_->stringToken(str); }

    static constexpr int OUTPUT_MAX_SZ = 128;
    SysExOutputStream sysExOutStream_;
    ElectraApp *app_;
};
