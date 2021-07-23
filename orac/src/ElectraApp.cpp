#include "ElectraApp.h"

#include <sstream>
#include <string>
#include <vector>


#include <kontrol/KontrolModel.h>

ElectraApp electraApp;

ElectraApp::ElectraApp () {
	windows_.setCurrentWindow (&defaultWindow_);
}

class E1KontrolCallback : public  Kontrol::KontrolCallback {
public:
    E1KontrolCallback(ElectraApp* app) : app_(app) { ; }

    // ~E1KontrolCallback() { ; }

   void rack(Kontrol::ChangeSource, const Kontrol::Rack &r) override {
   		app_->rack_.setLabel(r.host().c_str());
   }
   void module(Kontrol::ChangeSource, const Kontrol::Rack &r , const Kontrol::Module &m) override {
   		app_->module_.setLabel(m.type().c_str());
   }

   void page(Kontrol::ChangeSource, const Kontrol::Rack &, const Kontrol::Module & , const Kontrol::Page &p) override {
   		app_->page_.setLabel(p.displayName().c_str());
   }

   void param(Kontrol::ChangeSource, const Kontrol::Rack &, const Kontrol::Module &, const Kontrol::Parameter &) override { ; } 
   void changed(Kontrol::ChangeSource, const Kontrol::Rack &, const Kontrol::Module &, const Kontrol::Parameter &) override { ; } 
   void resource(Kontrol::ChangeSource, const Kontrol::Rack &, const std::string &, const std::string &) override { ; } 

   void deleteRack(Kontrol::ChangeSource, const Kontrol::Rack &) override { ; } 

   // void activeModule(ChangeSource, const Rack &, const Module &) { ; }
   // void loadModule(ChangeSource, const Rack &, const EntityId &, const std::string &) { ; }

   // void ping(ChangeSource src, const std::string &host, unsigned port, unsigned keepAlive) { ; }

   // void assignMidiCC(ChangeSource, const Rack &, const Module &, const Parameter &, unsigned midiCC) { ; }
   // void unassignMidiCC(ChangeSource, const Rack &, const Module &, const Parameter &, unsigned midiCC) { ; }

   // void assignModulation(ChangeSource, const Rack &, const Module &, const Parameter &, unsigned bus) { ; }
   // void unassignModulation(ChangeSource, const Rack &, const Module &, const Parameter &, unsigned bus) { ; }

   void publishStart(Kontrol::ChangeSource, unsigned numRacks) {
 	}

   void publishRackFinished(Kontrol::ChangeSource, const Kontrol::Rack &) {
 	}

   // void savePreset(ChangeSource, const Rack &, std::string preset) { ; }

   // void loadPreset(ChangeSource, const Rack &, std::string preset) { ; }

   // void saveSettings(ChangeSource, const Rack &) { ; }


   // void midiLearn(ChangeSource src, bool b) { ; }
   // void modulationLearn(ChangeSource src, bool b) { ; }

   // void stop() { ; }
private:
	ElectraApp* app_;
};


void ElectraApp::setup (void) {
	auto cb = std::make_shared<E1KontrolCallback>(this);
	Kontrol::KontrolModel::model()->addCallback("e1",cb);
	context.setAppName (APP_NAME);
	enableMidi=true;

	LocalFile config (context.getCurrentConfigFile ());

	if (!loadSetup (config))
	{
		appSetup.useDefault ();
	}

	auto window = currentWindow();

	unsigned ypos=30;

	window->addGraphics (new TextGraphics (Rectangle (0, ypos, 1024, 20),
										   "Orac", TextStyle::largeWhiteOnBlack,
										   TextAlign::center));

	ypos+=60;
	rack_.setBounds(0,ypos,1024,30);
	rack_.setLabel("Rack");
	window->addAndMakeVisible(&rack_);

	ypos+=30;
	module_.setBounds(0,ypos,1024,30);
	module_.setLabel("Module");
	window->addAndMakeVisible(&module_);

	ypos+=30;
	page_.setBounds(0,ypos,1024,30);
	page_.setLabel("Page");
	window->addAndMakeVisible(&page_);


	window->repaint ();

	logMessage ("setup completed");
}

void ElectraApp::execute (const char *filename) {
	logMessage ("execute: filename=%s", filename);
}

void ElectraApp::buttonDown (uint8_t buttonId) {
	logMessage ("buttonDown: buttonId=%d", buttonId);
	if(buttonId==1) {
		rack_.setLabel("");
		page_.setLabel("");
		module_.setLabel("");
	}
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
}

void ElectraApp::handleIncomingSysexMessage (MidiInput& midiInput,
											 SysexMessage& midiMessage) {
}

void ElectraApp::runUserTask (void) {
	// value_ = (value_ + 1) % 127;
	// slider->setValue(value_);
}

void ElectraApp::handleIncomingControlMessage (MidiInput&, MidiMessage&) {
}

bool ElectraApp::handleCtrlFileReceived (LocalFile,
										 ElectraCommand::Object object) {
	return (true);
}


static const unsigned TB_SYSEX_MSG = 0x20;

enum SysExMsgs {
    E1_STRING_TKN,
    E1_START_PUB,
    E1_RACK_END,
    E1_RACK_MSG,
    E1_MODULE_MSG,
    E1_PAGE_MSG,
    E1_PARAM_MSG,
    E1_CHANGED_MSG,
    E1_RESOURCE_MSG,
    E1_SYSEX_MAX
};


std::unordered_map<unsigned,std::string> tokenToString_;

unsigned readTokenId(const SysexBlock& sysexBlock, unsigned& idx) {
	unsigned msb= sysexBlock.peek(idx++);
	unsigned lsb= sysexBlock.peek(idx++);
	unsigned id= (msb << 7) + lsb;
	return id;
}

std::string readToken(const SysexBlock& sysexBlock, unsigned& idx){
	unsigned tkn = readTokenId(sysexBlock,idx);
	auto e= tokenToString_.find(tkn);
	if(e!=tokenToString_.end()) {
		return e->second;
	}
	logMessage("unknown string (%x) ", tkn);
	return "";
}

std::string readString(const SysexBlock& sysexBlock, unsigned& idx){
	std::stringbuf buf;
	bool done=false;
	while(!done) {
		auto ch=sysexBlock.peek(idx++);
		if(ch>0) {
			buf.sputc(ch);
		} else {
			done=true;
		}
	}
	return buf.str();
}

float readFloat(const SysexBlock& sysexBlock, unsigned& idx) {
	float val=0.0f;
	unsigned uval=0;
	for(unsigned i=0;i<5;i++) {
		unsigned bit7=sysexBlock.peek(idx++);
		uval += (bit7 << (i*7));
	}
    val = * static_cast<float *>(static_cast<void *>(&uval));
	// logMessage("readFloat %d %f",uval,  val);
	return val;
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

			unsigned tkn= ::readTokenId(sysexBlock,idx);
			while(idx < len - 1) {
				unsigned char ch = sysexBlock.peek(idx++);
				buf.sputc(ch);
			}
			std::string str= buf.str();
			tokenToString_[tkn]=str;
			// logMessage("New String tkn=%d str=%s", tkn, str.c_str());
			break;

		}
		case E1_START_PUB : {
			tokenToString_.clear();
			unsigned numRacks = readUnsigned(sysexBlock,idx);
			// logMessage("E1_START_PUB %d", numRacks);
            model_->publishStart(src, numRacks);
			break;
		}
		case E1_RACK_END : {
			Kontrol::EntityId rackId= readToken(sysexBlock, idx);
			// logMessage("E1_RACK_END %s", rackId.c_str());
            model_->publishRackFinished(src, rackId);
			break;
		}
		case E1_RACK_MSG : {
			Kontrol::EntityId rackId= readToken(sysexBlock, idx);
			std::string host= readString(sysexBlock,idx);
			unsigned port = readUnsigned(sysexBlock,idx);
			// logMessage("E1_RACK_MSG %s %s %u", rackId.c_str(), host.c_str(),port);
		    model_->createRack(src, rackId, host, port);
			break;
		}
		case E1_MODULE_MSG : {
			Kontrol::EntityId rackId= readToken(sysexBlock, idx);
			Kontrol::EntityId moduleId= readToken(sysexBlock, idx);
			std::string displayName= readString(sysexBlock, idx);
			std::string type= readString(sysexBlock, idx);
			// logMessage("E1_MODULE_MSG %s %s %s %s", rackId.c_str(), moduleId.c_str(),displayName.c_str(),type.c_str());
            model_->createModule(src, rackId, moduleId, displayName, type);
			break;
		}
		case E1_PAGE_MSG : {
			Kontrol::EntityId rackId= readToken(sysexBlock, idx);
			Kontrol::EntityId moduleId= readToken(sysexBlock, idx);
			Kontrol::EntityId pageId= readToken(sysexBlock, idx);
			std::string displayName= readString(sysexBlock, idx);

            std::vector<Kontrol::EntityId> paramIds;
			while(sysexBlock.peek(idx)!=0xF7 && idx < len) {
				std::string val= readString(sysexBlock, idx);
                paramIds.push_back(val);
            }

			// logMessage("E1_PAGE_MSG %s %s %s %s", rackId.c_str(), moduleId.c_str(),pageId.c_str(), displayName.c_str());
			model_->createPage(src, rackId, moduleId, pageId, displayName, paramIds);

			break;
		}
		case E1_PARAM_MSG : {
            std::vector<Kontrol::ParamValue> params;
			Kontrol::EntityId rackId= readToken(sysexBlock, idx);
			Kontrol::EntityId moduleId= readToken(sysexBlock, idx);
			while(sysexBlock.peek(idx)!=0xF7 && idx < len) {
				auto type = (Kontrol::ParamValue::Type) readUnsigned(sysexBlock,idx);
				switch(type) {
					case Kontrol::ParamValue::T_Float : {
						float val= readFloat(sysexBlock, idx);
                        params.push_back(Kontrol::ParamValue(val));
						break;
					}
					case Kontrol::ParamValue::T_String : 
					default: {
						std::string val= readString(sysexBlock, idx);
                        params.push_back(Kontrol::ParamValue(val));
                        break;
					}
				}
			}
			// logMessage("E1_PARAM_MSG %s %s", rackId.c_str(), moduleId.c_str());
            model_->createParam(src, rackId, moduleId, params);

			break;
		}
		case E1_CHANGED_MSG : {
			Kontrol::EntityId rackId= readToken(sysexBlock, idx);
			Kontrol::EntityId moduleId= readToken(sysexBlock, idx);
			Kontrol::EntityId paramId= readToken(sysexBlock, idx);
			if(sysexBlock.peek(idx)!=0xF7 && idx < len) {
				auto type = (Kontrol::ParamValue::Type) readUnsigned(sysexBlock,idx);
				switch(type) {
					case Kontrol::ParamValue::T_Float : {
						float val= readFloat(sysexBlock, idx);
						// logMessage("E1_CHANGED_MSG %s %s %s %f" , rackId.c_str(), moduleId.c_str(),paramId.c_str(), val);
                        model_->changeParam(src, rackId, moduleId, paramId,Kontrol::ParamValue(val));
						break;
					}

					case Kontrol::ParamValue::T_String : 
					default: {
						std::string val= readString(sysexBlock, idx);
						// logMessage("E1_CHANGED_MSG %s %s %s %s", rackId.c_str(), moduleId.c_str(),paramId.c_str(), val.c_str());
                        model_->changeParam(src, rackId, moduleId, paramId,Kontrol::ParamValue(val));
                        break;
					}
				}
			}
			break;
		}
		case E1_RESOURCE_MSG : {
			Kontrol::EntityId rackId= readToken(sysexBlock, idx);
			std::string resType= readToken(sysexBlock, idx);
			std::string resValue= readString(sysexBlock, idx);
			// logMessage("E1_RESOURCE_MSG %s %s %s", rackId.c_str(), resType.c_str(),resValue.c_str());
            model_->createResource(src, rackId, resType, resValue);
			break;
		}

		case E1_SYSEX_MAX:
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
