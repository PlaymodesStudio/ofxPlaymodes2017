 /*
 * AudioFrame.h
 *
 *  Created on: 09-oct-2008
 *      Author: arturo castro
 */

#include "VideoHeaderNodeBased.h"
#include "parametersControl.h"

namespace ofxPm{

//------------------------------------------------------
VideoHeaderNodeBased::VideoHeaderNodeBased(){
    fps         = 25.0;
	oneFrameMs	= 1000.0 / fps;
    inMs        = 0;
    outMs       = 0;
	lengthMs		= inMs - outMs;
	//buffer 		= NULL;
	lengthMs = 0;
	offsetFrames = 0.0;
	width = -11;
	height = -11;
    playing=false;
}


    //------------------------------------------------------
    void VideoHeaderNodeBased::setupNodeBased()
    {
//        //newFrameEvent.init("Playmodes.VideoHeader.newFrame");
//        this->buffer= &_buffer;
//        fps         = _buffer.getFps();
//        this->buffer->clear();
        oneFrameMs	= 1000.0 / fps;
//        totalBufferSizeInMs = _buffer.getSizeInFrames() * oneFrameMs;
        inMs        = totalBufferSizeInMs;
        outMs       = 0;
        lengthMs	= totalBufferSizeInMs;
        offsetFrames = 0.0;
        playing=false;
        
        parameters = new ofParameterGroup();
        parameters->setName("Video Header");
        parameters->add(paramVideoBufferInput.set("Buffer Input", nullptr));
        parameters->add(paramDelayMs.set("Delay Ms",0.0,0.0,3000.0));
        parameters->add(paramFrameOut.set("Frame Output",VideoFrame::VideoFrame()));
        
        parametersControl::getInstance().createGuiFromParams(parameters,ofColor::white);
        
        paramVideoBufferInput.addListener(this, &VideoHeaderNodeBased::setVideoBuffer);
        
        
//        VideoSource::width = _buffer.getWidth();
//        VideoSource::height = _buffer.getHeight();
//        
//        printf("VideoHeader::setup %d %d @ FPS %f\n",_buffer.getWidth(),_buffer.getHeight(),fps);
}

//------------------------------------------------------
VideoHeaderNodeBased::~VideoHeaderNodeBased(){
}

//------------------------------------------------------
void VideoHeaderNodeBased::draw(){
	
    // DRAWS THE BUFFER UI, NO TEXTURE AT ALL !!
    /*
	ofEnableAlphaBlending();
	
    double oneLength=(double)(ofGetWidth()-PMDRAWSPACING*2)/(double)(buffer->getMaxSize());
	int drawHeaderY = ofGetHeight() -140;
	int originXAtEnd = ofGetWidth() - PMDRAWSPACING;
	
    if(playing) ofSetColor(0,128,0,128);
    else ofSetColor(255,0,0,128);

    /// draw HEADER
    ////////////////
    int headerPosBasedOnFrameIndex = (buffer->getMaxSize() - currentFrameIndex)*oneLength;
    float headerPosBasedOnDelayMs = ((delayInMs/1000.0) / (buffer->getMaxSize()*(1.0/buffer->getFps())))*((double)(ofGetWidth()-PMDRAWSPACING*2));
    // Circle for header based on DelayInMs
    ofDrawCircle(originXAtEnd-headerPosBasedOnDelayMs,drawHeaderY,5);
    ofSetLineWidth(1);
    ofDrawLine(originXAtEnd-headerPosBasedOnFrameIndex+1,drawHeaderY,originXAtEnd-headerPosBasedOnFrameIndex,drawHeaderY+100);
    ofDrawRectangle(originXAtEnd-headerPosBasedOnFrameIndex-offsetFrames,drawHeaderY+50,oneLength,10);
	ofDrawBitmapString(ofToString(currentFrameIndex),ofPoint(originXAtEnd-headerPosBasedOnFrameIndex-offsetFrames+oneLength/2,drawHeaderY+45));
	
    // IN & OUT
    ////////////
	int inPos,outPos;
    
    // length of buffer in MicroSeconds passed to Int Ms
    int totalBuffLengthMs= int( (buffer->getFirstFrameTimestamp() - buffer->getLastFrameTimestamp())/1000) ;
    inPos = ofMap((inTS.raw()/1000)-int(buffer->getLastFrameTimestamp().raw()/1000),0,totalBuffLengthMs,PMDRAWSPACING,originXAtEnd);
    outPos = ofMap((outTS.raw()/1000)-int(buffer->getLastFrameTimestamp().raw()/1000),0,totalBuffLengthMs,PMDRAWSPACING,originXAtEnd);

	// draw in & out lines
	ofSetLineWidth(1.0);
	ofDrawLine(inPos,drawHeaderY,inPos,drawHeaderY+100);
    ofDrawLine(outPos,drawHeaderY,outPos,drawHeaderY+100);
	// draw in & out rectangle
	ofDrawRectangle(inPos,drawHeaderY,outPos-inPos,40);
	// draw inPos triangle
	ofBeginShape();
		ofVertex(inPos,drawHeaderY+10);
		ofVertex(inPos+5,drawHeaderY+5);
		ofVertex(inPos,drawHeaderY);
	ofEndShape();
	// draw outPos triangle
	ofBeginShape();
		ofVertex(outPos,drawHeaderY+10);
		ofVertex(outPos-5,drawHeaderY+5);
		ofVertex(outPos,drawHeaderY);
	ofEndShape();
	
	ofDrawBitmapString("<" + ofToString((inTS.raw()/1000000.0)),ofPoint(inPos+0,drawHeaderY+37));
	ofDrawBitmapString(ofToString((outTS.raw()/1000000.0)) + ">" ,ofPoint(outPos-30,drawHeaderY+20));
    
    ofSetColor(255,255,255);
	
	ofDisableAlphaBlending();
     */
}
//------------------------------------------------------
VideoFrame VideoHeaderNodeBased::getNextVideoFrame()
    {
        
    // frame to be returned;
    VideoFrame frame;
    
    paramVideoBufferInput.get()->lock();
    {
        // get the next frame timeStamp based on current behaviour
        currentFrameTs = getNextFrameTimestamp();
        // fetch closest video frame from buffer
        frame = paramVideoBufferInput.get()->getVideoFrame(currentFrameTs);
        // get the index of the fetched frame
        currentFrameIndex = frame.getBufferIndex();
    }
    paramVideoBufferInput.get()->unlock();
    return frame;
}

//------------------------------------------------------
Timestamp   VideoHeaderNodeBased::getNextFrameTimestamp()
{
    // to be returned
    Timestamp ts;
    
    // when buffer is not stopped we need to update the nowTS which is used to update TS of delay,inTS,outTS ...
    if(!paramVideoBufferInput.get()->isStopped())
    {
        nowTS = paramVideoBufferInput.get()->getFirstFrameTimestamp();
    }

    //update in and out TS based on "nowTS"
    inTS.update();
    outTS.update();
    
    inTS = nowTS - TimeDiff(inMs*1000);
    outTS = nowTS - TimeDiff(outMs*1000);
    
    // calculate the ts of the needed frame
    ts = nowTS - TimeDiff(paramDelayMs*1000);

    return ts;
}

//------------------------------------------------------
float VideoHeaderNodeBased::getFps(){
    return fps;
}
//------------------------------------------------------
void VideoHeaderNodeBased::setFps(float fps){
    this->fps=fps;
}
//------------------------------------------------------
/*VideoFrame VideoHeader::getVideoFrame(int index)
{
	buffer->lock();
		int indexFrame = CLAMP(index,0,buffer->size()-1);
		VideoFrame frame = buffer->getVideoFrame(indexFrame);
	buffer->unlock();
	return frame;
}
 */
//------------------------------------------------------
VideoBuffer* VideoHeaderNodeBased::getBuffer()
{
    return paramVideoBufferInput.get();
}

//------------------------------------------------------
void VideoHeaderNodeBased::setDelayMs(double _delayMs)
{
    paramDelayMs = _delayMs;
}


		
//------------------------------------------------------
string VideoHeaderNodeBased::getInfo()
{
    return "";
}
	
//------------------------------------------------------
void	VideoHeaderNodeBased::setOffsetInFrames(int _o)
{
	offsetFrames = _o;
}

//------------------------------------------------------
void VideoHeaderNodeBased::setInMs(double _in)
{
    this->inMs = _in;
}

//------------------------------------------------------
void VideoHeaderNodeBased::setOutMs(double _out)
{
    this->outMs = _out;
}

    //-----------------------------------------
    void VideoHeaderNodeBased::setVideoBuffer(ofxPm::VideoBuffer* &_videoBuffer)
    {
//
//        // allocate fbo where to draw
//        if (fbo.getWidth()<=0)
//        {
//            // setup FBO
//            int resX = videoBuffer->getWidth();
//            int resY = videoBuffer->getHeight();
//            fbo.allocate(resX,resY,GL_RGBA);
//            
//            
//            // setup Headers
//            videoHeader.resize(paramNumHeaders);
//            videoRenderer.resize(paramNumHeaders);
//            for (int i=0;i<paramNumHeaders;i++){
//                videoHeader[i].setup(*videoBuffer);
//                videoRenderer[i].setup(videoHeader[i]);
//                if(i==0)
//                {
//                    videoHeader[i].setDelayMs(1);
//                }
//                else videoHeader[i].setDelayMs(-1);
//                
//            }
//            
//        }
//        
//
        
        cout << "setting Video Buffer and sending new frame to header !!"  << endl;

        ofxPm::VideoFrame vf;
        vf = getNextVideoFrame();
        parameters->get("Frame Output").cast<ofxPm::VideoFrame>() = vf;
        
        
    }

    
}

