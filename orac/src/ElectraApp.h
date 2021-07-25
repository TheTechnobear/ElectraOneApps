#pragma once

#include "App.h"
#include "AppWindows.h"
#include "Electra.h"
#include "MidiBase.h"

#include "OPage.h"
#include "TextComponent.h"
#include "TextGraphics.h"

class E1KontrolCallback;

#include "SysExStream.h"
#include <kontrol/KontrolModel.h>

class E1KontrolCallback;

class ElectraApp : public App {
public:
  ElectraApp();

  // App
  void setup(void) override;
  void buttonDown(uint8_t buttonId) override;
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
  bool handleCtrlFileReceived(LocalFile, ElectraCommand::Object) override;
  void runUserTask(void) override;
  void execute(const char *filename) override;

  Windows *getWindows(void) override { return (&windows_); }
  MidiBase *getMidi(void) override;

  bool loadSetup(LocalFile file);

  Window *currentWindow() { return windows_.getCurrentWindow(); }

private:
	friend class E1KontrolCallback;

  const std::string &tokenString(unsigned tkn);
  unsigned stringToken(const char * str);
  void send(SysExOutputStream& sysex);

  bool handleE1SysEx(Kontrol::ChangeSource src,
  						SysExInputStream &sysex,
                     	std::shared_ptr<Kontrol::KontrolModel> model);

  std::unordered_map<unsigned, std::string> tokenToString_;
  std::map<std::string, unsigned> strToToken_;

  friend class E1KontrolCallback;

  PageWindow defaultWindow_;
  TextComponent rack_, module_, page_;

  OracPage pages_[3] = {0, 1, 2};
  AppWindows windows_;
  MidiBase midi_;
};
