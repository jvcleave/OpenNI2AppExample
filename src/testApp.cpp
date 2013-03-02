#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	int maxAttempts = 3;
	int attemptCounter = 0;
	ofxOpenNI2Grabber::Settings cameraSettings;
	cameraSettings.depthPixelFormat = openni::PIXEL_FORMAT_DEPTH_1_MM;
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
	camera.setupPerspective(false, 40, 0, -100000);
	camera.setup();
}
bool hasSetPos = false;
//--------------------------------------------------------------
void testApp::update(){
	ofSetWindowTitle(ofToString(ofGetFrameRate()));
	if (isReady) 
	{
		if (!hasSetPos)
		{
			hasSetPos =true;
			camera.setPosition(oniGrabber.getPointCloud().getCentroid());
		}
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	if (isReady) 
	{
		//oniGrabber.draw();
		camera.begin();
			//glPointSize(3);
			oniGrabber.getPointCloud().draw();
			//oniGrabber.getPointCloud().drawWireframe();
		camera.end();
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