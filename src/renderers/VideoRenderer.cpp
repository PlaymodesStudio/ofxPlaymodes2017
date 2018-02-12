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
    lastFrame = source->getNextVideoFrame();
    if(!lastFrame.isNull()){
        lastFrame.getTextureRef().draw(x,y,w,h);
    }
    
}
    
//--------------------------------------------------------------
ofTexture VideoRenderer::getLastFrameTexture()
{
    if(!lastFrame.isNull())
    {
        if(lastFrame.getTextureRef().isAllocated())
        {
            return lastFrame.getTextureRef();
        }
        else
        {
            cout << "VideoRenderer :: getLastFrame texture not allocated !!" << endl;
        }
    }
}

    
    
    



}



