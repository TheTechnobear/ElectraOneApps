#include "ElectraApp.h"

#include <sstream>
#include <string>


#include <kontrol/KontrolModel.h>

ElectraApp electraApp;

ElectraApp::ElectraApp () {
	windows_.setCurrentWindow (&defaultWindow_);
}


void ElectraApp::setup (void) {
	/*
	 * initialize app context
	 */
	context.setAppName (APP_NAME);
	enableMidi=true;

	/*
	 * Load the setup file. TODO: Setup file should be independent from the app
	 */
	LocalFile config (context.getCurrentConfigFile ());

	if (!loadSetup (config))
	{
		appSetup.useDefault ();
	}

	auto window = currentWindow();

	// knob = new KnobComponent();
	// knob->setBounds(300,430, 120,120);
	// knob->setLabel("Fred");
	// knob->onClick = [this]()
	// {
	// 	logMessage ("knob onClick");
	// 	return (true);
	// };
	// knob->setId (0);
	// window->assignPot(0,0,knob);

	// window->addAndMakeVisible(knob);

	// Add a text object
	window->addGraphics (new TextGraphics (Rectangle (0, 300, 1024, 20),
										   "Orac", TextStyle::largeWhiteOnBlack,
										   TextAlign::center));

	window->repaint ();

	logMessage ("setup completed");
}

void ElectraApp::execute (const char *filename) {
	logMessage ("execute: filename=%s", filename);
}

void ElectraApp::buttonDown (uint8_t buttonId) {
	logMessage ("buttonDown: buttonId=%d", buttonId);
}

void ElectraApp::potMove (uint8_t potId, int16_t relativeChange) {
	logMessage ("potMove: potId=%d, relativeChange=%d", potId, relativeChange);
}

void ElectraApp::potTouchDown (uint8_t potId) {
	auto window = currentWindow();
	window->processPotTouch(potId,true);
	logMessage ("potTouchDown: potId=%d", potId);
}

void ElectraApp::potTouchUp (uint8_t potId) {
	auto window = currentWindow();
	window->processPotTouch(potId,false);
	logMessage ("potTouchUp: potId=%d", potId);
}

void ElectraApp::touchDown (TouchEvent &touchEvent) {
	logMessage ("touchDown: x=%d, y=%d", touchEvent.getScreenX (),
				touchEvent.getScreenY ());
	if (Component *component = touchEvent.getEventComponent ())
	{
		component->onTouchDown (touchEvent);
	}
}

void ElectraApp::touchHold (TouchEvent& touchEvent) {
	logMessage ("touchHold: x=%d, y=%d", touchEvent.getScreenX (),
				touchEvent.getScreenY ());

	if (Component *component = touchEvent.getEventComponent ())
	{
		component->onTouchHold (touchEvent);
	}
}

void ElectraApp::touchUp (TouchEvent& touchEvent) {
	logMessage ("touchUp: x=%d, y=%d", touchEvent.getScreenX (),
				touchEvent.getScreenY ());

	if (Component *component = touchEvent.getEventComponent ())
	{
		component->onTouchUp (touchEvent);
	}
}

void ElectraApp::touchLongHold (TouchEvent& touchEvent) {
	logMessage ("touchLongHold: x=%d, y=%d", touchEvent.getScreenX (),
				touchEvent.getScreenY ());
}

void ElectraApp::touchClick (TouchEvent& touchEvent) {
	logMessage ("touchClick: x=%d, y=%d", touchEvent.getScreenX (),
				touchEvent.getScreenY ());
}

void ElectraApp::touchDoubleClick (TouchEvent& touchEvent) {
	logMessage ("touchDoubleClick: x=%d, y=%d", touchEvent.getScreenX (),
				touchEvent.getScreenY ());
}

void ElectraApp::handleIncomingMidiMessage (MidiInput& midiInput,
											MidiMessage& midiMessage) {
	// logMessage ("midi message: interface=%s, port=%d, channel=%d, type=%s, "
	// 			"data1=%d, data2=%d",
	// 			midiInput.getInterfaceName (), midiInput.getPort (),
	// 			midiMessage.getChannel (), midiMessage.getTypeName (),
	// 			midiMessage.getData1 (), midiMessage.getData2 ());

	// if ((midiMessage.getType () == MidiMessageType::ControlChange) &&
	// 	(midiMessage.getData1 () == 1))
	// {
	// 	slider->setValue (midiMessage.getData2 ());
	// 	drawings->setValue (midiMessage.getData2 ());
	// }
}

void ElectraApp::handleIncomingSysexMessage (MidiInput& midiInput,
											 SysexMessage& midiMessage) {
	logMessage ("---< sysex start: interface=%s, port=%d >---",
				midiInput.getInterfaceName (), midiInput.getPort ());

	SysexBlock sysexBlock = midiMessage.getSysexBlock ();
	size_t sysexLength = sysexBlock.getLength ();

	for (size_t i = 0; i < sysexLength; i++)
	{
		byte sysexByte = sysexBlock.peek (i);
		logMessage ("%d> %X (%c)", i, sysexByte, sysexByte);
	}

	logMessage ("---------------------");
}

void ElectraApp::runUserTask (void) {
	// value_ = (value_ + 1) % 127;
	// slider->setValue(value_);
}

void ElectraApp::handleIncomingControlMessage (MidiInput&, MidiMessage&) {
	logMessage ("handleIncomingControlMessage");
}

bool ElectraApp::handleCtrlFileReceived (LocalFile,
										 ElectraCommand::Object object) {
	logMessage ("handleCtrlFileReceived");
	return (true);
}


static const unsigned TB_SYSEX_MSG = 0x20;

enum SysExMsgs {
    E1_STRING_TKN,
    E1_RACK_MSG,
};


std::unordered_map<unsigned,std::string> tokenToString_;

unsigned readToken(const SysexBlock& sysexBlock, unsigned& idx) {
	unsigned msb= sysexBlock.peek(idx++);
	unsigned lsb= sysexBlock.peek(idx++);
	unsigned id= (msb << 7) + lsb;
	return id;
}

std::string readString(const SysexBlock& sysexBlock, unsigned& idx){
	unsigned tkn = readToken(sysexBlock,idx);
	auto e= tokenToString_.find(tkn);
	if(e!=tokenToString_.end()) {
		return e->second;
	}
	logMessage("unknown string (%x) ", tkn);
	return "";
}

unsigned readUnsigned(const SysexBlock& sysexBlock, unsigned& idx) {
	unsigned msb= sysexBlock.peek(idx++);
	unsigned lsb= sysexBlock.peek(idx++);
	unsigned v= (msb << 7) + lsb;

	// logMessage("readUnsigned %d %d %d", msb, lsb, v);
	return v;
}

void ElectraApp::handleElectraSysex (const SysexBlock& sysexBlock) {
	// logMessage ("handleElectraSysex");

	auto len = sysexBlock.getLength ();
	unsigned msgStart= 1 + 3;
	if(len < (msgStart + 1 + 1 + 1) ) { // F0 mfgid tbid msgtype F7
		logMessage("message too short");
		return;
	}

	unsigned idx=msgStart;


	byte TB_ID=sysexBlock.peek(idx++);
	if(TB_ID!=TB_SYSEX_MSG) {
		logMessage("unknown developer id (%x) ", TB_ID);
		return;
	}

	Kontrol::ChangeSource src= Kontrol::ChangeSource(Kontrol::ChangeSource::SrcType::REMOTE, "E1");
	auto model_=Kontrol::KontrolModel::model();
	byte msgid=sysexBlock.peek(idx++);
	switch (msgid) {
		case E1_STRING_TKN : {
			std::stringbuf buf;

			unsigned tkn= ::readToken(sysexBlock,idx);
			while(idx < len - 1) {
				unsigned char ch = sysexBlock.peek(idx++);
				buf.sputc(ch);
			}
			std::string str= buf.str();
			tokenToString_[tkn]=str;
			logMessage("New String tkn=%d str=%s", tkn, str.c_str());
			break;

		}
		case E1_RACK_MSG : {

			Kontrol::EntityId rackId= readString(sysexBlock, idx);
			std::string host= readString(sysexBlock,idx);
			unsigned port = readUnsigned(sysexBlock,idx);
			logMessage("Rack %s %s %u", rackId.c_str(), host.c_str(),port);
		    model_->createRack(src, rackId, host, port);
			break;
		}
		default: {
			logMessage("unknown message type (%x) ", msgid);
			for (size_t i = 0; i < len; i++) {
				byte sysexByte = sysexBlock.peek (i);
				logMessage ("%d> %X (%c)", i, sysexByte, sysexByte);
			}
			break;
		}

	}
}



MidiBase* ElectraApp::getMidi (void) {
	return (&midi_);
}

/** Load setup. TODO: setup is still having parts related to the ctrlv2 app
 *
 */
bool ElectraApp::loadSetup (LocalFile file) {
	if (!appSetup.load (file.getFilepath ()))
	{
		return (false);
	}

	/*
	 * Use setup patterns to assign USB host devices to MIDI ports
	 */
	assignUSBdevicesToPorts ();

	/*
	 * configure MIDI IO thru
	 */
	if (appSetup.router.midiIo1Thru)
	{
		midiDINInterfaces[0]->turnThruOn ();
	}
	else
	{
		midiDINInterfaces[0]->turnThruOff ();
	}

	if (appSetup.router.midiIo2Thru)
	{
		midiDINInterfaces[1]->turnThruOn ();
	}
	else
	{
		midiDINInterfaces[1]->turnThruOff ();
	}

	return (true);
}
