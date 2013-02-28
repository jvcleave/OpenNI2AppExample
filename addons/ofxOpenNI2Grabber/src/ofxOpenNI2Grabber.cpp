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

void ofxOpenNI2Grabber::printModes(openni::SensorType sensorType)
{
	string name;
	
	switch (sensorType)
	{
		case openni::SENSOR_COLOR:
		{
			name = "COLOR";
			break;
		}
		case openni::SENSOR_DEPTH:
		{
			name = "DEPTH";
			break;
		}
		case openni::SENSOR_IR:
		{
			name = "IR";
			break;
		}
	}
	const openni::SensorInfo* sensorInfo = device.getSensorInfo(sensorType);
	const openni::Array<openni::VideoMode>& videoModes  = sensorInfo->getSupportedVideoModes();
	for (int i=0; i<videoModes.getSize(); i++) 
	{
		openni::VideoMode mode = videoModes[i];
		
		ofLogVerbose() << name << " MODE: "	<< i;
		
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
		
		printModes(openni::SENSOR_COLOR);
	
		printModes(openni::SENSOR_DEPTH);
		printModes(openni::SENSOR_IR);
		
		
	}else 
	{
		ofLog(OF_LOG_VERBOSE, "Device open FAIL:\n%s\n", openni::OpenNI::getExtendedError());
		close();
	}
	const openni::SensorInfo* colorSensorInfo = device.getSensorInfo(openni::SENSOR_COLOR);
	const openni::Array<openni::VideoMode>& colorVideoModes  = colorSensorInfo->getSupportedVideoModes();
	
	
	
	const openni::SensorInfo* depthSensorInfo = device.getSensorInfo(openni::SENSOR_DEPTH);
	const openni::Array<openni::VideoMode>& depthVideoModes  = depthSensorInfo->getSupportedVideoModes();
	
	
	
	status = depth.create(device, openni::SENSOR_DEPTH);
	depth.setVideoMode(depthVideoModes[8]);
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
	color.setVideoMode(colorVideoModes[8]);
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
		depthVideoMode = depth.getVideoMode();
		depthWidth = depthVideoMode.getResolutionX();
		depthHeight = depthVideoMode.getResolutionY();
		
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
			depth.readFrame(&depthFrame);
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

}

void ofxOpenNI2Grabber::generateDepthPixels()
{
	
	
	/*const openni::DepthPixel* pDepthRow = (const openni::DepthPixel*)depthFrame.getData();
	openni::RGB888Pixel* pTexRow = m_pTexMap + depthFrame.getCropOriginY() * m_nTexMapX;
	int rowSize = depthFrame.getStrideInBytes() / sizeof(openni::DepthPixel);
	
	for (int y = 0; y < depthFrame.getHeight(); ++y)
	{
		const openni::DepthPixel* pDepth = pDepthRow;
		openni::RGB888Pixel* pTex = pTexRow + depthFrame.getCropOriginX();
		
		for (int x = 0; x < depthFrame.getWidth(); ++x, ++pDepth, ++pTex)
		{
			if (*pDepth != 0)
			{
				int nHistValue = m_pDepthHist[*pDepth];
				pTex->r = nHistValue;
				pTex->g = nHistValue;
				pTex->b = 0;
			}
		}
		
		pDepthRow += rowSize;
		pTexRow += m_nTexMapX;
	}*/
	
}
void ofxOpenNI2Grabber::close()
{
	isReady = false;
	openni::OpenNI::shutdown();
}