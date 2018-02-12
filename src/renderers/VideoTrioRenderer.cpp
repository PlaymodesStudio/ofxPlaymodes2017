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
    
    if(!frame.isNull())
    {
        ofVec2f frameResolution = ofVec2f(frame.getWidth(),frame.getHeight());
        float frameAspectRatio = frameResolution.x / frameResolution.y;

        cout << frameResolution << endl;
        
        if(true){
            glBlendEquationEXT(GL_MAX);
        }else{
            glBlendEquationEXT(GL_MIN);
        }

        
        ofPushMatrix();
        
        ofTranslate(frameResolution.y/2.0,frameResolution.x/2.0,0);
        // rotate to portrait
        ofRotate(90,0,0, 1);
        // flip x
        ofRotate(180,1,0,0);
        //ofTranslate(-frameResolution.x/2.0,-frameResolution.y/2.0,0);
        
        float overlap = 140;
        
        frame.getTextureRef().draw(0,0,frame.getWidth(),frame.getHeight());
        frame.getTextureRef().draw(0,1*frame.getHeight()-(1*overlap),frame.getWidth(),frame.getHeight());
        frame.getTextureRef().draw(0,2*frame.getHeight()-(2*overlap),frame.getWidth(),frame.getHeight());
        
        ofPopMatrix();
    }

    ofSetColor(255,128,0);
    ofDrawCircle(0,0,10);
    ofSetRectMode(OF_RECTMODE_CORNER);
    
    // TO DO : needed ?
    glBlendEquationEXT(GL_ADD);


}

//    lastFrame = source->getNextVideoFrame();
//    if(lastFrame!=NULL){
//        lastFrame.getTextureRef().draw(x,y,w,h);
//    }
    
    



}



