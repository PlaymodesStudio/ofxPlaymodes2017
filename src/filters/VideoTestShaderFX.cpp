/*
 * VideoMixerGPU.cpp
 *
 *  Created on: Jan 18, 2013
 *      Author: arturo
 */

#include "VideoTestShaderFX.h"

#include "parametersControl.h"


//#define STRINGIFY(...) #__VA_ARGS__
//static string fragmentTestSrc =
//#ifdef TARGET_LINUX
//"#version 140\n"
//"#extension GL_ARB_texture_rectangle : enable\n"
//#endif
//
////static string fragmentTestSrc =
////
////        STRINGIFY(
////                  "#version 140\n"
////                  
////                  uniform sampler2D tex0;
////
////                  void main (void){
////                      vec2 pos = gl_FragCoord.xy;
////                      vec4 color = vec4(texture2DRect(tex0, pos));
////
////                      gl_FragColor = vec4(1.0,0.0,0.0,1.0);
////                  }
////        );
////

namespace ofxPm{
VideoTestShaderFX::VideoTestShaderFX()
//:source(0),
//newFrame(false)
{
	// TODO Auto-generated constructor stub

}

VideoTestShaderFX::~VideoTestShaderFX() {
	// TODO Auto-generated destructor stub
}

void VideoTestShaderFX::setup(VideoSource & _source){

    lumaSmooth=0.25;
    lumaThreshold=0.025;
    source = &_source;
    fps = _source.getFps();
    isNodeBased = false;
    
	frame = VideoFrame::newVideoFrame(source->getNextVideoFrame());
	ofAddListener(source->newFrameEvent,this,&VideoTestShaderFX::newVideoFrame);
	//shader.setupShaderFromSource(GL_FRAGMENT_SHADER,fragmentTestSrc);
    shader.load("shaders/lumakey");
     
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

    void VideoTestShaderFX::setupNodeBased()
    {
        lumaSmooth=0.25;
        lumaThreshold=0.025;
        source = NULL;
        fps = -1;
        isNodeBased = true;
        
        shader.load("shaders/lumakey");
        
        // allocate fbo where to draw
        if (fbo.isAllocated()) fbo.allocate(source->getWidth(),source->getHeight(),GL_RGBA);
        
        // param nodes
        
        ofParameter<float>                  paramLumaThrshold;
        ofParameter<float>                  paramLumaSmooth;
        ofParameter<ofxPm::VideoFrame>      paramFrameIn;
        ofParameter<ofxPm::VideoFrame>      paramFrameOut;
        
        
        parameters = new ofParameterGroup();
        parameters->setName("Luma Key");
        parameters->add(paramLumaThrshold.set("Threshold",0.25,0.0,1.0));
        parameters->add(paramLumaSmooth.set("Smooth",0.25,0.0,1.0));
        parameters->add(paramFrameIn.set("Frame Input", frame));
        parameters->add(paramFrameOut.set("Frame Output", frame));
        
        parametersControl::getInstance().createGuiFromParams(parameters,ofColor::yellow);
        
        paramLumaThrshold.addListener(this, &VideoTestShaderFX::setLumaThreshold);
        paramLumaSmooth.addListener(this, &VideoTestShaderFX::setLumaSmooth);
        
        paramFrameIn.addListener(this, &VideoTestShaderFX::newVideoFrame);
    }

    
VideoFrame VideoTestShaderFX::getNextVideoFrame(){
    
//    if(source->getNextVideoFrame()!=NULL)
//    {
//        return source->getNextVideoFrame();
//    }
//    cout << " fx : getNextVF"  << endl;
    return frame;

}
//    void VideoGrabberPS3Eye::newFrame(ofPixels & pixels){
//        frame = VideoFrame::newVideoFrame(pixels);
//        //frame.getTextureRef();
//        newFrameEvent.notify(this,frame);
//        //    frameOut = frame;
//        
//        parameters->get("Frame Output").cast<ofxPm::VideoFrame>() = frame;
//    }
//    
void VideoTestShaderFX::newVideoFrame(VideoFrame & _frame){
//	//front = VideoFrame::newVideoFrame(frame);
//
    
//    VideoFrame f = source->getNextVideoFrame() ;
//	if(f!=NULL)
//    {
//        frame = f;
//    }
//
//
    if (!fbo.isAllocated()) fbo.allocate(_frame.getWidth(), _frame.getHeight());

	fbo.begin();
    {
        ofClear(0, 0, 0, 255);
        shader.begin();
        {
            shader.setUniformTexture("tex0",_frame.getTextureRef(),0);
            shader.setUniform1f("u_smooth",lumaSmooth);
            shader.setUniform1f("u_max",lumaThreshold);

            ofSetColor(255);
        //    frame.getTextureRef().bind();
        //	ofDrawRectangle(0,0,frame.getWidth(),frame.getHeight());
//            plane.draw();
            //image.getTexture().draw(0, 0,640,480);
            _frame.getTextureRef().draw(0,0,_frame.getWidth(),_frame.getHeight());
        }
        shader.end();
    }
    fbo.end();

	frame = VideoFrame::newVideoFrame(fbo);
//
	
    
//    front = frame;
  
    if(isNodeBased) parameters->get("Frame Output").cast<ofxPm::VideoFrame>() = frame;
    else ofNotifyEvent(newFrameEvent,frame);
}

}
