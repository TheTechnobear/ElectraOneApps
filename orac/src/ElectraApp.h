#pragma once

#include <App.h>
#include <MidiBase.h>
#include <TextGraphics.h>

#include <KontrolModel.h>


#include "Components/TextComponent.h"
#include "Windows/AppWindows.h"
#include "Windows/MainWindow.h"
#include "Windows/MenuWindow.h"

#include "Electra.h"
#include "SysExStream.h"

#include "Debug.h"
#include "IUICallback.h"


class E1KontrolCallback;


class ElectraApp : public App, public IUICallback {
public:
    explicit ElectraApp();

    // App
    void setup() override;
    void buttonUp(uint8_t buttonId) override;
    void buttonDown(uint8_t buttonId) override;
    void buttonLongHold(uint8_t buttonId) override;

    void touchDown(TouchEvent &touchEvent) override;
    void touchUp(TouchEvent &touchEvent) override;
    void touchHold(TouchEvent &touchEvent) override;
    void touchLongHold(TouchEvent &touchEvent) override;
    void touchClick(TouchEvent &touchEvent) override;
    void touchDoubleClick(TouchEvent &touchEvent) override;
    void potTouchDown(uint8_t potId) override;
    void potTouchUp(uint8_t potId) override;
    void potMove(uint8_t potId, int16_t relativeChange) override;
    void handleIncomingMidiMessage(MidiInput &midiInput,
                                   MidiMessage &midiMessage) override;
    void handleIncomingSysexMessage(MidiInput &midiInput,
                                    SysexMessage &midiMessage) override;
    void handleIncomingControlMessage(MidiInput &midiInput,
                                      MidiMessage &midiMessage) override;
    void handleElectraSysex(const SysexBlock &sysexBlock) override;
    void runUserTask() override;

    Windows *getWindows() override { return (&windows_); }

    MidiBase *getMidi() override;

    bool loadSetup(LocalFile file);

    OWindow *currentWindow() { return windows_.current(); }

    MainWindow &mainWindow() { return mainWindow_; }


    void publishStart();
    void publishEnd();

    // update menus
    void addPreset(const std::string &preset);
    void addModule(const std::string &module);


    // menu items
    void presetNew();
    void presetSave();
    void modLearn();
    void midiLearn();
    void patchSave();


    void moduleSelect();
    void presetSelect();
private:
    friend class E1KontrolCallback;

    const std::string &tokenString(unsigned tkn);
    unsigned stringToken(const char *str);
    void send(SysExOutputStream &sysex);

    bool handleE1SysEx(Kontrol::ChangeSource src,
                       SysExInputStream &sysex,
                       std::shared_ptr<Kontrol::KontrolModel> model);

    std::unordered_map<unsigned, std::string> tokenToString_;
    std::map<std::string, unsigned> strToToken_;

    friend class E1KontrolCallback;


    MainWindow mainWindow_;
    MenuWindow menuWindow_;
    DebugWindow debugWindow_;
    AppWindows windows_;
    std::shared_ptr<MenuWindow::MenuItem> moduleMenuItems_;
    std::shared_ptr<MenuWindow::MenuItem> presetMenuItems_;
    MidiBase midi_;
};
