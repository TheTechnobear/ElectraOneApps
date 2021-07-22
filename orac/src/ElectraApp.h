#pragma once

#include "Electra.h"
#include "App.h"
// #include "Message.h"
// #include "Event.h"
#include "AppWindows.h"
#include "MidiBase.h"
// #include "Device.h"

// #include "KnobComponent.h"
#include "TextGraphics.h"

class ElectraApp : public App {
public:
	ElectraApp ();

	// App
	void setup (void) override;
	void buttonDown (uint8_t buttonId) override; 
	void touchDown (TouchEvent &touchEvent)override;
	void touchUp (TouchEvent &touchEvent) override;
	void touchHold (TouchEvent &touchEvent) override;
	void touchLongHold (TouchEvent &touchEvent) override;
	void touchClick (TouchEvent &touchEvent) override;
	void touchDoubleClick (TouchEvent &touchEvent) override;
	void potTouchDown (uint8_t potId) override;
	void potTouchUp (uint8_t potId) override;
	void potMove (uint8_t potId, int16_t relativeChange) override;
	void handleIncomingMidiMessage (MidiInput &midiInput,
									MidiMessage &midiMessage) override;
	void handleIncomingSysexMessage (MidiInput &midiInput,
									 SysexMessage &midiMessage) override;
	void handleIncomingControlMessage (MidiInput &midiInput,
									   MidiMessage &midiMessage) override;
	void handleElectraSysex (const SysexBlock &sysexBlock) override;
	bool handleCtrlFileReceived (LocalFile, ElectraCommand::Object) override;
	void runUserTask (void) override;
	void execute (const char *filename) override;

	Windows* getWindows (void) override { return (&windows_);}
	MidiBase* getMidi (void) override;

	bool loadSetup (LocalFile file);

	Window* currentWindow() { return windows_.getCurrentWindow();}


private:
	PageWindow defaultWindow_;
	AppWindows windows_;
	MidiBase midi_;
};
