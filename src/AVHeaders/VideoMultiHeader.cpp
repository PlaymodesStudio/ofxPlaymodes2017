 /*
 * AudioFrame.h
 *
 *  Created on: 09-oct-2008
 *      Author: arturo castro
 */

#include "VideoMultiHeader.h"

namespace ofxPm{
VideoMultiHeader::VideoMultiHeader(VideoBuffer & buffer){
setup(buffer);
}

//------------------------------------------------------
VideoMultiHeader::VideoMultiHeader(){
    fps         = 25.0;
	oneFrameMs	= 1000.0 / fps;
    speed       = 1;
    inMs        = 0;
    outMs       = 0;
	lengthMs		= inMs - outMs;
	opacity		= 255;
    playing     = false;
	loopStart	= false;
	loopMode	= OF_LOOP_NORMAL;
	driveMode	= 0;
	buffer 		= NULL;
	windowPriority = "in";
	lengthMs = 0;
	offsetFrames = 0.0;
	width = -11;
	height = -11;
}


//------------------------------------------------------
void VideoMultiHeader::setup(VideoBuffer & _buffer){
    //newFrameEvent.init("Playmodes.VideoHeader.newFrame");
    this->buffer= &_buffer;
    fps         = _buffer.getFps();
	this->buffer->clear();
	totalBufferSizeInMs = _buffer.size() * (1000.0/fps);
	oneFrameMs	= 1000.0 / fps;
    speed       = 1;
    inMs        = totalBufferSizeInMs;
    outMs       = 0;
	lengthMs	= totalBufferSizeInMs;
    playing   	= false;
	opacity		= 255;
	loopStart	= false;
	loopMode	= OF_LOOP_NORMAL;
	driveMode	= 0;
	windowPriority = "in";
	offsetFrames = 0.0;
	
	VideoSource::width = _buffer.getWidth();
	VideoSource::height = _buffer.getHeight();
	
	printf("VideoMultiHeader::setup %d %d FPS %f\n",VideoSource::width,VideoSource::height,fps);
}

//------------------------------------------------------
VideoMultiHeader::~VideoMultiHeader(){
}

//------------------------------------------------------
void VideoMultiHeader::draw(){
	
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
VideoFrame VideoMultiHeader::getNextVideoFrame()
    {
        
    // frame to be returned;
    VideoFrame frame;
    
    buffer->lock();
    {
        // get the next frame timeStamp based on current behaviour
        currentFrameTs = getNextFrameTimestamp();
        // fetch closest video frame from buffer
        frame = buffer->getVideoFrame(currentFrameTs);
        // get the index of the fetched frame
        currentFrameIndex = frame.getBufferIndex();
    }
    buffer->unlock();
    return frame;
}

//------------------------------------------------------
Timestamp   VideoMultiHeader::getNextFrameTimestamp()
{
    // to be returned
    Timestamp ts;
    
    // when buffer is not stopped we need to update the nowTS which is used to update TS of delay,inTS,outTS ...
    if(!buffer->isStopped())
    {
        nowTS = buffer->getFirstFrameTimestamp();
    }

    //update in and out TS based on "nowTS"
    inTS.update();
    outTS.update();
    
    inTS = nowTS - TimeDiff(inMs*1000);
    outTS = nowTS - TimeDiff(outMs*1000);
    
    // calculate the ts of the needed frame
    
    // Multiheader ??
    //ts = nowTS - TimeDiff(delayInMs*1000);

    return ts;
}

//------------------------------------------------------
float VideoMultiHeader::getFps(){
    return fps;
}
//------------------------------------------------------
void VideoMultiHeader::setFps(float fps){
    this->fps=fps;
}
//------------------------------------------------------
VideoFrame VideoMultiHeader::getVideoFrame(int index)
{
	buffer->lock();
		int indexFrame = CLAMP(index,0,buffer->size()-1);
		VideoFrame frame = buffer->getVideoFrame(indexFrame);
	buffer->unlock();
	return frame;
}
//------------------------------------------------------
VideoBuffer *VideoMultiHeader::getBuffer()
{
    return buffer;
}

//------------------------------------------------------
void VideoMultiHeader::setMultiDelayMs(vector<double> _multiDelayMs)
{
    //delayInMs = _delayMs;
}


		
//------------------------------------------------------
string VideoMultiHeader::getInfo()
{
//	string s;
//
//	if(playing) oneFrameMs=(TimeDiff)(1000000.0/fps/speed);
//	else oneFrameMs=(TimeDiff)(1000000.0/fps/1.0);
//	
//	int buffer_size=buffer->size();
//	int totalNumFr = buffer->getTotalFrames();
//	int lastAbsFrame = totalNumFr - buffer_size; 
//	int inFrame  = this->getInFrames();
//	int	outFrame = this->getOutFrames();
//	int inAbsFrame  = totalNumFr -  inFrame;
//	int outAbsFrame = totalNumFr - outFrame;
//	
//	s = "Video Header >> Buff_" +ofToString(buffer_size) 
//		+" || TotalF_" + ofToString(totalNumFr)
//		+" || LastAbsF_" + ofToString(lastAbsFrame)
//		+" || inF_" + ofToString(inFrame) 
//		+" || in_" + ofToString(inMs)
//		+" || outF_" + ofToString(outFrame)
//		+" || out_" + ofToString(outMs)
//		+" || inAbsF_" + ofToString(inAbsFrame)
//		+" || outAbFs_" + ofToString(outAbsFrame)
//	    +"\n"
//
//
//	;
//
//	return s;
}
	
//------------------------------------------------------
void	VideoMultiHeader::setOffsetInFrames(int o)
{
	offsetFrames = o;
}

//------------------------------------------------------
void VideoMultiHeader::setInMs(double _in)
{
    // needs more precise control related to bufferMarkers !! (TO DO)
    /*
     double oneFrameMs=(TimeDiff)(1000000.0/fps/1.0);
     double fAux = double(in*1000.0) / (oneFrameMs*double(buffer->size()));
     printf("VH::setInMs faux %f",fAux);
     this->setInPct(CLAMP(fAux,0.0,1.0));
     */
    if(windowPriority=="in")
    {
        this->inMs = _in;
    }
    else if (windowPriority=="length")
    {
        if(inMs<=lengthMs)
        {
            this->inMs=lengthMs;
        }
        else
        {
            this->inMs = _in;
        }
        // update the state of out
        this->outMs = this->inMs - lengthMs;
    }
    
    // we clamp it to the out limit ... in could not be "smaller"(bigger really) then in
    //this->in=CLAMP(in,out,totalBufferSizeInMs);
}

//------------------------------------------------------
void VideoMultiHeader::setOutMs(double _out)
{
    this->outMs = _out;
    //cout << "Header: SetOutMs :: OutMs = " << outMs << " :: InMs = " << inMs << " :: LengthMs" << lengthMs <<endl;
}
    
}

