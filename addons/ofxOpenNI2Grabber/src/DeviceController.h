#pragma once

#include "ofMain.h"
#include "OpenNI.h"
#include "ofxOpenNI2GrabberSettings.h"

using namespace openni;

class DeviceController
{
public:
	DeviceController();
	void setup(ofxOpenNI2GrabberSettings _settings);
	Device device;
	ofxOpenNI2GrabberSettings settings;
	bool isKinect;
	void close();
	void printDeviceInfo();
	void printVideoMode(VideoMode mode);
	void printVideoModes();
	void registerDepthToColor();
	const VideoMode* findMode(SensorType sensorType);

	/*OpenNI::addDeviceStateChangedListener(this);
	OpenNI::addDeviceConnectedListener(this);
	OpenNI::addDeviceDisconnectedListener(this);*/
};