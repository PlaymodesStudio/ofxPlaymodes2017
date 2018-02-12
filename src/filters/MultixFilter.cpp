/*
 * AudioFrame.h
 *
 *  Created on: 09-oct-2008
 *      Author: arturo castro
 */

#include "MultixFilter.h"
namespace ofxPm{


MultixFilter::MultixFilter(VideoBuffer & _buffer, int numHeaders)
{
	setup(_buffer,numHeaders);
}

MultixFilter::~MultixFilter()
{
    //dtor
}

MultixFilter::MultixFilter(){
	minmaxBlend=0;
    opacityMode=0;
	numHeaders=0;
}

void MultixFilter::setup(VideoBuffer & _buffer, int _numHeaders){

    videoBuffer=&_buffer;
    numHeaders=_numHeaders;

    videoHeader.resize(numHeaders);
    videoRenderer.resize(numHeaders);
    for (int i=0;i<numHeaders;i++){
        videoHeader[i].setup(*videoBuffer);
        videoRenderer[i].setup(videoHeader[i]);
        if(i==0)
        {
            videoHeader[i].setDelayMs(1);
        }
        else videoHeader[i].setDelayMs(-1);

    }
    
    // RENDERING
    
    minmaxBlend=false;
    
    
    // get initial video frame
    VideoFrame v = videoBuffer->getNextVideoFrame();
    if(!v.isNull())
    {
        cout << "Multix..." << endl;
        frame = VideoFrame::newVideoFrame(videoBuffer->getNextVideoFrame());
    }
    ofAddListener(videoBuffer->newFrameEvent,this,&MultixFilter::newVideoFrame);

    
    // allocate fbo where to draw
    if(videoBuffer->getWidth()>0) fbo.allocate(videoBuffer->getWidth(),videoBuffer->getHeight(),GL_RGBA);
    else
    {
        cout << "MultixFilter !! WARNING : Fbo setup with default size !!" << endl;
        fbo.allocate(640,480,GL_RGBA);
    }


}

void MultixFilter::setNumHeaders(int _numHeaders){
    this->numHeaders=_numHeaders;
}

int MultixFilter::getNumHeaders(){
    return videoHeader.size();
}


//--------------------------------------------------------
void MultixFilter::updateValuesMs(vector<float> _vf)
{
    // CLEAN headers and renderers and resize the to new
    numHeaders = _vf.size();
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
void MultixFilter::updateValuesPct(vector<float> _vf)
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
VideoFrame MultixFilter::getNextVideoFrame()
{
    
    //    if(source->getNextVideoFrame()!=NULL)
    //    {
    //        return source->getNextVideoFrame();
    //    }
    //newVideoFrame(frame);
    return frame;
    
}

//--------------------------------------------------------
void MultixFilter::newVideoFrame(VideoFrame & _frame)
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

    fbo.begin();
    {
        ofClear(0,0,0,255);
        ofSetColor(255);
        //ofDrawRectangle(0,0,fbo.getWidth()-ofGetMouseX(),fbo.getHeight());
        //_frame.getTextureRef().draw(0,0,fbo.getWidth(),fbo.getHeight());
        //videoRenderer[1].draw(ofGetMouseX(),ofGetMouseY(),fbo.getWidth(),fbo.getHeight());
        drawIntoFbo(0,0,fbo.getWidth(),fbo.getHeight());
        
    }
    fbo.end();
    

    frame = VideoFrame::newVideoFrame(fbo);
    ofNotifyEvent(newFrameEvent,frame);
    
}

//--------------------------------------------------------
void MultixFilter::drawIntoFbo(int x, int y,int w, int h)
{
    // TO DO : needed ?
    //ofEnableAlphaBlending();

    if(minmaxBlend){
        glBlendEquationEXT(GL_MAX);
    }else{
        glBlendEquationEXT(GL_MIN);
    }

    // As we've
    int headersInAction = 0;
    float oneFrameMs	= 1000.0 / videoBuffer->getFps();
    float totalBufferSizeInMs = videoBuffer->getMaxSize() * oneFrameMs;

    float opac = 1.0;
	for(int i = videoHeader.size()-1; i>=0; i--)
    {
        // if delay time of each videoRenderer is in the right range of Ms (0..TotalMs)
        switch(opacityMode)
        {
            case 0 :
                opac = 1.0;
                break;
            case 1 :
                opac = (1.0/videoHeader.size()) * (videoHeader.size()-i);
                break;
            case 2 :
                opac = 1.0-(1.0/videoHeader.size()) * (videoHeader.size()-i-1);
                break;

        }
        //cout << "MultixFilter :: i : " << i << " Opac[0..1] : " << opac << endl;

        if((videoHeader[i].getDelayMs()>=0)&&(videoHeader[i].getDelayMs() < totalBufferSizeInMs))
        {
            headersInAction++;
            ofSetColor((opac*255.0),255);
            // or the opposite order size-i-1 ? or just "i"
            videoRenderer[i].draw(x,y,w,h);
        }
	}
    //cout << "MultixFilter :: Headers in Action : " << headersInAction << endl;
    ofDisableAlphaBlending();

    
//    // Draw Multix Shit
////    int copyWidth = 640.0 / float(videoBuffer->getMaxSize());
//    int copyWidth = 640.0 / float(videoHeader.size());
//    for(int i=0;i<videoHeader.size();i++)
//    {
//        ofVec2f origin = ofVec2f(copyWidth*i+5,500);
//        ofSetColor(255,0,0);
//        ofDrawRectangle(ofPoint(origin), (copyWidth), 100.0 * (videoHeader[i].getDelayMs()/1000.0));
//    }
//    ofSetColor(255,0,0);
//    //float oneFrameMs = (1.0 / 60.0) * 1000.0;
//    float totalLengthInMs = videoBuffer->getMaxSize() * oneFrameMs;
//    int maxHeight = 100 * (totalLengthInMs/1000.0);
//    ofDrawLine(0,500 + maxHeight , 640, 500 + maxHeight);
    
    // TO DO : needed ?
    glBlendEquationEXT(GL_ADD);
}


bool MultixFilter::isMinmaxBlend() const
{
    return minmaxBlend;
}


void MultixFilter::setMinmaxBlend(bool b)
{
    this->minmaxBlend = b;
}


VideoHeader * MultixFilter::getHeader(int header){
	return &videoHeader[header];
}

VideoRenderer * MultixFilter::getRenderer(int renderer){
	return &videoRenderer[renderer];
}
 
    
    
}


