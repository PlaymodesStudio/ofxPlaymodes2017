/*
 * AudioFrame.h
 *
 *  Created on: 09-oct-2008
 *      Author: arturo castro
 */

#include "MultixRenderer.h"
namespace ofxPm{


MultixRenderer::MultixRenderer(VideoBuffer & buffer, int numHeaders)
{
	setup(buffer,numHeaders);
}

MultixRenderer::~MultixRenderer()
{
    //dtor
}

MultixRenderer::MultixRenderer(){
	minmaxBlend=0;


	numHeaders=0;
}

void MultixRenderer::setup(VideoBuffer & buffer, int _numHeaders){

    videoBuffer=&buffer;
    numHeaders=_numHeaders;

    videoHeader.resize(numHeaders);
    videoRenderer.resize(numHeaders);
    for (int i=0;i<numHeaders;i++){
        videoHeader[i].setup(buffer);
        videoRenderer[i].setup(videoHeader[i]);
        if(i==0)
        {
            videoHeader[i].setDelayMs(1);
        }
        else videoHeader[i].setDelayMs(-1);

    }
    
    minmaxBlend=false;

}

void MultixRenderer::setNumHeaders(int _numHeaders){
    this->numHeaders=_numHeaders;
}

int MultixRenderer::getNumHeaders(){
    return videoHeader.size();
}


//--------------------------------------------------------
void MultixRenderer::updateValuesMs(vector<float> _vf)
{
    // CLEAN headers and renderers and resize the to new
    numHeaders = _vf.size();
    //cout << "MultixRender::updateValuesMs" << endl;
    int currNumHeaders = videoHeader.size();
    if(currNumHeaders!=numHeaders){
        videoHeader.resize(numHeaders);
        videoRenderer.resize(numHeaders);
        for(int i=0;i<numHeaders;i++)
        {
            videoHeader[i].setup(*videoBuffer);
            videoRenderer[i].setup(videoHeader[i]);
        }
        cout << "Multix::Renderer::Update Ms WARNING : currNumHeaders!=numHeaders" << endl;
    }
    
    // update the delayMs for each 
    for(int i=_vf.size()-1; i>=0; i--)
    {
        videoHeader[i].setDelayMs(_vf[i]);
    }
    videoHeader[0].setDelayMs(0);

}
    
//--------------------------------------------------------
void MultixRenderer::updateValuesPct(vector<float> _vf)
{
//    int currNumHeaders = videoHeader.size();
//    if(currNumHeaders!=numHeaders){
//        videoHeader.resize(numHeaders);
//        videoRenderer.resize(numHeaders);
//        for(int i=0;i<numHeaders;i++){
//            videoHeader[i].setup(*videoBuffer);
//            videoRenderer[i].setup(videoHeader[i]);
//        }
//        cout << "Multix::Renderer::Update Pct WARNING : currNumHeaders!=numHeaders" << endl;
//    }
//    
//    for(int i=videoRenderer.size()-1; i>=0; i--)
//    {
//        //videoHeader[i].setDelayMs(delayOffset*1000*i);
//        
//        float oneFrameMs = (1.0 / 60.0) * 1000.0;
//        float totalLengthInMs = videoBuffer->getMaxSize() * oneFrameMs;
//        
//        float myDelayMs = ofMap(_vf[i],0.0,1.0,0.0,delayOffset*100*videoBuffer->getMaxSize());
//        //float myDelayMs = delayOffset*100*i;
//        
//        
//        videoHeader[i].setDelayMs(myDelayMs);
//        
//        // cout << "Updating Values VF [" << i << "]" << " : " << _vf[i] << " VALUE : " << myDelayMs << " Total Length in Ms :" << totalLengthInMs << endl;
//        
//        
//        //cout << i << " : " << myDelayMs << ",";
//        //        videoRenderer[i].setTint(tint);
//        //        videoRenderer[i].setMinmaxBlend(minmaxBlend);
//    }
}

//--------------------------------------------------------
void MultixRenderer::draw(int x, int y,int w, int h)
{

    ofEnableAlphaBlending();
    
    if(minmaxBlend){
        glBlendEquationEXT(GL_MAX);
    }else{
        glBlendEquationEXT(GL_MIN);
    }

    // As we've
    int headersInAction = 0;
    float oneFrameMs	= 1000.0 / videoBuffer->getFps();
    float totalBufferSizeInMs = videoBuffer->getMaxSize() * oneFrameMs;

	for(int i = videoHeader.size()-1; i>=0; i--)
    {
        // if delay time of each videoRenderer is in the right range of Ms (0..TotalMs)
        if((videoHeader[i].getDelayMs()>=0)&&(videoHeader[i].getDelayMs() < totalBufferSizeInMs))
        {
            headersInAction++;
            videoRenderer[i].draw(x,y,w,h);
        }
	}
    cout << "MultixRenderer::HeadersInAction = " << headersInAction << " Buff FPS : " << videoBuffer->getFps() << endl;
    
    ofDisableAlphaBlending();
    

    // Draw Multix Shit
//    int copyWidth = 640.0 / float(videoBuffer->getMaxSize());
    int copyWidth = 640.0 / float(videoHeader.size());
    for(int i=0;i<videoHeader.size();i++)
    {
        ofVec2f origin = ofVec2f(copyWidth*i+5,500);
        ofSetColor(255,0,0);
        ofDrawRectangle(ofPoint(origin), (copyWidth), 100.0 * (videoHeader[i].getDelayMs()/1000.0));
    }
    ofSetColor(255,0,0);
    //float oneFrameMs = (1.0 / 60.0) * 1000.0;
    float totalLengthInMs = videoBuffer->getMaxSize() * oneFrameMs;
    int maxHeight = 100 * (totalLengthInMs/1000.0);
    ofDrawLine(0,500 + maxHeight , 640, 500 + maxHeight);
    ofSetColor(255);
}


VideoBuffer *MultixRenderer::getVideoBuffer() const
{
    return videoBuffer;
}

bool MultixRenderer::isMinmaxBlend() const
{
    return minmaxBlend;
}


void MultixRenderer::setMinmaxBlend(bool b)
{
    this->minmaxBlend = b;
}

void MultixRenderer::setVideoBuffer(VideoBuffer *videoBuffer)
{
    this->videoBuffer = videoBuffer;
}

VideoHeader * MultixRenderer::getHeader(int header){
	return &videoHeader[header];
}

VideoRenderer * MultixRenderer::getRenderer(int renderer){
	return &videoRenderer[renderer];
}
    
}

