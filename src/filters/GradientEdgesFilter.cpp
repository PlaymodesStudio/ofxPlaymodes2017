/*
 * VideoMixerGPU.cpp
 *
 *  Created on: Jan 18, 2013
 *      Author: arturo
 */

#include "GradientEdgesFilter.h"

namespace ofxPm{
GradientEdgesFilter::GradientEdgesFilter()
//:source(0),
//newFrame(false)
{
	// TODO Auto-generated constructor stub

}

GradientEdgesFilter::~GradientEdgesFilter() {
	// TODO Auto-generated destructor stub
}

void GradientEdgesFilter::setup(VideoSource & _source){

    gradientWidth = 0.25;
    gradientXorY = 1.0;
    
    source = &_source;
    fps = _source.getFps();
    
	frame = VideoFrame::newVideoFrame(source->getNextVideoFrame());
	ofAddListener(source->newFrameEvent,this,&GradientEdgesFilter::newVideoFrame);
	//shader.setupShaderFromSource(GL_FRAGMENT_SHADER,fragmentTestSrc);
    shader.load("shaders/gradient");
     
	//shader.linkProgram();
    // allocate fbo where to draw
    if(source->getWidth()>0) fbo.allocate(source->getWidth(),source->getHeight(),GL_RGBA);
    else
    {
        cout << "VideoTestShaderFX !! WARNING : Fbo setup with default size !!" << endl;
        fbo.allocate(frame.getWidth(),frame.getHeight(),GL_RGBA);
    }
//    plane.set(source->getWidth(),source->getHeight(), 10, 10);
//    plane.mapTexCoords(0, 0,source->getWidth(),source->getHeight());
    
    cout <<"FX Source W : "  << source->getWidth() << " H : " << source->getHeight() << endl;
}

VideoFrame GradientEdgesFilter::getNextVideoFrame(){
    
//    if(source->getNextVideoFrame()!=NULL)
//    {
//        return source->getNextVideoFrame();
//    }
//    cout << " fx : getNextVF"  << endl;
    return frame;

}

void GradientEdgesFilter::newVideoFrame(VideoFrame & _frame){
//	//front = VideoFrame::newVideoFrame(frame);
//
    
//    VideoFrame f = source->getNextVideoFrame() ;
//	if(f!=NULL)
//    {
//        frame = f;
//    }
//
//
	fbo.begin();
    {
        ofClear(0, 0, 0, 255);
        shader.begin();
        {
            shader.setUniformTexture("tex0",_frame.getTextureRef(),0);
            shader.setUniform1f("u_width",gradientWidth);
            shader.setUniform1i("u_xory",gradientXorY);
            
            ofSetColor(255);
        //    frame.getTextureRef().bind();
        //	ofDrawRectangle(0,0,frame.getWidth(),frame.getHeight());
//            plane.draw();
            //image.getTexture().draw(0, 0,640,480);
            _frame.getTextureRef().draw(0,0,frame.getWidth(),frame.getHeight());
        }
        shader.end();
    }
    fbo.end();

	frame = VideoFrame::newVideoFrame(fbo);
//
	ofNotifyEvent(newFrameEvent,frame);
    
//    front = frame;
    
}

}
