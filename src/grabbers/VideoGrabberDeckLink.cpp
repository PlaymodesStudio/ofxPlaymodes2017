/*
 * AudioFrame.h
 *
 *  Created on: 09-oct-2008
 *      Author: arturo castro
 */

#include "VideoGrabberDeckLink.h"

namespace ofxPm{
VideoGrabberDeckLink::VideoGrabberDeckLink()
{
}
//------------------------------------------------------
VideoGrabberDeckLink::~VideoGrabberDeckLink()
{
}
//------------------------------------------------------
bool VideoGrabberDeckLink::initGrabber(int w, int h)
{
    bool ret;
    
    if (input.setup(0)) {
        input.start(bmdModeHD1080p6000);
        ret = true;
    }
    else
    {
        ret = false;
    }

    frame    = VideoFrame::newVideoFrame(getPixelsRef());
	VideoSource::width    = w;
	VideoSource::height   = h;
	
    fbo.allocate(w,h,GL_RGB);
    
	printf("VideoGrabberDeckLink::initGrabber %d %d\n",VideoSource::width,VideoSource::height);
	return ret;
}

//------------------------------------------------------
    VideoFrame VideoGrabberDeckLink::getNextVideoFrame(){
    return frame;
}

//------------------------------------------------------
    void VideoGrabberDeckLink::update()
{
//        ofVideoGrabber::update();
//        if(isFrameNew()){
//            newFrame(getPixelsRef());
//	}
    input.update();
    
    fbo.begin();
    {
        ofSetColor(255);
        input.draw(0,0,VideoSource::width,VideoSource::height);
    }
    fbo.end();
    
    fbo.readToPixels(pixels);
    
    newFrame(pixels);
}

void VideoGrabberDeckLink::newFrame(ofPixels & pixels){
	frame = VideoFrame::newVideoFrame(pixels);
	//frame.getTextureRef();
	newFrameEvent.notify(this,frame);
}

//------------------------------------------------------
float VideoGrabberDeckLink::getFps(){
	return fps;
}
	
//------------------------------------------------------
void VideoGrabberDeckLink::setFps(float fps){
	this->fps = fps;
}

	
}
