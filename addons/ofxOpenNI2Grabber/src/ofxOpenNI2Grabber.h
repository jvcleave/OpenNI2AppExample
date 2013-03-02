#pragma once

/*
 *  ofxOpenNI2Grabber.h
 *
 *  Created by jason van cleave on 2/28/13.
 *
 */
#include "ofMain.h"
#include "OpenNI.h"
#include "ofxOpenNI2GrabberUtils.h"

using namespace openni;

class ofxOpenNI2Grabber: 
public ofThread, 
public OpenNI::DeviceStateChangedListener,
public OpenNI::DeviceConnectedListener,
public OpenNI::DeviceDisconnectedListener
{
public:
	struct Settings;
	
	ofxOpenNI2Grabber();
	bool setup(Settings _settings);
	void readFrame();
	void draw();
	bool close();
	
	Device device;
	const char* deviceURI;

	VideoStream depth;
	VideoMode depthVideoMode;
	VideoFrameRef depthFrame;
	int depthWidth;
	int depthHeight;
	
	VideoStream color;
	VideoMode colorVideoMode;
	VideoFrameRef colorFrame;
	int colorWidth;
	int colorHeight;
	vector<VideoStream*> streams;
	//VideoStream**		streams;
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
	
	float	deviceMaxDepth;
	
	bool isKinect;
//hometest_single.oni
	const VideoMode* findMode(Device& device, SensorType sensorType);
	void allocateDepthBuffers();
	void allocateDepthRawBuffers();
	void allocateColorBuffers();
	struct Settings {
		int		width;					// camera width
		int		height;					// camera height
		int		fps;					// camera fps
		bool	doDepth;				// use Depth camera
		bool	doRawDepth;				// capture raw Depth info
		bool	doColor;				// use RGB camera
		bool	doPointCloud;			// option for point cloud
		bool	doPointCloudColor;		// color point cloud
		PixelFormat depthPixelFormat; //PIXEL_FORMAT_DEPTH_1_MM, PIXEL_FORMAT_DEPTH_100_UM, PIXEL_FORMAT_SHIFT_9_2, PIXEL_FORMAT_SHIFT_9_3
		PixelFormat colorPixelFormat; //PIXEL_FORMAT_RGB888, PIXEL_FORMAT_YUV422, PIXEL_FORMAT_GRAY8, PIXEL_FORMAT_GRAY16, PIXEL_FORMAT_JPEG
		bool isKinect;
		bool	useOniFile;
		string oniFilePath;
		Settings();
	};
	Settings 			settings;
	
	//OpenNI::DeviceStateChangedListener callback
	void onDeviceStateChanged(const DeviceInfo*, DeviceState);
	
	//OpenNI::DeviceConnectedListener callback
	void onDeviceConnected(const DeviceInfo*);
	
	//OpenNI::DeviceDisconnectedListener callback
	void onDeviceDisconnected(const DeviceInfo*);
	ofMesh pointCloud;
	bool isPointCloudValid;
	ofMesh & getPointCloud();
protected:
	void threadedFunction();
	
	
};