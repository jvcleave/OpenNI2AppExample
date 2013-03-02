/*
 *  ofxOpenNI2Grabber.cpp
 *  OpenNI2AppExample
 *
 *  Created by jason van cleave on 2/28/13.
 *  Copyright 2013 jasonvancleave.com. All rights reserved.
 *
 */

#include "ofxOpenNI2Grabber.h"


ofxOpenNI2Grabber::ofxOpenNI2Grabber()
{
	isReady = false;	
}

bool ofxOpenNI2Grabber::setup(ofxOpenNI2GrabberSettings settings_)
{
	settings = settings_;
	
	Status status = STATUS_OK;

	status = OpenNI::initialize();
	if (status == STATUS_OK)
	{
		ofLogVerbose() << "initialize PASS";
		deviceController.setup(settings);
	}else 
	{
		ofLog(OF_LOG_ERROR, "initialize FAIL:\n%s\n", OpenNI::getExtendedError());
	}
	
	

	if (status == STATUS_OK)
	{
		ofLogVerbose() << "Device open PASS";
		
	}else 
	{
		ofLogError() << "Device open FAIL: " << OpenNI::getExtendedError();
		deviceController.close();
		return false;
	}
	
	if (settings.doDepth) 
	{
		
		depthSource.setup(deviceController.device);
		streams.push_back(&depthSource.videoStream);
		if(!settings.useOniFile && !deviceController.isKinect)
		{
			const VideoMode* requestedMode = deviceController.findMode(SENSOR_DEPTH); 
			if (requestedMode != NULL) 
			{
				depthSource.videoStream.setVideoMode(*requestedMode);
			}
		}
		
	}
	if (settings.doColor) 
	{
		rgbSource.setup(deviceController.device);
		streams.push_back(&rgbSource.videoStream);
		if(!settings.useOniFile && !deviceController.isKinect)
		{
			const VideoMode* requestedMode = deviceController.findMode(SENSOR_COLOR); 
			if (requestedMode != NULL) 
			{
				rgbSource.videoStream.setVideoMode(*requestedMode);
			}
		}
	}
	if(settings.doRegisterDepthToColor)
	{
		deviceController.registerDepthToColor();
	}
	
	isReady = true;
	return isReady;
}

void ofxOpenNI2Grabber::threadedFunction()
{
	while (isThreadRunning()) 
	{
		int changedIndex;
		Status status = OpenNI::waitForAnyStream(&streams[0], streams.size(), &changedIndex);
		if (status != STATUS_OK)
		{
			ofLogError() << "waitForAnyStream FAIL:" << OpenNI::getExtendedError();
		}
	}
}

void ofxOpenNI2Grabber::draw()
{
	if (settings.doColor) rgbSource.draw();
	if (settings.doColor) depthSource.draw();
}


bool ofxOpenNI2Grabber::close()
{
	isReady = false;
	stopThread();
	
	depthSource.close();
	rgbSource.close();
	deviceController.close();
	
	OpenNI::shutdown();
	return isReady;
	//
}



