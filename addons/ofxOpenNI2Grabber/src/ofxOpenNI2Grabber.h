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
#include "RGBSource.h"
#include "DepthSource.h"

/*
 OpenNI::addDeviceStateChangedListener(this);
 OpenNI::addDeviceConnectedListener(this);
 OpenNI::addDeviceDisconnectedListener(this);
 */
using namespace openni;

class ofxOpenNI2Grabber: 
public ofThread
{
public:
	struct Settings;
	
	ofxOpenNI2Grabber();
	bool setup(Settings _settings);
	void readFrame();
	void draw();
	bool close();
	
	Device device;
	RGBSource rgbSource;
	DepthSource depthSource;
	const char* deviceURI;


	vector<VideoStream*> streams;

	

	
	void generateDepthPixels();
	bool isReady;
	
	float	deviceMaxDepth;
	
	bool isKinect;

	const VideoMode* findMode(Device& device, SensorType sensorType);
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
		bool doRegisterDepthToColor;
		bool isKinect;
		bool	useOniFile;
		string oniFilePath;
		Settings();
	};
	Settings 			settings;
	
	/*ofMesh pointCloud;
	bool isPointCloudValid;
	ofMesh & getPointCloud();*/
protected:
	void threadedFunction();
	
	
};