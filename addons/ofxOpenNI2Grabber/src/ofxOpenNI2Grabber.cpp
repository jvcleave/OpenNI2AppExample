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
	openni::Status status = openni::STATUS_OK;
	
	const char* deviceURI = openni::ANY_DEVICE;
	status = openni::OpenNI::initialize();
	
	ofLog(OF_LOG_VERBOSE, "After initialization:\n%s\n", openni::OpenNI::getExtendedError());
	
	status = device.open(deviceURI);
	if (status == openni::STATUS_OK)
	{
		ofLogVerbose() << "Device open PASS" <<  deviceURI;
		
	}else 
	{
		ofLog(OF_LOG_VERBOSE, "Device open FAIL:\n%s\n", openni::OpenNI::getExtendedError());
		close();
	}
	
	status = depth.create(device, openni::SENSOR_DEPTH);
	
	if (status == openni::STATUS_OK)
	{
		ofLogVerbose() << "Find depth stream PASS";
		
		status = depth.start();
		if (status == openni::STATUS_OK)
		{
			ofLogVerbose() << "Start depth stream PASS";
		}else 
		{
			ofLog(OF_LOG_VERBOSE,"Start depth stream FAIL:\n%s\n", openni::OpenNI::getExtendedError());
			depth.destroy();
		}
		
	}else
	{
		ofLog(OF_LOG_VERBOSE,"Find depth stream FAIL:\n%s\n", openni::OpenNI::getExtendedError());
	}
	
	status = color.create(device, openni::SENSOR_COLOR);
	if (status == openni::STATUS_OK)
	{
		ofLogVerbose() << "Find color stream PASS";
		status = color.start();
		if (status == openni::STATUS_OK)
		{
			ofLogVerbose() << "Start color stream PASS";
		}else 
		{
			ofLog(OF_LOG_VERBOSE,"Start color stream FAIL:\n%s\n", openni::OpenNI::getExtendedError());
			color.destroy();
		}
	}else
	{
		ofLog(OF_LOG_VERBOSE,"Find color stream FAIL:\n%s\n", openni::OpenNI::getExtendedError());
	}
	
	
	if (depth.isValid())
	{
		ofLogVerbose() << "DEPTH streams VALID";
	}else 
	{
		ofLogVerbose() << "DEPTH streams INVALID";
		close();
	}

	if (color.isValid())
	{
		ofLogVerbose() << "COLOR stream VALID";
	}else 
	{
		ofLogVerbose() << "COLOR stream INVALID";
		close();
		
	}
	
}


void ofxOpenNI2Grabber::setup()
{

}


void ofxOpenNI2Grabber::update()
{

}


void ofxOpenNI2Grabber::draw()
{
	
}

void ofxOpenNI2Grabber::close()
{
	openni::OpenNI::shutdown();
}