/*
 * VideoRenderer.cpp
 *
 *  Created on: 09-oct-2008
 *      Author: arturo castro
 */

#include "VideoRendererNodeBased.h"

#include "parametersControl.h"

namespace ofxPm
{
    VideoRendererNodeBased::VideoRendererNodeBased()
    {
        
    }


    //--------------------------------------------------------------
    VideoRendererNodeBased::~VideoRendererNodeBased()
    {
    
    }


    //--------------------------------------------------------------
    void VideoRendererNodeBased::setup()
    {

        vFrame = ofxPm::VideoFrame();
        
        parameters = new ofParameterGroup();
        parameters->setName("Video Renderer");
        parameters->add(paramFrameIn.set("Frame In", vFrame));

        parametersControl::getInstance().createGuiFromParams(parameters,ofColor::green);
        
    //    paramGradientXorY.addListener(this, &GradientEdgesFilter::setGradientXorY);
        paramFrameIn.addListener(this, &VideoRendererNodeBased::newVideoFrame);
    }
    
    //--------------------------------------------------------------

    void VideoRendererNodeBased::newVideoFrame(VideoFrame & frame)
    {
        vFrame = frame;
    }
    

    //--------------------------------------------------------------
    void VideoRendererNodeBased::draw(int x,int y,int w,int h)
    {
        if(!vFrame.isNull())
        {
            if(!vFrame.isNull())
            {
                if(vFrame.getTextureRef().isAllocated())
                {
                    vFrame.getTextureRef().draw(x,y,w,h);
                }
            }
        }
    }
    //--------------------------------------------------------------
    void VideoRendererNodeBased::draw()
    {
        if(!vFrame.isNull())
        {
            if(!vFrame.isNull())
            {
                if(vFrame.getTextureRef().isAllocated())
                {
                    vFrame.getTextureRef().draw(0,0,vFrame.getWidth(),vFrame.getHeight());
                }
            }
        }
    }
    //--------------------------------------------------------------
    void VideoRendererNodeBased::draw(int _x,int _y)
    {
        if(!vFrame.isNull())
        {
            if(!vFrame.isNull())
            {
                if(vFrame.getTextureRef().isAllocated())
                {
                    vFrame.getTextureRef().draw(_x,_y,vFrame.getWidth(),vFrame.getHeight());
                }
            }
        }
    }
    

//--------------------------------------------------------------
ofTexture VideoRendererNodeBased::getLastFrameTexture()
{
    if(!vFrame.isNull())
    {
        if(vFrame.getTextureRef().isAllocated())
        {
            return vFrame.getTextureRef();
        }
        else
        {
            cout << "VideoRenderer :: getLastFrame texture not allocated !!" << endl;
        }
    }
    return vFrame.getTextureRef();;
}

    
    
    



}



