/*
 * VideoRenderer.cpp
 *
 *  Created on: 09-oct-2008
 *      Author: arturo castro
 */

#include "VideoTrioRenderer.h"


namespace ofxPm
{
    VideoTrioRenderer::VideoTrioRenderer(){
    source=NULL;
}

//--------------------------------------------------------------
VideoTrioRenderer::VideoTrioRenderer(VideoSource & source)
{
	setup(source);
}

//--------------------------------------------------------------
VideoTrioRenderer::~VideoTrioRenderer() {
}


//--------------------------------------------------------------
void VideoTrioRenderer::setup(VideoSource & source){
    this->source=&source;
}


//--------------------------------------------------------------
void VideoTrioRenderer::draw()
{
    drawNextFrame(0,0,source->getWidth(),source->getHeight());
}


//--------------------------------------------------------------
void VideoTrioRenderer::draw(int x,int y,int w,int h)
{
    drawNextFrame(x,y,w,h);
}

    
//--------------------------------------------------------------
void VideoTrioRenderer::drawNextFrame(int x,int y,int w,int h)
{
    ofSetRectMode(OF_RECTMODE_CENTER);

    VideoFrame frame = source->getNextVideoFrame();
    
    if(frame!=NULL)
    {
        ofVec2f frameResolution = ofVec2f(frame.getWidth(),frame.getHeight());
        float frameAspectRatio = frameResolution.x / frameResolution.y;

        ofPushMatrix();
        
        ofTranslate(frameResolution.y/2.0,frameResolution.x/2.0,0);
        ofRotate(90,0,0, 1);
        //ofTranslate(-frameResolution.x/2.0,-frameResolution.y/2.0,0);
        
        frame.getTextureRef().draw(0,0,frame.getWidth(),frame.getHeight());
        
        ofPopMatrix();
    }

    ofSetColor(255,128,0);
    ofDrawCircle(0,0,10);
    ofSetRectMode(OF_RECTMODE_CORNER);

}

    
    
    



}



