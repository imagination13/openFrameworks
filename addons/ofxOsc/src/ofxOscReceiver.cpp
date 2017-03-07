// copyright (c) openFrameworks team 2010-2017
// copyright (c) damian stewart 2007-2009
#include "ofxOscReceiver.h"

//--------------------------------------------------------------
ofxOscReceiver::ofxOscReceiver() : allowReuse(true), port(0) {}

//--------------------------------------------------------------
ofxOscReceiver::ofxOscReceiver(const ofxOscReceiver & mom){
	copy(mom);
}

//--------------------------------------------------------------
ofxOscReceiver& ofxOscReceiver::operator=(const ofxOscReceiver & mom){
	return copy(mom);
}

//--------------------------------------------------------------
ofxOscReceiver& ofxOscReceiver::copy(const ofxOscReceiver& other){
	if(this == &other) return *this;
	allowReuse = other.allowReuse;
	port = other.port;
	if(other.listenSocket){
		setup(port);
	}
	return *this;
}

//--------------------------------------------------------------
bool ofxOscReceiver::setup(int port){
	if(osc::UdpSocket::GetUdpBufferSize() == 0){
	   osc::UdpSocket::SetUdpBufferSize(65535);
	}
	
	// if we're already running, shutdown before running again
	if(listenSocket){
		clear();
	}
	
	// create socket
	osc::UdpListeningReceiveSocket *socket = nullptr;
	try{
		socket = new osc::UdpListeningReceiveSocket(osc::IpEndpointName(osc::IpEndpointName::ANY_ADDRESS, port), this, allowReuse);
		auto deleter = [](osc::UdpListeningReceiveSocket*socket){
			// tell the socket to shutdown
			socket->Break();
			delete socket;
		};
		auto newPtr = std::unique_ptr<osc::UdpListeningReceiveSocket, decltype(deleter)>(socket, deleter);
		listenSocket = std::move(newPtr);
	}
	catch(std::exception &e){
		ofLogError("ofxOscReceiver") << "couldn't create receive socket on port: " << port << " " << e.what();
		if(socket != nullptr){
			delete socket;
			socket = nullptr;
		}
		return false;
	}

	listenThread = std::thread([this]{
		while(listenSocket){
			try{
				listenSocket->Run();
			}
			catch(std::exception &e){
				ofLogWarning() << e.what();
			}
		}
	});

	// detach thread so we don't have to wait on it before creating a new socket
	// or on destruction, the custom deleter for the socket unique_ptr already
	// does the right thing
	listenThread.detach();

	this->port = port;
	
	return true;
}

//--------------------------------------------------------------
void ofxOscReceiver::clear() {
	listenSocket.reset();
	port = 0;
}

//--------------------------------------------------------------
bool ofxOscReceiver::hasWaitingMessages(){
	return !messagesChannel.empty();
}

//--------------------------------------------------------------
bool ofxOscReceiver::getNextMessage(ofxOscMessage * message){
	return getNextMessage(*message);
}

//--------------------------------------------------------------
bool ofxOscReceiver::getNextMessage(ofxOscMessage & message){
	return messagesChannel.tryReceive(message);
}

//--------------------------------------------------------------
bool ofxOscReceiver::getParameter(ofAbstractParameter & parameter){
	ofxOscMessage msg;
	while(messagesChannel.tryReceive(msg)){
		ofAbstractParameter * p = &parameter;
		vector<string> address = ofSplitString(msg.getAddress(),"/", true);
		for(unsigned int i = 0; i < address.size(); i++){
			if(p){
				if(address[i] == p->getEscapedName()){
					if(p->type() == typeid(ofParameterGroup).name()){
						if(address.size() >= i+1){
							ofParameterGroup* g = static_cast<ofParameterGroup*>(p);
							if(g->contains(address[i+1])){
								p = &g->get(address[i+1]);
							}
							else{
								p = nullptr;
							}
						}
					}
					else if(p->type() == typeid(ofParameter<int>).name() &&
						msg.getArgType(0) == OFXOSC_TYPE_INT32){
						p->cast<int>() = msg.getArgAsInt32(0);
					}
					else if(p->type() == typeid(ofParameter<float>).name() &&
						msg.getArgType(0) == OFXOSC_TYPE_FLOAT){
						p->cast<float>() = msg.getArgAsFloat(0);
					}
					else if(p->type() == typeid(ofParameter<double>).name() &&
						msg.getArgType(0) == OFXOSC_TYPE_DOUBLE){
						p->cast<double>() = msg.getArgAsDouble(0);
					}
					else if(p->type() == typeid(ofParameter<bool>).name() &&
						(msg.getArgType(0) == OFXOSC_TYPE_TRUE ||
						 msg.getArgType(0) == OFXOSC_TYPE_FALSE ||
						 msg.getArgType(0) == OFXOSC_TYPE_INT32 ||
						 msg.getArgType(0) == OFXOSC_TYPE_INT64 ||
						 msg.getArgType(0) == OFXOSC_TYPE_FLOAT ||
						 msg.getArgType(0) == OFXOSC_TYPE_DOUBLE ||
						 msg.getArgType(0) == OFXOSC_TYPE_STRING ||
						 msg.getArgType(0) == OFXOSC_TYPE_SYMBOL)){
						p->cast<bool>() = msg.getArgAsBool(0);
					}
					else if(msg.getArgType(0) == OFXOSC_TYPE_STRING){
						p->fromString(msg.getArgAsString(0));
					}
				}
			}
		}
	}
	return true;
}

//--------------------------------------------------------------
int ofxOscReceiver::getPort(){
	return port;
}

//--------------------------------------------------------------
void ofxOscReceiver::disableReuse(){
	allowReuse = false;
	if(listenSocket){
		setup(port);
	}
}

//--------------------------------------------------------------
void ofxOscReceiver::enableReuse(){
	allowReuse = true;
	if(listenSocket){
		setup(port);
	}
}

// PROTECTED
//--------------------------------------------------------------
void ofxOscReceiver::ProcessMessage(const osc::ReceivedMessage &m, const osc::IpEndpointName& remoteEndpoint){
	// convert the message to an ofxOscMessage
	ofxOscMessage msg;

	// set the address
	msg.setAddress(m.AddressPattern());
	
	// set the sender ip/host
	char endpointHost[osc::IpEndpointName::ADDRESS_STRING_LENGTH];
	remoteEndpoint.AddressAsString(endpointHost);
	msg.setRemoteEndpoint(endpointHost, remoteEndpoint.port);

	// transfer the arguments
	for(osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin(); arg != m.ArgumentsEnd(); ++arg){
		if(arg->IsInt32()){
			msg.addIntArg(arg->AsInt32Unchecked());
		}
		else if(arg->IsInt64()){
			msg.addInt64Arg(arg->AsInt64Unchecked());
		}
		else if( arg->IsFloat()){
			msg.addFloatArg(arg->AsFloatUnchecked());
		}
		else if(arg->IsDouble()){
			msg.addDoubleArg(arg->AsDoubleUnchecked());
		}
		else if(arg->IsString()){
			msg.addStringArg(arg->AsStringUnchecked());
		}
		else if(arg->IsSymbol()){
			msg.addSymbolArg(arg->AsSymbolUnchecked());
		}
		else if(arg->IsChar()){
			msg.addCharArg(arg->AsCharUnchecked());
		}
		else if(arg->IsMidiMessage()){
			msg.addMidiMessageArg(arg->AsMidiMessageUnchecked());
		}
		else if(arg->IsBool()){
			msg.addBoolArg(arg->AsBoolUnchecked());
		}
		else if(arg->IsInfinitum()){
			msg.addTriggerArg();
		}
		else if(arg->IsTimeTag()){
			msg.addTimetagArg(arg->AsTimeTagUnchecked());
		}
		else if(arg->IsRgbaColor()){
			msg.addRgbaColorArg(arg->AsRgbaColorUnchecked());
		}
		else if(arg->IsBlob()){
			const char * dataPtr;
			osc::osc_bundle_element_size_t len = 0;
			arg->AsBlobUnchecked((const void*&)dataPtr, len);
			ofBuffer buffer(dataPtr, len);
			msg.addBlobArg(buffer);
		}
		else {
			ofLogError("ofxOscReceiver") << "ProcessMessage: argument in message "
				<< m.AddressPattern() << " is an unknown type";
		}
	}

	// send msg to main thread
	messagesChannel.send(std::move(msg));
}
