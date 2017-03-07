// copyright (c) openFrameworks team 2010-2017
// copyright (c) damian stewart 2007-2009
#include "ofxOscMessage.h"
#include "ofLog.h"
#include "ofUtils.h"

//--------------------------------------------------------------
ofxOscMessage::ofxOscMessage() : remoteHost(""), remotePort(0) {}

//--------------------------------------------------------------
ofxOscMessage::~ofxOscMessage(){
	clear();
}

//--------------------------------------------------------------
ofxOscMessage::ofxOscMessage(const ofxOscMessage& other){ 
	copy(other);
}

//--------------------------------------------------------------
ofxOscMessage& ofxOscMessage::operator=(const ofxOscMessage& other){ 
	return copy(other);
}

ofxOscMessage& ofxOscMessage::copy(const ofxOscMessage& other){
	if(this == &other) return *this;
	clear();

	// copy address & remote info
	address = other.address;
	remoteHost = other.remoteHost;
	remotePort = other.remotePort;

	// copy arguments
	for(int i = 0; i < (int)other.args.size(); ++i){
		switch(other.getArgType(i)){
			case OFXOSC_TYPE_TRUE: case OFXOSC_TYPE_FALSE:
				args.push_back(new ofxOscArgBool(other.getArgAsBool(i)));
				break;
			case OFXOSC_TYPE_INT32:
				args.push_back(new ofxOscArgInt32(other.getArgAsInt32(i)));
				break;
			case OFXOSC_TYPE_INT64:
				args.push_back(new ofxOscArgInt64(other.getArgAsInt64(i)));
				break;
			case OFXOSC_TYPE_FLOAT:
				args.push_back(new ofxOscArgFloat(other.getArgAsFloat(i)));
				break;
			case OFXOSC_TYPE_DOUBLE:
				args.push_back(new ofxOscArgDouble(other.getArgAsDouble(i)));
				break;
			case OFXOSC_TYPE_STRING:
				args.push_back(new ofxOscArgString(other.getArgAsString(i)));
				break;
			case OFXOSC_TYPE_SYMBOL:
				args.push_back(new ofxOscArgSymbol(other.getArgAsSymbol(i)));
				break;
			case OFXOSC_TYPE_CHAR:
				args.push_back(new ofxOscArgChar(other.getArgAsChar(i)));
				break;
			case OFXOSC_TYPE_MIDI_MESSAGE:
				args.push_back(new ofxOscArgMidiMessage(other.getArgAsMidiMessage(i)));
				break;
			case OFXOSC_TYPE_TRIGGER:
				args.push_back(new ofxOscArgTrigger());
				break;
			case OFXOSC_TYPE_TIMETAG:
				args.push_back(new ofxOscArgTimetag(other.getArgAsTimetag(i)));
				break;
			case OFXOSC_TYPE_BLOB:
				args.push_back(new ofxOscArgBlob(other.getArgAsBlob(i)));
				break;
			case OFXOSC_TYPE_RGBA_COLOR:
				args.push_back(new ofxOscArgRgbaColor(other.getArgAsRgbaColor(i)));
				break;
			default:
				ofLogError("ofxOscMessage") << "copy(): bad argument type "
					<< other.getArgType(i);
				assert(false);
				break;
		}
	}
	
	return *this;
}

//--------------------------------------------------------------
void ofxOscMessage::clear(){
	address = "";
	remoteHost = "";
	remotePort = 0;
	for(unsigned int i = 0; i < args.size(); ++i){
		delete args[i];
	}
	args.clear();
}

//--------------------------------------------------------------
void ofxOscMessage::setAddress(const string &_address){
	address = _address;
}

//--------------------------------------------------------------
string ofxOscMessage::getAddress() const{ 
	return address;
}

//--------------------------------------------------------------
string ofxOscMessage::getRemoteIp() const{ 
	return remoteHost;
}

//--------------------------------------------------------------
string ofxOscMessage::getRemoteHost() const{
	return remoteHost;
}

//--------------------------------------------------------------
int ofxOscMessage::getRemotePort() const{
	return remotePort;
}

// get methods
//--------------------------------------------------------------
int ofxOscMessage::getNumArgs() const{
	return (int)args.size();
}

//--------------------------------------------------------------
ofxOscArgType ofxOscMessage::getArgType(int index) const{
	if(index >= (int)args.size()) {
		ofLogError("ofxOscMessage") << "getArgType(): index "
		                            << index << " out of bounds";
		return OFXOSC_TYPE_INDEXOUTOFBOUNDS;
	}
	else{
		return args[index]->getType();
	}
}

//--------------------------------------------------------------
string ofxOscMessage::getArgTypeName(int index) const{
	if(index >= (int)args.size()) {
		ofLogError("ofxOscMessage") << "getArgTypeName(): index "
		                            << index << " out of bounds";
		return "INDEX OUT OF BOUNDS";
	}
	else{
		return args[index]->getTypeName();
	}
}

//--------------------------------------------------------------
int32_t ofxOscMessage::getArgAsInt(int index) const{
	return getArgAsInt32(index);
}

//--------------------------------------------------------------
int32_t ofxOscMessage::getArgAsInt32(int index) const{
	if(getArgType(index) != OFXOSC_TYPE_INT32){
		if(getArgType(index) == OFXOSC_TYPE_INT64){
			// warn about possible lack of precision
			ofLogWarning("ofxOscMessage")
				<< "getArgAsInt32(): converting int64 to int32 for argument "
				<< index;
			return (int32_t)((ofxOscArgInt64*)args[index])->get();
		}
		else if (getArgType(index) == OFXOSC_TYPE_FLOAT){
			return (int32_t)((ofxOscArgFloat*)args[index])->get();
		}
		else if (getArgType(index) == OFXOSC_TYPE_DOUBLE){
			// warn about possible lack of precision
			ofLogWarning("ofxOscMessage")
				<< "getArgAsInt32(): converting double to int32 for argument "
				<< index;
			return (int32_t)((ofxOscArgDouble*)args[index])->get();
		}
		else if(getArgType(index)  == OFXOSC_TYPE_TRUE || 
			    getArgType(index) == OFXOSC_TYPE_FALSE){
			return (int32_t)((ofxOscArgBool*)args[index])->get();
		}
		else{
			ofLogError("ofxOscMessage") << "getArgAsInt32(): argument "
			                            << index << " is not a number";
			return 0;
		}
	}
	else{
		return ((ofxOscArgInt32*)args[index])->get();
	}
}

//--------------------------------------------------------------
int64_t ofxOscMessage::getArgAsInt64(int index) const{
	if(getArgType(index) != OFXOSC_TYPE_INT64){
		if(getArgType(index) == OFXOSC_TYPE_INT32){
			return (int64_t)((ofxOscArgInt32*)args[index])->get();
		}
		else if(getArgType(index) == OFXOSC_TYPE_FLOAT){
			return (int64_t)((ofxOscArgFloat*)args[index])->get();
		}
		else if(getArgType(index) == OFXOSC_TYPE_DOUBLE){
			return (int64_t)((ofxOscArgDouble*)args[index])->get();
		}
		else if(getArgType(index)  == OFXOSC_TYPE_TRUE ||
			    getArgType(index) == OFXOSC_TYPE_FALSE){
			return (int64_t)((ofxOscArgBool*)args[index])->get();
		}
		else{
			ofLogError("ofxOscMessage") << "getArgAsInt64(): argument "
			                            << index << " is not a number";
			return 0;
		}
	}
	else{
		return ((ofxOscArgInt64*)args[index])->get();
	}
}

//--------------------------------------------------------------
float ofxOscMessage::getArgAsFloat(int index) const{
	if(getArgType(index) != OFXOSC_TYPE_FLOAT){
		if(getArgType(index) == OFXOSC_TYPE_INT32){
			return (float)((ofxOscArgInt32*)args[index])->get();
		}
		else if(getArgType(index) == OFXOSC_TYPE_INT64){
			// warn about possible lack of precision
			ofLogWarning("ofxOscMessage")
				<< "getArgAsFloat(): converting int64 to float for argument "
				<< index;
			return (float)((ofxOscArgInt64*)args[index])->get();
		}
		else if(getArgType(index) == OFXOSC_TYPE_DOUBLE){
			// warn about possible lack of precision
			ofLogWarning("ofxOscMessage")
				<< "getArgAsFloat(): converting double to float for argument "
				<< index;
			return (float)((ofxOscArgDouble*)args[index])->get();
		}
		else if(getArgType(index)  == OFXOSC_TYPE_TRUE ||
			    getArgType(index) == OFXOSC_TYPE_FALSE){
			return (float)((ofxOscArgBool*)args[index])->get();
		}
		else{
			ofLogError("ofxOscMessage") << "getArgAsFloat(): argument "
			                            << index << " is not a number";
			return 0;
		}
	}
	else{
		return ((ofxOscArgFloat*)args[index])->get();
	}
}

//--------------------------------------------------------------
double ofxOscMessage::getArgAsDouble(int index) const{
	if(getArgType(index) != OFXOSC_TYPE_DOUBLE){
		if(getArgType(index) == OFXOSC_TYPE_INT32){
			return (double)((ofxOscArgInt32*)args[index])->get();
		}
		else if(getArgType(index) == OFXOSC_TYPE_INT64){
			return (double)((ofxOscArgInt64*)args[index])->get();
		}
		else if(getArgType(index) == OFXOSC_TYPE_FLOAT){
			return (double)((ofxOscArgFloat*)args[index])->get();
		}
		else if( getArgType(index)  == OFXOSC_TYPE_TRUE ||
			     getArgType(index) == OFXOSC_TYPE_FALSE){
			return (double)((ofxOscArgBool*)args[index])->get();
		}
		else{
			ofLogError("ofxOscMessage") << "getArgAsDouble(): argument "
			                            << index << " is not a number";
			return 0;
		}
	}
	else{
		return ((ofxOscArgDouble*)args[index])->get();
	}
}

//--------------------------------------------------------------
string ofxOscMessage::getArgAsString(int index) const{
	if(getArgType(index) != OFXOSC_TYPE_STRING){
		// warn about string conversions
		if(getArgType(index) == OFXOSC_TYPE_INT32){
			ofLogWarning("ofxOscMessage")
				<< "getArgAsString(): converting int32 to string for argument "
				<< index;
			return ofToString(((ofxOscArgInt32*)args[index])->get());
		}
		else if(getArgType(index) == OFXOSC_TYPE_INT64){
			ofLogWarning("ofxOscMessage")
				<< "getArgAsString(): converting int64 to string for argument "
				<< index;
			return ofToString(((ofxOscArgInt64*)args[index])->get());
		}
		else if(getArgType(index) == OFXOSC_TYPE_FLOAT){
			ofLogWarning("ofxOscMessage")
				<< "getArgAsString(): converting float to string for argument "
				<< index;
			return ofToString(((ofxOscArgFloat*)args[index])->get());
		}
		else if(getArgType(index) == OFXOSC_TYPE_DOUBLE){
			ofLogWarning("ofxOscMessage")
				<< "getArgAsString(): converting double to string for argument "
				<< index;
			return ofToString(((ofxOscArgDouble*)args[index])->get());
		}
		else if(getArgType(index) == OFXOSC_TYPE_SYMBOL){
			return ((ofxOscArgSymbol*)args[index])->get();
		}
		else if(getArgType(index) == OFXOSC_TYPE_CHAR){
			ofLogWarning("ofxOscMessage")
				<< "getArgAsString(): converting char to string for argument "
				<< index;
			return ofToString(((ofxOscArgChar*)args[index])->get());
		}
		else{
			ofLogError("ofxOscMessage")
				<< "getArgAsString(): argument " << index
				<< " is not a string interpretable value";
			return "";
		}
	}
	else{
		return ((ofxOscArgString*)args[index])->get();
	}
}

//--------------------------------------------------------------
string ofxOscMessage::getArgAsSymbol(int index) const{
	if(getArgType(index) != OFXOSC_TYPE_SYMBOL){
		// warn about string conversions
		if(getArgType(index) == OFXOSC_TYPE_INT32){
			ofLogWarning("ofxOscMessage")
				<< "getArgAsSymbol(): converting int32 to symbol (string) "
				<< "for argument " << index;
			return ofToString(((ofxOscArgInt32*)args[index])->get());
		}
		else if(getArgType(index) == OFXOSC_TYPE_INT64){
			ofLogWarning("ofxOscMessage")
				<< "getArgAsSymbol(): converting int64 to symbol (string) "
				<< "for argument " << index;
			return ofToString(((ofxOscArgInt64*)args[index])->get());
		}
		else if(getArgType(index) == OFXOSC_TYPE_FLOAT){
			ofLogWarning("ofxOscMessage")
				<< "getArgAsSymbol(): converting float to symbol (string) "
				<< "for argument " << index;
			return ofToString(((ofxOscArgFloat*)args[index])->get());
		}
		else if(getArgType(index) == OFXOSC_TYPE_DOUBLE){
			ofLogWarning("ofxOscMessage")
				<< "getArgAsSymbol(): converting double to symbol (string) "
				<< "for argument " << index;
			return ofToString(((ofxOscArgDouble*)args[index])->get());
		}
		else if(getArgType(index) == OFXOSC_TYPE_STRING){
			return ((ofxOscArgString*)args[index])->get();
		}
		else if(getArgType(index) == OFXOSC_TYPE_CHAR){
			ofLogWarning("ofxOscMessage")
				<< "getArgAsSymbol(): converting char to symbol (string) "
				<< "for argument " << index;
			return ofToString(((ofxOscArgChar*)args[index])->get());
		}
		else{
			ofLogError("ofxOscMessage") << "getArgAsSymbol(): argument "
				<< index << " is not a symbol (string) interpretable value";
			return "";
		}
	}
	else{
		return ((ofxOscArgSymbol*)args[index])->get();
	}
}

//--------------------------------------------------------------
char ofxOscMessage::getArgAsChar(int index) const{
	if(getArgType(index) == OFXOSC_TYPE_CHAR){
		return ((ofxOscArgChar*)args[index])->get();
	}
	else{
		ofLogError("ofxOscMessage") << "getArgAsChar(): argument "
			<< index << " is not a char";
		return 0;
	}
}

//--------------------------------------------------------------
int32_t ofxOscMessage::getArgAsMidiMessage(int index) const{
	if(getArgType(index) == OFXOSC_TYPE_MIDI_MESSAGE){
		return ((ofxOscArgMidiMessage*)args[index])->get();
	}
	else{
		ofLogError("ofxOscMessage") << "getArgAsMidiMessage(): argument "
			<< index << " is not a midi message";
		return 0;
	}
}

//--------------------------------------------------------------
bool ofxOscMessage::getArgAsBool(int index) const{
	switch(getArgType(index)){
		case OFXOSC_TYPE_TRUE: case OFXOSC_TYPE_FALSE:
			return ((ofxOscArgBool*)args[index])->get();
		case OFXOSC_TYPE_INT32:
			return ((ofxOscArgInt32*)args[index])->get() > 0;
		case OFXOSC_TYPE_INT64:
			return ((ofxOscArgInt64*)args[index])->get() > 0;
		case OFXOSC_TYPE_FLOAT:
			return ((ofxOscArgFloat*)args[index])->get() > 0;
		case OFXOSC_TYPE_DOUBLE:
			return ((ofxOscArgDouble*)args[index])->get() > 0;
		case OFXOSC_TYPE_STRING: case OFXOSC_TYPE_SYMBOL:
			return ((ofxOscArgString*)args[index])->get() == "true";
		default:
			ofLogError("ofxOscMessage") << "getArgAsBool(): argument "
				<< index << " is not a boolean interpretable value";
			return false;
	}
}

//--------------------------------------------------------------
bool ofxOscMessage::getArgAsTrigger(int index) const{
	if(getArgType(index) != OFXOSC_TYPE_TRIGGER){
		ofLogError("ofxOscMessage") << "getArgAsTrigger(): argument "
			<< index << " is not a trigger";
		return false;
	}
	else{
		return ((ofxOscArgTrigger*)args[index])->get();
	}
}

//--------------------------------------------------------------
bool ofxOscMessage::getArgAsImpulse(int index) const{
	return getArgAsTrigger(index);
}

//--------------------------------------------------------------
bool ofxOscMessage::getArgAsInfinitum(int index) const{
	return getArgAsTrigger(index);
}

//--------------------------------------------------------------
int64_t ofxOscMessage::getArgAsTimetag(int index) const{
	if(getArgType(index) != OFXOSC_TYPE_TIMETAG){
		if(getArgType(index) == OFXOSC_TYPE_DOUBLE){
			// warn about possible conversion issue
			ofLogWarning("ofxOscMessage")
				<< "getArgAsTimetag(): converting double to Timetag "
				<< "for argument " << index;
			return (std::int64_t)((ofxOscArgDouble*)args[index])->get();
		}
		else{
			ofLogError("ofxOscMessage") << "getArgAsTimetag(): argument "
				<< index << " is not a valid number";
			return 0;
		}
	}
	else{
		return ((ofxOscArgTimetag*)args[index])->get();
	}
}

//--------------------------------------------------------------
ofBuffer ofxOscMessage::getArgAsBlob(int index) const{
	if(getArgType(index) != OFXOSC_TYPE_BLOB){
		ofLogError("ofxOscMessage") << "getArgAsBlob(): argument "
			<< index << " is not a blob";
		return ofBuffer();
	}
	else{
		return ((ofxOscArgBlob*)args[index])->get();
	}
}

//--------------------------------------------------------------
int32_t ofxOscMessage::getArgAsRgbaColor(int index) const{
	if(getArgType(index) != OFXOSC_TYPE_RGBA_COLOR){
		if(getArgType(index) == OFXOSC_TYPE_INT32){
			return ((ofxOscArgInt32*)args[index])->get();
		}
		else{
			ofLogError("ofxOscMessage") << "getArgAsRgbaColor(): argument "
				<< index << " is not an rgba color";
			return 0;
		}
	}
	else{
		return ((ofxOscArgRgbaColor*)args[index])->get();
	}
}

// set methods
//--------------------------------------------------------------
void ofxOscMessage::addIntArg(int32_t argument){
	args.push_back(new ofxOscArgInt32(argument));
}

//--------------------------------------------------------------
void ofxOscMessage::addInt32Arg(int32_t argument){
	args.push_back(new ofxOscArgInt32(argument));
}

//--------------------------------------------------------------
void ofxOscMessage::addInt64Arg(int64_t argument){
	args.push_back(new ofxOscArgInt64(argument));
}

//--------------------------------------------------------------
void ofxOscMessage::addFloatArg(float argument){
	args.push_back(new ofxOscArgFloat(argument));
}

//--------------------------------------------------------------
void ofxOscMessage::addDoubleArg(double argument){
	args.push_back(new ofxOscArgDouble(argument));
}

//--------------------------------------------------------------
void ofxOscMessage::addStringArg(const std::string &argument){
	args.push_back(new ofxOscArgString(argument));
}

//--------------------------------------------------------------
void ofxOscMessage::addSymbolArg(const std::string &argument){
	args.push_back(new ofxOscArgSymbol(argument));
}

//--------------------------------------------------------------
void ofxOscMessage::addCharArg( char argument){
	args.push_back(new ofxOscArgChar(argument));
}

//--------------------------------------------------------------
void ofxOscMessage::addMidiMessageArg(int32_t argument){
	args.push_back(new ofxOscArgMidiMessage(argument));
}

//--------------------------------------------------------------
void ofxOscMessage::addBoolArg(bool argument){
	args.push_back(new ofxOscArgBool(argument));
}

//--------------------------------------------------------------
void ofxOscMessage::addTriggerArg(){
	args.push_back(new ofxOscArgTrigger());
}

//--------------------------------------------------------------
void ofxOscMessage::addImpulseArg(){
	args.push_back(new ofxOscArgTrigger());
}

//--------------------------------------------------------------
void ofxOscMessage::addInfinitumArg(){
	args.push_back(new ofxOscArgTrigger());
}

//--------------------------------------------------------------
void ofxOscMessage::addTimetagArg(int64_t argument){
	args.push_back(new ofxOscArgTimetag(argument));
}

//--------------------------------------------------------------
void ofxOscMessage::addBlobArg(const ofBuffer &argument){
	args.push_back(new ofxOscArgBlob(argument));
}

//--------------------------------------------------------------
void ofxOscMessage::addRgbaColorArg(int32_t argument){
	args.push_back(new ofxOscArgRgbaColor(argument));
}

// util
//--------------------------------------------------------------
void ofxOscMessage::setRemoteEndpoint(const string &host, int port){
	remoteHost = host;
	remotePort = port;
}
