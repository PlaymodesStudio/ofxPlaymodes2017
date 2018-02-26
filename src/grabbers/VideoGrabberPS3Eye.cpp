/*
 * AudioFrame.h
 *
 *  Created on: 09-oct-2008
 *      Author: arturo castro
 */

#include "VideoGrabberPS3Eye.h"
#include "parametersControl.h"

namespace ofxPm{
VideoGrabberPS3Eye::VideoGrabberPS3Eye(){
}

VideoGrabberPS3Eye::~VideoGrabberPS3Eye(){
}

bool VideoGrabberPS3Eye::initGrabber(int w, int h){
    
    // parametersGroup
    
    parameters = new ofParameterGroup();
    parameters->setName("VideoGrabberPS3Eye");
    parameters->add(paramExposure.set("Exposure",1,0,255));
    parameters->add(paramHue.set("Hue",0,0,255));
    parameters->add(paramAutoGain.set("AutoGrain",false));
    parameters->add(paramAutoWB.set("AutoWB",false));
    parameters->add(paramFrameOut.set("Frame Output", frame));
    
    parametersControl::getInstance().createGuiFromParams(parameters,ofColor::orange);
    
    paramExposure.addListener(this, &VideoGrabberPS3Eye::setExposure);
    paramAutoGain.addListener(this, &VideoGrabberPS3Eye::setAutoGain);
    paramHue.addListener(this, &VideoGrabberPS3Eye::setHue);
    paramAutoWB.addListener(this,&VideoGrabberPS3Eye::setAutoWhiteBalance);
    
    // list out the devices
    std::vector<ps3eye::PS3EYECam::PS3EYERef> devices( ps3eye::PS3EYECam::getDevices() );
    cout << "Devices List : " << devices.size() << endl;
    
    if(devices.size())
    {
        eye = devices.at(0);
        bool res = eye->init(w, h, 60);
        eye->start();
        width = eye->getWidth();
        height = eye->getHeight();
        
        videoFrame 	= new unsigned char[eye->getWidth()*eye->getHeight()*3];
        //        videoTexture.allocate(eye->getWidth(), eye->getHeight(), GL_BGR_EXT);
        videoTexture.allocate(eye->getWidth(), eye->getHeight(), GL_RGB);
    }
	return true;
}

VideoFrame VideoGrabberPS3Eye::getNextVideoFrame(){

    eye->getFrame(videoFrame);
    videoTexture.loadData(videoFrame, eye->getWidth(),eye->getHeight(), GL_BGR_EXT);
    ofPixels p;
    p.setFromPixels(videoFrame, 640, 480,OF_PIXELS_BGR);
    newFrame(p);

    return frame;
}

void VideoGrabberPS3Eye::update()
    {
//        ofVideoGrabber::update();
//        if(isFrameNew()){
        
        eye->getFrame(videoFrame);
        videoTexture.loadData(videoFrame, eye->getWidth(),eye->getHeight(), GL_BGR_EXT);
        ofPixels p;
        p.setFromPixels(videoFrame, 640, 480,OF_PIXELS_BGR);
        //void setFromPixels(const PixelType * newPixels,size_t w, size_t h, ofPixelFormat pixelFormat);

        newFrame(p);
//	}
}

void VideoGrabberPS3Eye::newFrame(ofPixels & pixels){
	frame = VideoFrame::newVideoFrame(pixels);
	//frame.getTextureRef();
	newFrameEvent.notify(this,frame);
//    frameOut = frame;
    
    parameters->get("Frame Output").cast<ofxPm::VideoFrame>() = frame;
}

//------------------------------------------------------
float VideoGrabberPS3Eye::getFps(){
	return fps;
}
	
//------------------------------------------------------
void VideoGrabberPS3Eye::setFps(float fps){
	this->fps = fps;
}

	
}
