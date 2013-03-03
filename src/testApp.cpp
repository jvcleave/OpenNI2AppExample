#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);

	ofxOpenNI2GrabberSettings settings;
	settings.width = 640;
	settings.height = 480;
	settings.fps = 30;
	settings.doDepth = true;
	settings.doRawDepth = true;
	settings.doColor = true;
	settings.depthPixelFormat = PIXEL_FORMAT_DEPTH_100_UM;
	settings.colorPixelFormat = PIXEL_FORMAT_RGB888;
	settings.doRegisterDepthToColor = false;
	settings.useOniFile = false;
	settings.oniFilePath = "UNDEFINED";
	
	//will search this directory for an .oni file
	//if not found will use the first available camera
	
	ofDirectory currentONIDirectory(ofToDataPath("current", true));
	if (currentONIDirectory.exists()) 
	{
		currentONIDirectory.listDir();
		vector<ofFile> files = currentONIDirectory.getFiles();
		if (files.size()>0) 
		{
			settings.useOniFile = true;
			settings.oniFilePath = files[0].path();
			ofLogVerbose() << "using oniFilePath : " << settings.oniFilePath;
		}		
	}
	
	isReady = oniGrabber.setup(settings);
	
	ofLogVerbose() << "started";
	
}

//--------------------------------------------------------------
void testApp::update(){
	ofSetWindowTitle(ofToString(ofGetFrameRate()));
	
}

//--------------------------------------------------------------
void testApp::draw(){
	if (isReady) 
	{
		oniGrabber.draw();
	}
	
}
void testApp::exit()
{
	ofLogVerbose() << "EXIT";
	if (isReady) 
	{
		isReady = oniGrabber.close();
	}
	
}
//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}