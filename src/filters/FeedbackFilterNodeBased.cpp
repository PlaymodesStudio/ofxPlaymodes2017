/*
 * VideoMixerGPU.cpp
 *
 *  Created on: Jan 18, 2013
 *      Author: arturo
 */

#include "FeedbackFilterNodeBased.h"
#include "parametersControl.h"


namespace ofxPm{
FeedbackFilterNodeBased::FeedbackFilterNodeBased()
//:source(0),
//newFrame(false)
{
	// TODO Auto-generated constructor stub

}

FeedbackFilterNodeBased::~FeedbackFilterNodeBased() {
	// TODO Auto-generated destructor stub
}

    void FeedbackFilterNodeBased::setup(VideoSource & _source){
        
        
        source = &_source;
        fps = _source.getFps();
        isNodeBased = false;
        
        frame = VideoFrame::newVideoFrame(source->getNextVideoFrame());
        ofAddListener(source->newFrameEvent,this,&FeedbackFilterNodeBased::newVideoFrame);
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

    void FeedbackFilterNodeBased::setupNodeBased(){
        
        
        source = NULL;
        fps = -1;
        isNodeBased = true;
        
        frame = VideoFrame();
        ofFbo fboAux;
        fboAux.allocate(640,480);
        frame = VideoFrame::newVideoFrame(fboAux);
        //ofAddListener(source->newFrameEvent,this,&GradientEdgesFilter::newVideoFrame);
        
        shader.load("shaders/feedback");
        
        if (true)
        {
            lastFrameRendered = ofxPm::VideoFrame();
        }
        
        // param nodes
                
        parameters = new ofParameterGroup();
        parameters->setName("Feedback");
        parameters->add(paramScale.set("Scale",0.98,-1.0,2.0));
        parameters->add(paramMixAmmount.set("Mix Ammount",0.1,0.0,1.0));
        parameters->add(paramFrameIn.set("Frame Input", frame));
        parameters->add(paramFrameOut.set("Frame Output", frame));
        
        parametersControl::getInstance().createGuiFromParams(parameters,ofColor::purple);
        
//        paramGradientWidth.addListener(this, &FeedbackFilterNodeBased::setGradientWidth);
//        paramGradientXorY.addListener(this, &FeedbackFilterNodeBased::setGradientXorY);
        
        paramFrameIn.addListener(this, &FeedbackFilterNodeBased::newVideoFrame);
    }

VideoFrame FeedbackFilterNodeBased::getNextVideoFrame()
{
    
//    if(source->getNextVideoFrame()!=NULL)
//    {
//        return source->getNextVideoFrame();
//    }
//    cout << " fx : getNextVF"  << endl;
    return frame;

}

    void FeedbackFilterNodeBased::newVideoFrame(VideoFrame & _frame)
    {
//	//front = VideoFrame::newVideoFrame(frame);
//
//    VideoFrame f = source->getNextVideoFrame() ;
//	if(f!=NULL)
//    {
//        frame = f;
//    }
//
//
    if (!fbo.isAllocated())
    {
        fbo.allocate(_frame.getWidth(), _frame.getHeight());
    }
    else{
        fbo.begin();
        {
            ofClear(0, 0, 0, 255);
            shader.begin();
            {
                shader.setUniformTexture("tex0",_frame.getTextureRef(),0);
                shader.setUniformTexture("tex1",frame.getTextureRef(),1);
                shader.setUniform1f("u_scale",paramScale);
                shader.setUniform1f("u_mixAmmount",paramMixAmmount);
                float* res = new float[2];
                res[0]=640;
                res[1]=480;
                shader.setUniform2fv("u_resolution",res);
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
        
        
        if(!isNodeBased) ofNotifyEvent(newFrameEvent,frame);
        else parameters->get("Frame Output").cast<ofxPm::VideoFrame>() = frame;
        
    }
    
    
}

}
