/*
 * VideoRenderer.cpp
 *
 *  Created on: 09-oct-2008
 *      Author: arturo castro
 */

#include "VideoRenderer.h"


namespace ofxPm
{
    VideoRenderer::VideoRenderer(){
    source=NULL;
}

//--------------------------------------------------------------
VideoRenderer::VideoRenderer(VideoSource & source)
{
	setup(source);
}

//--------------------------------------------------------------
VideoRenderer::~VideoRenderer() {
}


//--------------------------------------------------------------
void VideoRenderer::setup(VideoSource & source){
    this->source=&source;
}


//--------------------------------------------------------------
void VideoRenderer::draw()
{
    drawNextFrame(0,0,source->getWidth(),source->getHeight());
}


//--------------------------------------------------------------
void VideoRenderer::draw(int x,int y,int w,int h)
{
    drawNextFrame(x,y,w,h);
}

    
//--------------------------------------------------------------
void VideoRenderer::drawNextFrame(int x,int y,int w,int h)
{
    VideoFrame frame = source->getNextVideoFrame();
    if(frame!=NULL){
        frame.getTextureRef().draw(x,y,w,h);
    }
    
}

    
    
    



}



