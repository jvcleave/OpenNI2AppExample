#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	int maxAttempts = 3;
	int attemptCounter = 0;
	ofxOpenNI2Grabber::Settings cameraSettings;
	cameraSettings.doColor = false;
	
	ofDirectory currentONIDirectory(ofToDataPath("current", true));
	if (currentONIDirectory.exists()) 
	{
		currentONIDirectory.listDir();
		vector<ofFile> files = currentONIDirectory.getFiles();
		if (files.size()>0) 
		{
			cameraSettings.useOniFile = true;
			cameraSettings.oniFilePath = files[0].path();
			ofLogVerbose() << "using oniFilePath : " << cameraSettings.oniFilePath;
		}		
	}
	
	//cameraSettings.depthPixelFormat = openni::PIXEL_FORMAT_DEPTH_1_MM;
	while (!isReady) 
	{
		isReady = oniGrabber.setup(cameraSettings);
		attemptCounter++;
		
		ofLogVerbose() << "attemptCounter: " << attemptCounter;
		if (attemptCounter == maxAttempts) 
		{
			ofExit(0);
		}
	}
	ofLogVerbose() << "started";
	oniGrabber.startThread(false, false);
	//camera.setupPerspective(true, 40, 0, -100000);
	//camera.setup();
}
bool hasSetPos = false;
//--------------------------------------------------------------
void testApp::update(){
	ofSetWindowTitle(ofToString(ofGetFrameRate()));
	
}

//--------------------------------------------------------------
void testApp::draw(){
	if (isReady) 
	{
		oniGrabber.draw();
		/*camera.begin();
			ofScale(200, 200, 200);
			glPointSize(3);
			oniGrabber.getPointCloud().draw();
			//oniGrabber.getPointCloud().drawWireframe();
		camera.end();*/
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