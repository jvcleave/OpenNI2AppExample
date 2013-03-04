#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	/*
	Defaults for desktop
	Some strange behavior when using color
	the texture will sometimes be black but if you physically pick up the camera (seriously) it will kick on 
	*/
	settings.width = 640;
	settings.height = 480;
	settings.fps = 30;
	settings.doDepth = true;
	settings.doRawDepth = true;
	settings.doColor = true;


#ifdef TARGET_OPENGLES
	/*
	 in my tests on the RPi - these work well
	 depth only 320x240@25,30,60 fps
	 color only 320x240@25,30,60 fps
	 
	 color only 320x240 60fps will have a color shift and occassional glitches
	 
	 color will glitch out when both are on
	 640x480 works in both but much slower
	 
	 depth recordings 320x240 @30fps play back well on the RPi
	 sample: http://jvcref.com/files/oni/depth_320_240_30_2013-03-04-15-09-15-430.oni
	 */
	settings.width = 320;
	settings.height = 240;
	settings.fps = 30;
	settings.doDepth = true;
	settings.doRawDepth = true;
	settings.doColor = true;
#endif

	
	settings.depthPixelFormat = PIXEL_FORMAT_DEPTH_1_MM;
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
	
	ofLogVerbose() << "testApp started";
	string oniFileName = "";
	if (settings.doDepth) 
	{
		oniFileName+="depth_";
	}
	if (settings.doColor) 
	{
		oniFileName+="color_";
	}
	oniFileName+= ofToString(settings.width);
	oniFileName+="_";
	oniFileName+= ofToString(settings.height);
	oniFileName+="_";
	oniFileName+= ofToString(settings.fps);
	oniFileName+="_";
	oniFileName+=ofGetTimestampString();
	
	string oniRecordingPath = ofToDataPath(oniFileName+".oni", true);
	recorder.create(oniRecordingPath.c_str());
	if (oniGrabber.depthSource.isOn) 
	{
		recorder.attach(oniGrabber.depthSource.videoStream, false);
	}
	if (oniGrabber.rgbSource.isOn) 
	{
		recorder.attach(oniGrabber.rgbSource.videoStream, false);
	}
	
	
}

//--------------------------------------------------------------
void testApp::update(){
	if (isReady) 
	{
		oniGrabber.update();
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	if (isReady) 
	{
		//oniGrabber.draw();
		if (settings.doDepth) 
		{
			ofTexture depth = oniGrabber.getDepthTextureReference();
			depth.draw(0, 0);
		}
		if (settings.doColor) 
		{
			ofTexture color = oniGrabber.getRGBTextureReference();
			color.draw(color.getWidth(), 0);
		}
		
	}
	ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), 20, 400, ofColor(0, 0, 0, 128), ofColor::yellow);
}
void testApp::exit()
{
	ofLogVerbose() << "\n EXITING, be patient - takes some time \n";
	if (isReady) 
	{
		oniGrabber.close();
	}
	
}
//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if (key == 'r') 
	{
		recorder.start();
	}
	if (key == ' ') 
	{
		recorder.stop();
	}
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