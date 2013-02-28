#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	oniGrabber.setup();

}

//--------------------------------------------------------------
void testApp::update(){
	if (oniGrabber.isReady) 
	{
		oniGrabber.update();
	}
	
}

//--------------------------------------------------------------
void testApp::draw(){
	if (oniGrabber.isReady) 
	{
		oniGrabber.draw();
	}
	
}
void testApp::exit()
{
	ofLogVerbose() << "EXIT";
	oniGrabber.close();
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