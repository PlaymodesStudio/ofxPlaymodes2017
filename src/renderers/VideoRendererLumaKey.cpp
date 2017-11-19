/*
 * VideoRenderer.cpp
 *
 *  Created on: 09-oct-2008
 *      Author: arturo castro
 */

#include "VideoRendererLumaKey.h"



namespace ofxPm{
VideoRendererLumaKey::VideoRendererLumaKey(){
    source=NULL;
	minmaxBlend=false;
	shaderActive=false;
    lumaThreshold=0.25;
    lumaSmooth=0.25;
}

VideoRendererLumaKey::VideoRendererLumaKey(VideoSource & source) {
	setup(source);
}

VideoRendererLumaKey::~VideoRendererLumaKey() {
}


void VideoRendererLumaKey::setup(VideoSource & source){
    this->source=&source;

	minmaxBlend=false;
	shaderActive=true;
    lumaThreshold=0.25;
    lumaSmooth=0.25;
    
    shader.load("shaders/lumakey");

}

//void VideoRendererLumaKey::draw(){
//	if(shaderActive)
//		shader.begin();
//	if(tint.a<255){
//		ofEnableAlphaBlending();
//
//		if(minmaxBlend){
//			glBlendEquationEXT(GL_MAX);
//
//			//glBlendFuncSeparateEXT( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_DST_ALPHA );
//			//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
//			//glBlendEquationSeparateEXT(GL_MAX,GL_ADD);
//		}else{
//			glBlendEquationEXT(GL_MIN);
//
//
//			//glBlendFuncSeparateEXT( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_DST_ALPHA );
//			//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
//			//glBlendEquationSeparateEXT(GL_MIN,GL_ADD);
//		}
//		ofSetColor(tint);
//		/// drawing the video render
//		drawNextFrame();
//		glBlendEquationEXT(GL_FUNC_ADD);
//		ofDisableAlphaBlending();
//	}else{
//		ofSetColor(tint);
//		drawNextFrame();
//	}
//	if(shaderActive)
//		shader.end();
//
//}

//    void VideoRendererLumaKey::draw(int x,int y,int w,int h){
//        if(tint.a<255){
//            ofEnableAlphaBlending();
//            
//            if(minmaxBlend){
//                glBlendEquationEXT(GL_MAX);
//                
//                //glBlendFuncSeparateEXT( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_DST_ALPHA );
//                //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
//                //glBlendEquationSeparateEXT(GL_MAX,GL_ADD);
//            }else{
//                glBlendEquationEXT(GL_MIN);
//                
//                
//                //glBlendFuncSeparateEXT( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_DST_ALPHA );
//                //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
//                //glBlendEquationSeparateEXT(GL_MIN,GL_ADD);
//            }
//            ofSetColor(tint);
//            /// drawing the video render
//            drawNextFrame(x,y,w,h);
//            glBlendEquationEXT(GL_FUNC_ADD);
//            ofDisableAlphaBlending();
//        }else{
//            ofSetColor(tint);
//            drawNextFrame();
//        }
//        
//    }

    
//void VideoRendererLumaKey::drawNextFrame()
//{
//    drawNextFrame(0,0,source->getWidth(),source->getHeight());
//}

void VideoRendererLumaKey::drawNextFrame(int x,int y,int w,int h)
{
    
    VideoFrame frame = source->getNextVideoFrame();
    
    if(shaderActive)
    {
        shader.begin();
    }
    
    if(frame!=NULL)
    {
        ofEnableAlphaBlending();

        if(minmaxBlend){
            glBlendEquationEXT(GL_MAX);
        }else{
            glBlendEquationEXT(GL_MIN);
        }
//        ofSetColor(tint);
//
//
        shader.setUniformTexture("tex0", frame.getTextureRef(), 0);
        shader.setUniform1f("u_smooth",lumaSmooth);
        shader.setUniform1f("u_max",lumaThreshold);
        
        frame.getTextureRef().draw(x,y,w,h);
    }
    else
    {
        cout << "VRLKey :: Frame is Null !!" << endl;
    }
    
    if(shaderActive)
    {
        shader.end();
    }
    
}
    
    
ofShader VideoRendererLumaKey::getShader() const
{
    return shader;
}

ofColor VideoRendererLumaKey::getTint() const
{
    return tint;
}

bool VideoRendererLumaKey::isMinmaxBlend() const
{
    return minmaxBlend;
}

bool VideoRendererLumaKey::isShaderActive() const
{
    return shaderActive;
}

void VideoRendererLumaKey::setMinmaxBlend(bool b)
{
    this->minmaxBlend = b;
}

void VideoRendererLumaKey::setShader(ofShader shader)
{
    this->shader = shader;
}

void VideoRendererLumaKey::setShaderActive(bool shaderActive)
{
    this->shaderActive = shaderActive;
}

void VideoRendererLumaKey::setTint(ofColor tint)
{
    this->tint = tint;
}


}



