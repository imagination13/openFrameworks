// copyright (c) openFrameworks team 2010-2017
// copyright (c) damian stewart 2007-2009
#pragma once

#include "ofxOscMessage.h"

/// \class ofxOscBundle
/// \brief an OSC bundle of ofxMessages and/or other ofxOscBundles
class ofxOscBundle{
public:

	ofxOscBundle();
	~ofxOscBundle();
	ofxOscBundle(const ofxOscBundle& other);
	ofxOscBundle& operator=(const ofxOscBundle& other);
	/// for operator= and copy constructor
	ofxOscBundle& copy(const ofxOscBundle& other);
	
	/// clear bundle & message contents
	void clear();

	/// add another bundle to the bundle
	void addBundle(const ofxOscBundle& element);
	
	/// add a message to the bundle
	void addMessage(const ofxOscMessage& message);
	
	/// \return the current bundle count
	int getBundleCount() const;
	
	/// \return the current message count
	int getMessageCount() const;
	
	/// \return the bundle at the given index
	const ofxOscBundle& getBundleAt(int i) const;
	
	/// \return the bundle at the given index
	ofxOscBundle& getBundleAt(int i);
	
	/// \return the message at the given index
	const ofxOscMessage& getMessageAt(int i) const;
	
	/// \return the message at the given index
	ofxOscMessage& getMessageAt(int i);
	
private:
		
	vector<ofxOscMessage> messages; //< bundled messages
	vector<ofxOscBundle> bundles; //< bundled bundles
};
