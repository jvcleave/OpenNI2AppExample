/*
 *  ofxOpenNI2Grabber.cpp
 *  OpenNI2AppExample
 *
 *  Created by jason van cleave on 2/28/13.
 *  Copyright 2013 jasonvancleave.com. All rights reserved.
 *
 */

#include "ofxOpenNI2Grabber.h"

ofxOpenNI2Grabber::Settings::Settings() {
	width = 640;
	height = 480;
	fps = 30;
	doDepth = true;
	doColor = true;
	depthPixelFormat = openni::PIXEL_FORMAT_DEPTH_100_UM;
	colorPixelFormat = openni::PIXEL_FORMAT_RGB888;
	isKinect = false;
	
}
const openni::VideoMode* ofxOpenNI2Grabber::findMode(openni::Device& device, openni::SensorType sensorType)
{
	const openni::VideoMode* mode = NULL;
	const openni::SensorInfo* sensorInfo = device.getSensorInfo(sensorType);
	const openni::Array<openni::VideoMode>& videoModes = sensorInfo->getSupportedVideoModes();
	for (int i=0; i<videoModes.getSize(); i++) 
	{
		const openni::VideoMode& currentMode  = videoModes[i];
		switch (sensorType) 
		{
			case openni::SENSOR_DEPTH:
			{
				if(
				   currentMode.getPixelFormat() == settings.depthPixelFormat
				   && currentMode.getResolutionX() == settings.width
				   && currentMode.getResolutionY() == settings.height
				   && currentMode.getFps() == settings.fps
				   ){
					mode = &currentMode;
					ofxOpenNI2GrabberUtils::printMode(*mode);
					return mode;
				}
				break;
				
			}
			case openni::SENSOR_COLOR:
			{
				if(
				   currentMode.getPixelFormat() == settings.colorPixelFormat
				   && currentMode.getResolutionX() == settings.width
				   && currentMode.getResolutionY() == settings.height
				   && currentMode.getFps() == settings.fps
				   ){
					mode = &currentMode;
					ofxOpenNI2GrabberUtils::printMode(*mode);
					return mode;
				}
				break;
			}
		}
	}
	return mode;
	
}

ofxOpenNI2Grabber::ofxOpenNI2Grabber()
{
	isReady = false;	
}
bool ofxOpenNI2Grabber::setup(Settings settings_)
{
	settings = settings_;
	
	openni::Status status = openni::STATUS_OK;
	
	deviceURI = openni::ANY_DEVICE;
	status = openni::OpenNI::initialize();
	if (status == openni::STATUS_OK)
	{
		ofLogVerbose() << "OpenNI::initialize PASS";
	}else 
	{
		ofLog(OF_LOG_ERROR, "OpenNI::initialize FAIL:\n%s\n", openni::OpenNI::getExtendedError());
	}
	
	status = device.open(deviceURI);

	if (status == openni::STATUS_OK)
	{
		ofLogVerbose() << "Device open PASS" <<  deviceURI;
		ofxOpenNI2GrabberUtils::printDeviceInfo(device);
	}else 
	{
		ofLog(OF_LOG_VERBOSE, "Device open FAIL:\n%s\n", openni::OpenNI::getExtendedError());
		close();
	}
	
	ofxOpenNI2GrabberUtils::printModes(device);

	
	status = depth.create(device, openni::SENSOR_DEPTH);
	const openni::VideoMode* requestedMode = findMode(device, openni::SENSOR_DEPTH); 
	if (requestedMode != NULL) 
	{
		depth.setVideoMode(*requestedMode);
	}
		
	
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
	const openni::VideoMode* requestedColorMode = findMode(device, openni::SENSOR_COLOR); 
	if (requestedColorMode != NULL) 
	{
		color.setVideoMode(*requestedColorMode);
	}
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
		allocateDepthBuffers();
		allocateDepthRawBuffers();
	}else 
	{
		ofLogVerbose() << "DEPTH streams INVALID";
		return close();
	}
	
	if (color.isValid())
	{
		allocateColorBuffers();
		ofLogVerbose() << "COLOR stream VALID";
	}else 
	{
		ofLogVerbose() << "COLOR stream INVALID";
		return close();
	}
	
	streams = new openni::VideoStream*[2];
	streams[0] = &depth;
	streams[1] = &color;
	
	isReady = true;
	return isReady;
}
void ofxOpenNI2Grabber::allocateDepthBuffers()
{
	depthVideoMode = depth.getVideoMode();
	depthWidth = depthVideoMode.getResolutionX();
	depthHeight = depthVideoMode.getResolutionY();
	deviceMaxDepth	= depth.getMaxPixelValue();
	
	ofLogVerbose() << "deviceMaxDepth: " << deviceMaxDepth;
	
	depthPixels[0].allocate(depthWidth, depthHeight, OF_IMAGE_COLOR_ALPHA);
	depthPixels[1].allocate(depthWidth, depthHeight, OF_IMAGE_COLOR_ALPHA);
	currentDepthPixels = &depthPixels[0];
	backDepthPixels = &depthPixels[1];
	
	depthTexture.allocate(depthWidth, depthHeight, GL_RGBA);
}

void ofxOpenNI2Grabber::allocateDepthRawBuffers()
{
	depthRawPixels[0].allocate(depthWidth, depthHeight, OF_PIXELS_MONO);
	depthRawPixels[1].allocate(depthWidth, depthHeight, OF_PIXELS_MONO);
	currentDepthRawPixels = &depthRawPixels[0];
	backDepthRawPixels = &depthRawPixels[1];
}

void ofxOpenNI2Grabber::allocateColorBuffers()
{
	colorVideoMode = color.getVideoMode();
	colorWidth = colorVideoMode.getResolutionX();
	colorHeight = colorVideoMode.getResolutionY();
	
	rgbPixels[0].allocate(colorWidth, colorHeight, OF_IMAGE_COLOR);
	rgbPixels[1].allocate(colorWidth, colorHeight, OF_IMAGE_COLOR);
	currentRGBPixels = &rgbPixels[0];
	backRGBPixels = &rgbPixels[1];
	
	rgbTexture.allocate(colorWidth, colorHeight, GL_RGB);
}

void ofxOpenNI2Grabber::threadedFunction()
{
	while (isThreadRunning()) 
	{
		readFrame();
	}
}

void ofxOpenNI2Grabber::readFrame()
{
	int changedIndex;
	openni::Status rc = openni::OpenNI::waitForAnyStream(streams, 2, &changedIndex);
	if (rc != openni::STATUS_OK)
	{
		ofLogError() << "Wait failed";
		return;
	}
	lock();
	switch (changedIndex)
	{
		case 0:
		{
			generateDepthPixels();
			break;
		}
		case 1:
		{
			color.readFrame(&colorFrame);
			backRGBPixels->setFromPixels((unsigned char *)colorFrame.getData(), colorWidth, colorHeight, OF_IMAGE_COLOR);
			swap(backRGBPixels,currentRGBPixels);
			break;
		}
		default:
		{
			ofLogError() << "Error in wait";
		}
	}
	unlock();
}


void ofxOpenNI2Grabber::draw()
{
	//lock();
	depthTexture.loadData(*currentDepthPixels);
	rgbTexture.loadData(*currentRGBPixels);
	
	rgbTexture.draw(0, 0);
	depthTexture.draw(settings.width, 0);

}
void ofxOpenNI2Grabber::generateDepthPixels()
{
	depth.readFrame(&depthFrame);
	const openni::DepthPixel* oniDepthPixels = (const openni::DepthPixel*)depthFrame.getData();
	
	//backDepthRawPixels->setFromPixels(oniDepthPixels, depthWidth, depthHeight, 1);
	
	
	float max = 255;
	for (unsigned short y = 0; y < depthHeight; y++) 
	{
		unsigned char * texture = backDepthPixels->getPixels() + y * depthWidth * 4;
		for (unsigned short  x = 0; x < depthWidth; x++, oniDepthPixels++, texture += 4)
		{
			unsigned char red = 0;
			unsigned char green = 0;
			unsigned char blue = 0;
			unsigned char alpha = 255;
			
			unsigned short col_index;
			
			unsigned char a = (unsigned char)(((*oniDepthPixels) / ( deviceMaxDepth / max)));
			red		= a;
			green	= a;
			blue	= a;
			
			texture[0] = red;
			texture[1] = green;
			texture[2] = blue;
			
			if (*oniDepthPixels == 0)
			{
				texture[3] = 0;
			} else	
			{
				texture[3] = alpha;
			}
		}
	}
	swap(backDepthPixels,currentDepthPixels);
	
}
bool ofxOpenNI2Grabber::close()
{
	isReady = false;
	stopThread();
	
	depth.stop();
	color.stop();
	depth.destroy();
	color.destroy();
	device.close();
	
	openni::OpenNI::shutdown();
	return isReady;
	//
}