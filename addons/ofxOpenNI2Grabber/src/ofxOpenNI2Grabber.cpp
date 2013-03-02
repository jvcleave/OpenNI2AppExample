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
	doRawDepth = true;
	doColor = true;
	depthPixelFormat = PIXEL_FORMAT_DEPTH_1_MM;
	colorPixelFormat = PIXEL_FORMAT_RGB888;
	doRegisterDepthToColor = true;
	isKinect = false;
	doPointCloud = true;			// option for point cloud
	doPointCloudColor = true;		// color point cloud
	useOniFile = false;
	oniFilePath = "UNDEFINED";
}

ofxOpenNI2Grabber::ofxOpenNI2Grabber()
{
	isReady = false;	
}
bool ofxOpenNI2Grabber::setup(Settings settings_)
{
	settings = settings_;
	
	Status status = STATUS_OK;

	status = OpenNI::initialize();
	if (status == STATUS_OK)
	{
		ofLogVerbose() << "initialize PASS";
	
		Array<DeviceInfo> deviceInfoList;
		OpenNI::enumerateDevices(&deviceInfoList);
		for (int i=0; i<deviceInfoList.getSize(); i++) 
		{
			ofLogVerbose() << "Device URI: "			<< deviceInfoList[i].getUri();
			ofLogVerbose() << "Device Vendor: "			<< deviceInfoList[i].getVendor();
			string deviceName = deviceInfoList[i].getName();
			if (deviceName == "Kinect") 
			{
				settings.isKinect = true;
				ofLogVerbose() << "DEVICE IS KINECT";
				
			}
			ofLogVerbose() << "Device Name: "			<< deviceName;
			ofLogVerbose() << "Device USB Vendor ID "	<< deviceInfoList[i].getUsbVendorId();
			ofLogVerbose() << "Device USB Product ID: "	<< deviceInfoList[i].getUsbProductId();
		}
		
	}else 
	{
		ofLog(OF_LOG_ERROR, "initialize FAIL:\n%s\n", OpenNI::getExtendedError());
	}
	if (settings.useOniFile) 
	{
		status = device.open(settings.oniFilePath.c_str());
	}else 
	{
		device.open(ANY_DEVICE);
	}
	

	if (status == STATUS_OK)
	{
		ofLogVerbose() << "Device open PASS";
		ofxOpenNI2GrabberUtils::printDeviceInfo(device);
	}else 
	{
		ofLogError() << "Device open FAIL: " << OpenNI::getExtendedError();
		device.close();
		return false;
	}
	
	ofxOpenNI2GrabberUtils::printModes(device);

	if (settings.doDepth) 
	{
		
		depthSource.setup(device);
		streams.push_back(&depthSource.videoStream);
		/*if(!settings.useOniFile && !settings.isKinect)
		{
			const VideoMode* requestedMode = findMode(device, SENSOR_DEPTH); 
			if (requestedMode != NULL) 
			{
				depth.setVideoMode(*requestedMode);
			}
		}*/
		
	}
	if (settings.doColor) 
	{
		rgbSource.setup(device);
		streams.push_back(&rgbSource.videoStream);
	}
	if(settings.doRegisterDepthToColor)
	{
		if (device.isImageRegistrationModeSupported(IMAGE_REGISTRATION_DEPTH_TO_COLOR)) 
		{
			status = device.setImageRegistrationMode(IMAGE_REGISTRATION_DEPTH_TO_COLOR);
			if (status == STATUS_OK)
			{
				ofLogVerbose() << "IMAGE_REGISTRATION_DEPTH_TO_COLOR PASS";
			}else
			{
				ofLogError() << "IMAGE_REGISTRATION_DEPTH_TO_COLOR FAIL:" << OpenNI::getExtendedError();
			}
		}else 
		{
			ofLogError() << "Device does not support IMAGE_REGISTRATION_DEPTH_TO_COLOR";
		}

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





//The Kinect/freenect driver does not find anything, The Xtion Pro has options
const VideoMode* ofxOpenNI2Grabber::findMode(Device& device, SensorType sensorType)
{
	const VideoMode* mode = NULL;
	const SensorInfo* sensorInfo = device.getSensorInfo(sensorType);
	const Array<VideoMode>& videoModes = sensorInfo->getSupportedVideoModes();
	for (int i=0; i<videoModes.getSize(); i++) 
	{
		const VideoMode& currentMode  = videoModes[i];
		ofLogVerbose() << i;
		switch (sensorType) 
		{
			case SENSOR_DEPTH:
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
			case SENSOR_COLOR:
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
bool ofxOpenNI2Grabber::close()
{
	isReady = false;
	stopThread();
	
	depthSource.close();
	rgbSource.close();
	device.close();
	
	OpenNI::shutdown();
	return isReady;
	//
}



