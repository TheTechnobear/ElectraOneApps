#pragma once

#include "Electra.h"
#include "App.h"
// #include "Message.h"
// #include "Event.h"
#include "Windows.h"
#include "MidiBase.h"
// #include "Device.h"

// #include "KnobComponent.h"
#include "TextGraphics.h"

class ElectraApp : public App
{
	public:
		ElectraApp ();

		void setup (void);
		void buttonDown (uint8_t buttonId);
		void touchDown (TouchEvent &touchEvent);
		void touchUp (TouchEvent &touchEvent);
		void touchHold (TouchEvent &touchEvent);
		void touchLongHold (TouchEvent &touchEvent);
		void touchClick (TouchEvent &touchEvent);
		void touchDoubleClick (TouchEvent &touchEvent);
		void potTouchDown (uint8_t potId);
		void potTouchUp (uint8_t potId);
		void potMove (uint8_t potId, int16_t relativeChange);
		void handleIncomingMidiMessage (MidiInput &midiInput,
										MidiMessage &midiMessage);
		void handleIncomingSysexMessage (MidiInput &midiInput,
										 SysexMessage &midiMessage);
		void handleIncomingControlMessage (MidiInput &midiInput,
										   MidiMessage &midiMessage);
		void handleElectraSysex (const SysexBlock &sysexBlock);
		bool handleCtrlFileReceived (LocalFile, ElectraCommand::Object);
		void runUserTask (void);
		void execute (const char *filename);
		bool loadSetup (LocalFile file);


		Windows* getWindows (void) {
			return (&windows_);
		}
		Window* currentWindow() {
			return windows_.getCurrentWindow();
		}

		MidiBase* getMidi (void);

	private:
		PageWindow defaultWindow_;
		Windows windows_;
		MidiBase midi_;
};
