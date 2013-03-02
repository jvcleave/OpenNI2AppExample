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
	depthPixelFormat = PIXEL_FORMAT_DEPTH_100_UM;
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
	if (settings.useOniFile) {
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
		ofLog(OF_LOG_VERBOSE, "Device open FAIL:\n%s\n", OpenNI::getExtendedError());
		device.close();
		return false;
	}
	
	ofxOpenNI2GrabberUtils::printModes(device);

	if (settings.doDepth) 
	{
		status = depth.create(device, SENSOR_DEPTH);
		if (status == STATUS_OK)
		{
			ofLogVerbose() << "Find depth stream PASS";
			
			status = depth.start();
			if (status == STATUS_OK)
			{
				ofLogVerbose() << "Start depth stream PASS";
			}else 
			{
				ofLog(OF_LOG_VERBOSE,"Start depth stream FAIL:\n%s\n", OpenNI::getExtendedError());
				depth.destroy();
			}
			
		}else
		{
			ofLog(OF_LOG_VERBOSE,"Find depth stream FAIL:\n%s\n", OpenNI::getExtendedError());
		}
		
		if(!settings.useOniFile && !settings.isKinect)
		{
			const VideoMode* requestedMode = findMode(device, SENSOR_DEPTH); 
			if (requestedMode != NULL) 
			{
				depth.setVideoMode(*requestedMode);
			}
		}
		if (depth.isValid())
		{
			ofLogVerbose() << "DEPTH streams VALID";
			allocateDepthBuffers();
			if (settings.doRawDepth) 
			{
				allocateDepthRawBuffers();
			}
			/*status = depth.addNewFrameListener(this);
			if (status == STATUS_OK)
			{
				ofLogVerbose() << "DEPTH Add onNewFrameListener PASS";
			}*/
		}else 
		{
			ofLogVerbose() << "DEPTH streams INVALID";
			device.close();
			return false;
		}
	}
	
	if (settings.doColor) 
	{
		status = color.create(device, SENSOR_COLOR);
		if (status == STATUS_OK)
		{
			ofLogVerbose() << "Find color stream PASS";
			status = color.start();
			if (status == STATUS_OK)
			{
				ofLogVerbose() << "Start color stream PASS";
			}else 
			{
				
				ofLog(OF_LOG_VERBOSE,"Start color stream FAIL:\n%s\n", OpenNI::getExtendedError());
				color.destroy();
			}
		}else
		{
			ofLog(OF_LOG_VERBOSE,"Find color stream FAIL:\n%s\n", OpenNI::getExtendedError());
		}
		
		if(!settings.useOniFile && !settings.isKinect)
		{
			const VideoMode* requestedColorMode = findMode(device, SENSOR_COLOR); 
			if (requestedColorMode != NULL) 
			{
				color.setVideoMode(*requestedColorMode);
			}
		}
		if (color.isValid())
		{
			allocateColorBuffers();
			ofLogVerbose() << "COLOR stream VALID";
			/*status = color.addNewFrameListener(this);
			if (status == STATUS_OK)
			{
				ofLogVerbose() << "COLOR Add onNewFrameListener PASS";
			}else {
				ofLogError() << "COLOR Add onNewFrameListener FAIL" << OpenNI::getExtendedError();
			}*/

		}else 
		{
			ofLogVerbose() << "COLOR stream INVALID";
			device.close();
			return false;
		}
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
				ofLog(OF_LOG_ERROR,"IMAGE_REGISTRATION_DEPTH_TO_COLOR FAIL:\n%s\n", OpenNI::getExtendedError());
			}
		}else 
		{
			ofLogError() << "Device does not support IMAGE_REGISTRATION_DEPTH_TO_COLOR";
		}

	}
	streams.resize(2);
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
	
	if (settings.doPointCloud) 
	{
		pointCloud.setMode(OF_PRIMITIVE_POINTS);
		pointCloud.getVertices().resize(depthWidth*depthHeight);
		pointCloud.getColors().resize(depthWidth*depthHeight);
		int i=0;
		for(int y=0; y<depthHeight; y++)
		{
			for(int x=0; x<depthWidth; x++)
			{
				pointCloud.getVertices()[i].set(float(x)/depthWidth, float(y)/depthHeight, 0);
				i++;
			}
		}
		
	}

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
	Status rc = OpenNI::waitForAnyStream(&streams[0], streams.size(), &changedIndex);
	if (rc != STATUS_OK)
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
ofMesh & ofxOpenNI2Grabber::getPointCloud(){
	//lock();
	return pointCloud;
}


void ofxOpenNI2Grabber::generateDepthPixels()
{
	//lock();
	depth.readFrame(&depthFrame);
	const DepthPixel* oniDepthPixels = (const DepthPixel*)depthFrame.getData();
	
	if (settings.doRawDepth) 
	{
		backDepthRawPixels->setFromPixels(oniDepthPixels, depthWidth, depthHeight, 1);
		swap(backDepthRawPixels,currentDepthRawPixels);
	}
	if (settings.doPointCloud)
	{
			const DepthPixel* pcDepthPixels = oniDepthPixels;
			for (unsigned short y = 0; y < depthHeight; y++) 
			{
				ofVec3f* point = &pointCloud.getVertices()[0] + y * depthWidth;
		
				for (unsigned short x = 0; x < depthWidth; x++, pcDepthPixels++, point++) 
				{
					point->z = (*pcDepthPixels)/deviceMaxDepth;
					point->z*=5.0f;
				}
			}
			if(settings.doPointCloudColor)
			{
				unsigned char * rgbColorPtr = currentRGBPixels->getPixels();
				for(int i=0;i<(int)pointCloud.getColors().size();i++)
				{
					pointCloud.getColors()[i] = ofColor(*rgbColorPtr, *(rgbColorPtr+1), *(rgbColorPtr+2));
					rgbColorPtr+=3;
				}
			}
			
		
	}
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
void ofxOpenNI2Grabber::calculateHistogram(float* pHistogram, int histogramSize, const VideoFrameRef& frame)
{
	const DepthPixel* pDepth = (const DepthPixel*)frame.getData();
	// Calculate the accumulative histogram (the yellow display...)
	memset(pHistogram, 0, histogramSize*sizeof(float));
	int restOfRow = frame.getStrideInBytes() / sizeof(DepthPixel) - frame.getWidth();
	int height = frame.getHeight();
	int width = frame.getWidth();
	
	unsigned int nNumberOfPoints = 0;
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x, ++pDepth)
		{
			if (*pDepth != 0)
			{
				pHistogram[*pDepth]++;
				nNumberOfPoints++;
			}
		}
		pDepth += restOfRow;
	}
	for (int nIndex=1; nIndex<histogramSize; nIndex++)
	{
		pHistogram[nIndex] += pHistogram[nIndex-1];
	}
	if (nNumberOfPoints)
	{
		for (int nIndex=1; nIndex<histogramSize; nIndex++)
		{
			pHistogram[nIndex] = (256 * (1.0f - (pHistogram[nIndex] / nNumberOfPoints)));
		}
	}
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
	
	OpenNI::shutdown();
	return isReady;
	//
}



