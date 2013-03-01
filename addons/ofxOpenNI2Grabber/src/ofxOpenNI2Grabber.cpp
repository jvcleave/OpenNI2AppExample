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
	isKinect = false;
}
void ofxOpenNI2Grabber::printMode(openni::VideoMode mode)
{
	openni::PixelFormat pixelFormat = mode.getPixelFormat();
	string pixelFormatName;
	switch (pixelFormat) 
	{
		case openni::PIXEL_FORMAT_DEPTH_1_MM:
		{
			pixelFormatName = "PIXEL_FORMAT_DEPTH_1_MM";
			break;
		}
		case openni::PIXEL_FORMAT_DEPTH_100_UM:
		{
			pixelFormatName = "PIXEL_FORMAT_DEPTH_100_UM";
			break;
		}
		case openni::PIXEL_FORMAT_SHIFT_9_2:
		{
			pixelFormatName = "PIXEL_FORMAT_SHIFT_9_2";
			break;
		}
		case openni::PIXEL_FORMAT_SHIFT_9_3:
		{
			pixelFormatName = "PIXEL_FORMAT_SHIFT_9_3";
			break;
		}
		case openni::PIXEL_FORMAT_RGB888:
		{
			pixelFormatName = "PIXEL_FORMAT_RGB888";
			break;
		}
		case openni::PIXEL_FORMAT_YUV422:
		{
			pixelFormatName = "PIXEL_FORMAT_YUV422";
			break;
		}
		case openni::PIXEL_FORMAT_GRAY8:
		{
			pixelFormatName = "PIXEL_FORMAT_GRAY8";
			break;
		}
		case openni::PIXEL_FORMAT_GRAY16:
		{
			pixelFormatName = "PIXEL_FORMAT_GRAY16";
			break;
		}
		case openni::PIXEL_FORMAT_JPEG:
		{
			pixelFormatName = "PIXEL_FORMAT_JPEG";
			break;
		}
			
	}
	ofLogVerbose() << "PixelFormat: "	<< pixelFormatName;
	ofLogVerbose() << "ResolutionX: "	<< mode.getResolutionX();
	ofLogVerbose() << "ResolutionY: "	<< mode.getResolutionY();
	ofLogVerbose() << "FPS: "			<< mode.getFps();
}

void ofxOpenNI2Grabber::printModes()
{
	const openni::SensorInfo* colorSensorInfo = device.getSensorInfo(openni::SENSOR_COLOR);
	const openni::Array<openni::VideoMode>& colorVideoModes= colorSensorInfo->getSupportedVideoModes();
	ofLogVerbose() << "\n--------COLOR MODES--------\n";
	for (int i=0; i<colorVideoModes.getSize(); i++) 
	{
		ofLogVerbose() << "COLOR MODE: " << i;
		printMode(colorVideoModes[i]);
	}
	
	const openni::SensorInfo* depthSensorInfo = device.getSensorInfo(openni::SENSOR_DEPTH);
	const openni::Array<openni::VideoMode>& depthVideoModes = depthSensorInfo->getSupportedVideoModes();
	ofLogVerbose() << "\n--------DEPTH MODES--------\n";
	for (int i=0; i<depthVideoModes.getSize(); i++) 
	{
		ofLogVerbose() << "DEPTH MODE: " << i;
		printMode(depthVideoModes[i]);
	}
}

void ofxOpenNI2Grabber::printDeviceInfo()
{
	openni::DeviceInfo deviceInfo = device.getDeviceInfo();
	ofLogVerbose() << "Device URI: "			<< deviceInfo.getUri();
	ofLogVerbose() << "Device Vendor: "			<< deviceInfo.getVendor();
	string deviceName = deviceInfo.getName();
	if (deviceName == "Kinect") 
	{
		isKinect = true;
		ofLogVerbose() << "DEVICE IS KINECT";
		
	}
	ofLogVerbose() << "Device Name: "			<< deviceName;
	ofLogVerbose() << "Device USB Vendor ID "	<< deviceInfo.getUsbVendorId();
	ofLogVerbose() << "Device USB Product ID: "	<< deviceInfo.getUsbProductId();
}

void ofxOpenNI2Grabber::setup()
{
	openni::Status status = openni::STATUS_OK;
	
	deviceURI = openni::ANY_DEVICE;
	status = openni::OpenNI::initialize();
	
	ofLog(OF_LOG_VERBOSE, "After initialization:\n%s\n", openni::OpenNI::getExtendedError());
	
	status = device.open(deviceURI);


	if (status == openni::STATUS_OK)
	{
		ofLogVerbose() << "Device open PASS" <<  deviceURI;
		printDeviceInfo();
	}else 
	{
		ofLog(OF_LOG_VERBOSE, "Device open FAIL:\n%s\n", openni::OpenNI::getExtendedError());
		close();
	}
	
	if(!isKinect)
	{
		printModes();
	}
	
	
	
	status = depth.create(device, openni::SENSOR_DEPTH);
	if(!isKinect)
	{
		//depth.setVideoMode(depthVideoModes[8]);
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
	if(!isKinect)
	{
		//color.setVideoMode(colorVideoModes[8]);
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
	
	ofSleepMillis(5000);
	if (depth.isValid())
	{
		ofLogVerbose() << "DEPTH streams VALID";
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
		
	}else 
	{
		ofLogVerbose() << "DEPTH streams INVALID";
		close();
	}
	
	if (color.isValid())
	{
		ofLogVerbose() << "COLOR stream VALID";
		colorVideoMode = color.getVideoMode();
		colorWidth = colorVideoMode.getResolutionX();
		colorHeight = colorVideoMode.getResolutionY();
		
		rgbPixels[0].allocate(colorWidth, colorHeight, OF_IMAGE_COLOR);
		rgbPixels[1].allocate(colorWidth, colorHeight, OF_IMAGE_COLOR);
		currentRGBPixels = &rgbPixels[0];
		backRGBPixels = &rgbPixels[1];
		
		rgbTexture.allocate(colorWidth, colorHeight, GL_RGB);
		
	}else 
	{
		ofLogVerbose() << "COLOR stream INVALID";
		close();
		
	}

	if (depthWidth == colorWidth && depthHeight == colorHeight)
	{
		width = depthWidth;
		height = depthHeight;
		ofLogVerbose() << "width: " << width;
		ofLogVerbose() << "height: " << height;
	}else
	{
		ofLog(OF_LOG_ERROR, "Error - expect color and depth to be in same resolution: D: %dx%d, C: %dx%d\n",
			   depthWidth, depthHeight,
			   colorWidth, colorHeight);
	}
	streams = new openni::VideoStream*[2];
	streams[0] = &depth;
	streams[1] = &color;
	isReady = true;
}


void ofxOpenNI2Grabber::update()
{
	int changedIndex;
	openni::Status rc = openni::OpenNI::waitForAnyStream(streams, 2, &changedIndex);
	if (rc != openni::STATUS_OK)
	{
		ofLogError() << "Wait failed";
		return;
	}
	switch (changedIndex)
	{
		case 0:
		{
			
			//currentDepthPixels->setFromPixels((unsigned char *)depthFrame.getData(), depthWidth, depthHeight, OF_IMAGE_COLOR_ALPHA);
			generateDepthPixels();
			depthTexture.loadData(*currentDepthPixels);
			break;
		}
		case 1:
		{
			color.readFrame(&colorFrame);
			backRGBPixels->setFromPixels((unsigned char *)colorFrame.getData(), colorWidth, colorHeight, OF_IMAGE_COLOR);
			rgbTexture.loadData(*backRGBPixels);
			
			break;
		}
		default:
		{
			ofLogError() << "Error in wait";
		}
	}
}


void ofxOpenNI2Grabber::draw()
{
	rgbTexture.draw(0, 0);
	depthTexture.draw(width, 0);


}
void ofxOpenNI2Grabber::generateDepthPixels()
{
	depth.readFrame(&depthFrame);
	const openni::DepthPixel* oniDepthPixels = (const openni::DepthPixel*)depthFrame.getData();
	
	//backDepthRawPixels->setFromPixels(oniDepthPixels, depthWidth, depthHeight, 1);
	
	float max = 255;
	for (unsigned short y = 0; y < depthHeight; y++) 
	{
		unsigned char * texture = currentDepthPixels->getPixels() + y * depthWidth * 4;
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
	
}
void ofxOpenNI2Grabber::close()
{
	isReady = false;
	//openni::OpenNI::shutdown();
}