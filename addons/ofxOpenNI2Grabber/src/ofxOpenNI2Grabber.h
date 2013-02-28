#pragma once

/*
 *  ofxOpenNI2Grabber.h
 *
 *  Created by jason van cleave on 2/28/13.
 *
 */
#include "ofMain.h"
#include <OpenNI.h>

class ofxOpenNI2Grabber
{
	ofxOpenNI2Grabber();
	void setup();
	void update();
	void draw();
	void close();
	
	openni::Device device;
	openni::VideoStream depth, color;
};