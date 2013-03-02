#pragma once

#include "ofMain.h"
#include "OpenNI.h"

using namespace openni;

class DepthSource : public VideoStream::NewFrameListener
{
public:
	DepthSource();
	int width;
	int height;
	VideoStream videoStream;
	
	ofTexture texture;
	ofPixels pixels[2];
	ofPixels* backPixels;
	ofPixels* currentPixels;
	
	
	ofShortPixels rawPixels[2];
	ofShortPixels* backRawPixels;
	ofShortPixels* currentRawPixels;
	
	
	VideoMode videoMode;
	
	VideoFrameRef videoFrameRef;
	bool setup(Device& device);
	void allocateBuffers();
	void draw();
	void close();
	float	deviceMaxDepth;
	void onNewFrame(VideoStream&);
};

