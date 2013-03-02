/*
 *  RGBSource.cpp
 *  OpenNI2AppExample
 *
 *  Created by jason van cleave on 3/2/13.
 *  Copyright 2013 jasonvancleave.com. All rights reserved.
 *
 */

#include "RGBSource.h"

RGBSource::RGBSource()
{
	width = 0;
	height = 0;
	backPixels = NULL;
	currentPixels = NULL;
}
bool RGBSource::setup(Device& device)
{
	bool isReady = false;
	Status status = STATUS_OK;
	
	status = videoStream.create(device, SENSOR_COLOR);
	if (status == STATUS_OK)
	{
		ofLogVerbose() << "Find RGBSource videoStream PASS";
		status = videoStream.start();
		if (status == STATUS_OK)
		{
			ofLogVerbose() << "RGBSource videoStream start PASS";
		}else 
		{
			
			ofLogError() << "RGBSource videoStream start FAIL: " << OpenNI::getExtendedError();
			videoStream.destroy();
		}
	}else
	{
		ofLogError() << "Find RGBSource videoStream FAIL: "<< OpenNI::getExtendedError();
	}
	if (videoStream.isValid())
	{
		allocateBuffers();
		status = videoStream.addNewFrameListener(this);
		if (status == STATUS_OK)
		{
			ofLogVerbose() << "RGBSource videoStream addNewFrameListener PASS";
		}
		
		isReady = true;
	}else 
	{
		ofLogError() << "RGBSource is INVALID";
	}

	
	return isReady;
}
void RGBSource::draw()
{
	texture.loadData(*currentPixels);
	texture.draw(0, 0);
}
void RGBSource::allocateBuffers()
{
	videoMode = videoStream.getVideoMode();
	width = videoMode.getResolutionX();
	height = videoMode.getResolutionY();
	
	pixels[0].allocate(width, height, OF_IMAGE_COLOR);
	pixels[1].allocate(width, height, OF_IMAGE_COLOR);
	currentPixels = &pixels[0];
	backPixels = &pixels[1];
	
	texture.allocate(width, height, GL_RGB);
}
void RGBSource::onNewFrame(VideoStream& stream)
{
	//ofLogVerbose() << "RGBSource::onNewFrame";
	stream.readFrame(&videoFrameRef);
	backPixels->setFromPixels((unsigned char *)videoFrameRef.getData(), width, height, OF_IMAGE_COLOR);
	swap(backPixels, currentPixels);
}
void RGBSource::close()
{
	videoStream.stop();
	videoStream.destroy();
}