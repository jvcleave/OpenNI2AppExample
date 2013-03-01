#pragma once
/*
 *  ofxOpenNI2GrabberUtils.h
 *
 *  Created by jason van cleave on 3/1/13.
 *
 */

#include "ofMain.h"
#include <OpenNI.h>


class ofxOpenNI2GrabberUtils
{
public:
	static void printDeviceInfo(openni::Device& device)
	{
		openni::DeviceInfo deviceInfo = device.getDeviceInfo();
		ofLogVerbose() << "Device URI: "			<< deviceInfo.getUri();
		ofLogVerbose() << "Device Vendor: "			<< deviceInfo.getVendor();
		string deviceName = deviceInfo.getName();
		if (deviceName == "Kinect") 
		{
			//isKinect = true;
			ofLogVerbose() << "DEVICE IS KINECT";
			
		}
		ofLogVerbose() << "Device Name: "			<< deviceName;
		ofLogVerbose() << "Device USB Vendor ID "	<< deviceInfo.getUsbVendorId();
		ofLogVerbose() << "Device USB Product ID: "	<< deviceInfo.getUsbProductId();
	}
	
	static void printMode(openni::VideoMode mode)
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
	static void printModes(openni::Device& device)
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
	
	


};