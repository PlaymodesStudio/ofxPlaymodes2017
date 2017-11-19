/*
 * VideoMixerGPU.cpp
 *
 *  Created on: Jan 18, 2013
 *      Author: arturo
 */

#include "VideoTestShaderFX.h"

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
        fbo.allocate(640,480,GL_RGBA);
    }
//    plane.set(source->getWidth(),source->getHeight(), 10, 10);
//    plane.mapTexCoords(0, 0,source->getWidth(),source->getHeight());
    
    cout <<"FX Source W : "  << source->getWidth() << " H : " << source->getHeight() << endl;
}

VideoFrame VideoTestShaderFX::getNextVideoFrame(){
    
//    if(source->getNextVideoFrame()!=NULL)
//    {
//        return source->getNextVideoFrame();
//    }
//    cout << " fx : getNextVF"  << endl;
    return frame;

}

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
            _frame.getTextureRef().draw(0,0,640,480);
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
