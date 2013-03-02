/*
 *  ofxGameCamera.cpp
 *
 *  Created by James George on 1/19/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 */

#include "ofxGameCamera.h"

static float ClampAngle (float angle, float min, float max) {
	if (angle < -360.0f)
		angle += 360.f;
	if (angle > 360.0f)
		angle -= 360.0f;
	return ofClamp(angle, min, max);
}


ofxGameCamera::ofxGameCamera() {
	
	sensitivityX = 0.15f;
	sensitivityY = 0.15f;

	minimumX = -360.0f;
	maximumX =  360.0f;

	minimumY = -60.0f;
	maximumY =  60.0f;

	rotationX = 0.0f;
	rotationY = 0.0f;
	rotationZ = 0.0f;

	targetXRot = 0.0f;
	targetYRot = 0.0f;
	targetZRot = 0.0f;
	
	speed = 10.0f;
	
	lastMouse = ofVec2f(0,0);

	usemouse = true;
	autosavePosition = false;
	useArrowKeys = true;

	applyRotation = true;
	applyTranslation = true;

	rollSpeed = 2;

	hasLoaded = false;
	cameraPositionFile =  "_gameCameraPosition.xml";
}

void ofxGameCamera::setup(){
	ofAddListener(ofEvents().update, this, &ofxGameCamera::update);
}

void ofxGameCamera::moveForward()
{
	targetNode.dolly(-speed);
}
void ofxGameCamera::moveBack()
{
	targetNode.dolly(speed);
}

void ofxGameCamera::moveLeft()
{
	targetNode.truck(-speed);
}
void ofxGameCamera::moveRight()
{
	targetNode.truck(speed);
}
void ofxGameCamera::moveUp()
{
	targetNode.boom(speed);
}
void ofxGameCamera::moveDown()
{
	targetNode.boom(-speed);
}
void ofxGameCamera::update(ofEventArgs& args){	
    
    ofVec3f startPos = getPosition();
	ofVec2f startRot = ofVec3f(rotationX, rotationY, rotationZ);
    
	//forward
    if(applyTranslation){
        if(ofGetKeyPressed('w') || (useArrowKeys && ofGetKeyPressed(OF_KEY_UP)) ){
			moveForward();

        }
        
        if(ofGetKeyPressed('s') || (useArrowKeys && ofGetKeyPressed(OF_KEY_DOWN)) ){    
			moveBack();

        }
        
        if(ofGetKeyPressed('a') || (useArrowKeys && ofGetKeyPressed(OF_KEY_LEFT)) ){
            moveLeft();
        }
        
        if(ofGetKeyPressed('d') || (useArrowKeys && ofGetKeyPressed(OF_KEY_RIGHT)) ){
            moveRight();
        }
        
        if(ofGetKeyPressed('c') || (useArrowKeys && ofGetKeyPressed(OF_KEY_PAGE_DOWN)) ){
			moveDown();
        }
        
        if(ofGetKeyPressed('e') || (useArrowKeys && ofGetKeyPressed(OF_KEY_PAGE_UP)) ){
            moveUp();
        }
		
		//TODO make variable
		setPosition(getPosition() + (targetNode.getPosition() - getPosition()) *.2);
	}
	
	if(applyRotation){	
		if(ofGetKeyPressed('r')){
			targetZRot += rollSpeed;
            applyRotation = true;
            //			updateRotation();		
        }
		if(ofGetKeyPressed('q')){
			targetZRot -= rollSpeed;
            //			updateRotation();	
            applyRotation = true;
		}
	}
	
    
	ofVec2f mouse( ofGetMouseX(), ofGetMouseY() );
	if(usemouse && applyRotation && ofGetMousePressed(0)){
        
		float dx = (mouse.x - lastMouse.x) * sensitivityX;
		float dy = (mouse.y - lastMouse.y) * sensitivityY;
		targetXRot += dx;
		targetYRot += dy;
		
		targetYRot = ClampAngle(targetYRot, minimumY, maximumY);
		targetXRot = ClampAngle(targetXRot, minimumX, maximumX);
	}
	
	if(applyRotation){
		updateRotation();		
	}
	
	lastMouse = mouse;
    
	if(!ofGetMousePressed(0) && autosavePosition && (startPos != getPosition() || startRot != ofVec3f(rotationX, rotationY, rotationZ))){
		saveCameraPosition();
	}
}

void ofxGameCamera::begin(ofRectangle rect) { 
	ofCamera::begin(rect);
}

void ofxGameCamera::setAnglesFromOrientation(){
	ofVec3f rotation = getOrientationEuler();
	rotationX = targetXRot = -rotation.y;
	rotationY = targetYRot = -rotation.z;
	rotationZ = targetZRot = -rotation.x;
}

void ofxGameCamera::updateRotation()
{
	rotationX += (targetXRot - rotationX) *.2;
	rotationY += (targetYRot - rotationY) *.2;
	rotationZ += (targetZRot - rotationZ) *.2;
	
	setOrientation(ofQuaternion(0,0,0,1)); //reset	
	setOrientation(getOrientationQuat() * ofQuaternion(-rotationZ, getZAxis()));
	setOrientation(getOrientationQuat() * ofQuaternion(-rotationX, getYAxis()));
	setOrientation(getOrientationQuat() * ofQuaternion(-rotationY, getXAxis()));
		
	targetNode.setOrientation(getOrientationQuat());
}

void ofxGameCamera::saveCameraPosition()
{
	ofxXmlSettings savePosition;
	savePosition.addTag("camera");
	savePosition.pushTag("camera");

	savePosition.addTag("position");
	savePosition.pushTag("position");

	savePosition.addValue("X", getPosition().x);
	savePosition.addValue("Y", getPosition().y);
	savePosition.addValue("Z", getPosition().z);

	savePosition.popTag(); //pop position

	savePosition.addTag("rotation");
	savePosition.pushTag("rotation");

	savePosition.addValue("X", rotationX);
	savePosition.addValue("Y", rotationY);
	savePosition.addValue("Z", rotationZ);

	savePosition.popTag(); //pop rotation

	savePosition.popTag(); //camera;

	savePosition.saveFile(cameraPositionFile);
}
void ofxGameCamera::loadCachedPosition()
{
	targetNode.setPosition(cachedTargetNodePosition);
	targetXRot = rotationX = cachedRotationX; 
	targetYRot = rotationY = cachedRotationY;
	targetZRot = rotationZ = cachedRotationZ; 
	setFov(cachedFov);
	updateRotation();
}
void ofxGameCamera::loadCameraPosition()
{
	if (hasLoaded)
	{
		loadCachedPosition();
		return;
	}
	ofxXmlSettings loadPosition;
	if(loadPosition.loadFile(cameraPositionFile)){

		loadPosition.pushTag("camera");
		loadPosition.pushTag("position");
		// tig: defaulting with floats so as to get floats back.
		setPosition(ofVec3f(loadPosition.getValue("X", 0.),
							loadPosition.getValue("Y", 0.),
							loadPosition.getValue("Z", 0.)));
		targetNode.setPosition(getPosition());
		cachedTargetNodePosition = getPosition();
		
		loadPosition.popTag();

		loadPosition.pushTag("rotation");
		cachedRotationX = targetXRot = rotationX = loadPosition.getValue("X", 0.);
		cachedRotationY = targetYRot = rotationY = loadPosition.getValue("Y", 0.);
		cachedRotationZ =  targetZRot = rotationZ = loadPosition.getValue("Z", 0.);
		float fov = loadPosition.getValue("FOV", -1);
		
		if(fov != -1){
			setFov(fov);
		}
		cachedFov = getFov();
		loadPosition.popTag();

		loadPosition.popTag(); //pop camera;

		updateRotation();
		hasLoaded = true;
	}
	else{
		ofLog(OF_LOG_ERROR, "ofxGameCamera: couldn't load position!");
	}

}

void ofxGameCamera::reset(){
 	rotationX = 0.0f;
	rotationY = 0.0f;
	rotationZ = 0.0f;
    
	targetXRot = 0.0f;
	targetYRot = 0.0f;
	targetZRot = 0.0f;
    
    targetNode.setPosition(ofVec3f(0,0,0));
    targetNode.setOrientation(ofQuaternion());

}
