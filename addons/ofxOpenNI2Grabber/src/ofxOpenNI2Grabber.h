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
public:
	ofxOpenNI2Grabber();
	void setup();
	void update();
	void draw();
	void close();
	
	openni::Device device;
	const char* deviceURI;

	openni::VideoStream depth;
	openni::VideoMode depthVideoMode;
	openni::VideoFrameRef depthFrame;
	int depthWidth;
	int depthHeight;
	
	openni::VideoStream color;
	openni::VideoMode colorVideoMode;
	openni::VideoFrameRef colorFrame;
	int colorWidth;
	int colorHeight;
	
	int width;
	int height;
	
	openni::VideoStream**		streams;
	ofTexture depthTexture;
	ofPixels depthPixels[2];
	ofPixels* backDepthPixels;
	ofPixels* currentDepthPixels;
	
	// depth raw
	ofShortPixels depthRawPixels[2];
	ofShortPixels* backDepthRawPixels;
	ofShortPixels* currentDepthRawPixels;
	
	// rgb
	ofTexture rgbTexture;
	ofPixels rgbPixels[2];
	ofPixels* backRGBPixels;
	ofPixels* currentRGBPixels;
	
	void generateDepthPixels();
	bool isReady;
	
	
	void printDeviceInfo();
	float	deviceMaxDepth;
	
	bool isKinect;

	
	void printModes();
	void printMode(openni::VideoMode mode);
	
};