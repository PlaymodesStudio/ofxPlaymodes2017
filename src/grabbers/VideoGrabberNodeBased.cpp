/*
 * AudioFrame.h
 *
 *  Created on: 09-oct-2008
 *      Author: arturo castro
 */

#include "VideoGrabberNodeBased.h"
#include "parametersControl.h"

namespace ofxPm{
VideoGrabberNodeBased::VideoGrabberNodeBased(){
}

VideoGrabberNodeBased::~VideoGrabberNodeBased(){
}

bool VideoGrabberNodeBased::initGrabber(int w, int h){
    
    // parametersGroup
    numGrabberDevices = VideoGrabberNodeBased::listDevices().size();

    
    parameters = new ofParameterGroup();
    parameters->setName("VideoGrabber");
    parameters->add(paramGrab.set("Grab", false));
    parameters->add(paramSettings.set("Settings", true));
    parameters->add(paramDeviceId.set("DeviceId",0,0,numGrabberDevices));
    parameters->add(paramResolutionX.set("ResolutionX",640,0,1920));
    parameters->add(paramResolutionY.set("ResolutionY",480,0,1080));
    parameters->add(paramReconnect.set("Reconnect",true));
    parameters->add(paramFrameOut.set("Frame Output", frame));
    
    parametersControl::getInstance().createGuiFromParams(parameters,ofColor::orange);
    
    paramSettings.addListener(this, &VideoGrabberNodeBased::setGrabberSettings);
    paramReconnect.addListener(this, &VideoGrabberNodeBased::setReconnect);


    // GRABBER ORIGINAL CODE
	bool ret = ofVideoGrabber::initGrabber(w,h,true);
	frame    = VideoFrame::newVideoFrame(getPixelsRef());
	VideoSource::width    = ofVideoGrabber::getWidth();
	VideoSource::height   = ofVideoGrabber::getHeight();
	
	printf("VideoGrabber::initGrabber %d %d\n",VideoSource::width,VideoSource::height);
	return ret;
}

VideoFrame VideoGrabberNodeBased::getNextVideoFrame(){
    return frame;
}

    void VideoGrabberNodeBased::update()
    {
        if(paramGrab)
        {
            ofVideoGrabber::update();
            if(ofVideoGrabber::isFrameNew())
            {
                newFrame(getPixelsRef());
            }
        }
    }

void VideoGrabberNodeBased::newFrame(ofPixels & pixels){
	frame = VideoFrame::newVideoFrame(pixels);
	//frame.getTextureRef();
	newFrameEvent.notify(this,frame);
    
    parameters->get("Frame Output").cast<ofxPm::VideoFrame>() = frame;
    
}

//------------------------------------------------------
float VideoGrabberNodeBased::getFps(){
	return fps;
}
	
void VideoGrabberNodeBased::setFps(float fps){
	this->fps = fps;
}

    //------------------------------------------------------
    void VideoGrabberNodeBased::setGrabberSettings(bool & _b)
    {
        ofVideoGrabber::videoSettings();
        
    }
    //------------------------------------------------------
    void VideoGrabberNodeBased::setReconnect(bool & _b)
    {
        ofVideoGrabber::close();
        ofVideoGrabber::listDevices();
        ofVideoGrabber::setDeviceID(paramDeviceId);
        ofVideoGrabber::setup(paramResolutionX,paramResolutionY);
        
    }
    
    


    
}
