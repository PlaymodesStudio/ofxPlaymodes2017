 /*
 * AudioFrame.h
 *
 *  Created on: 09-oct-2008
 *      Author: arturo castro
 */

#include "VideoHeader.h"

namespace ofxPm{
VideoHeader::VideoHeader(VideoBuffer & buffer){
setup(buffer);
}

//------------------------------------------------------
VideoHeader::VideoHeader(){
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
void VideoHeader::setup(VideoBuffer & _buffer){
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
	
	printf("VideoHeader::setup %d %d FPS %f\n",VideoSource::width,VideoSource::height,fps);
}

//------------------------------------------------------
VideoHeader::~VideoHeader(){
}

//------------------------------------------------------
void VideoHeader::draw(){
	
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
	ofDrawBitmapString("*" +ofToString(this->getLengthFrames()) + "" ,ofPoint(((outPos-inPos)/2)+inPos-30,drawHeaderY-5));
    
    ofSetColor(255,255,255);
	
	ofDisableAlphaBlending();
}
//------------------------------------------------------
VideoFrame VideoHeader::getNextVideoFrame(){
    
    // frame to be returned;
    VideoFrame frame;
    
    buffer->lock();
    {
        // get the next frame timeStamp based on current behaviour
        currentFrameTs = getNextTimestampFrame();
        // fetch closest video frame from buffer
        frame = buffer->getVideoFrame(currentFrameTs);
        // get the index of the fetched frame
        currentFrameIndex = frame.getBufferIndex();
    }
    buffer->unlock();
    return frame;
}

//------------------------------------------------------
Timestamp   VideoHeader::getNextTimestampFrame()
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
    
    // when NOT playing the loop we give a delay timestamp based on delayInMs
    if(!isPlaying())
    {
    
        // just a delay
        ts = nowTS - TimeDiff(delayInMs*1000);
    }
    // when playing the loop ...
    else
    {
        double loopPositionAbsolute = phasors[0]->getPhasor() * (outMs-inMs); //ofMap(loopPositionNormalized, 0, 1, 0, out-in);
        ts = inTS - TimeDiff(loopPositionAbsolute*1000);
        return ts;
    }
    
    cout << "Header : getting next TS frame at : " << ts.raw() << endl;

    return ts;
}

//------------------------------------------------------
int VideoHeader::getNextPosition()
{
//    
//    
//    
//    // returns the real position in the buffer . calculate the next position in frames
//    // from the beginning of the recording based on speed
//    // position expresses number of frames since start
//    
//    // calculate how much microseconds is a frame
//    // if we're playing, speed has sense, if not not ?
//    
//    unsigned int buffer_size;
//    unsigned int totalNumFr;
//    unsigned int lastAbsFrame;
//    int	inFrame;
//    int outFrame;
//    int	inAbsFrame;
//    int	outAbsFrame;
//    int backpos;
//    int nextPos;
//    
//    buffer_size=buffer->size();
//    totalNumFr = buffer->getTotalFrames();
//    lastAbsFrame = totalNumFr - buffer_size;
//    //			inFrame  = int(double(buffer_size-1)*(in));
//    //			outFrame = int(double(buffer_size-1)*(out));
//    inFrame = this->getInFrames();
//    outFrame = this->getOutFrames();
//    inAbsFrame  = totalNumFr -  inFrame;
//    outAbsFrame = totalNumFr - outFrame;
//    
//    switch (driveMode)
//    {
//        case 0 :
//            // normal mode, based on time
//            
//            if(playing) oneFrame=(TimeDiff)(1000000.0/fps/speed);
//            else oneFrame=(TimeDiff)(1000000.0/fps/1.0);
//            
//            
//            // if time spend since last positionTS.update() + portion to next frame is >= oneFrame
//            // means that we need to update the position !!
//            // position is expressed in frames since started (0..N)
//            if((double)positionTS.elapsed()+(position-floor(position))*(double)abs(oneFrame)>=abs(oneFrame))
//            {
//                if(oneFrame!=0)
//                {
//                    position=position + (double(positionTS.elapsed())/(double)oneFrame);
//                }
//                // updates the time-stamp with the current time
//                positionTS.update();
//            }
//            
//            // if header is playing and loopStart is requested, set position to inPoint or outPoint depending on speed's sign !
//            if(playing && loopStart)
//            {
//                if(speed>0.0) position=double(inAbsFrame);
//                else position=double(outAbsFrame);
//                loopStart=false;
//            }
//            
//            // if we're playing in loop and we're reaching the outpoint
//            if(playing && (int(position) > (outAbsFrame)))
//            {
//                if(loopMode==OF_LOOP_NORMAL) position = double(inAbsFrame);
//                else if (loopMode==OF_LOOP_NONE)
//                {
//                    setPlaying(false);
//                }
//                else if (loopMode==OF_LOOP_PALINDROME)
//                {
//                    speed=-speed;
//                }
//            }
//            // if we're in playing in loop and we're reaching the inpoint (while speed is negative probably)
//            else if(playing && (int(position) < (inAbsFrame)) && speed<0.0)
//            {
//                if(loopMode==OF_LOOP_NORMAL) position = double(outAbsFrame);
//                else if (loopMode==OF_LOOP_NONE) setPlaying(false);
//                else if (loopMode==OF_LOOP_PALINDROME)
//                {
//                    speed=-speed;
//                }
//            }
//            
//            // clamp position to it's limits ...
//            
//            if(playing) position=CLAMP(position,double(inAbsFrame),double(outAbsFrame));
//            else position=CLAMP(position,double(lastAbsFrame),double(totalNumFr));
//            
//            
//            // backpos
//            backpos=0;	
//            if (!playing)
//            { 
//                backpos=0;
//                nextPos= float(buffer_size-1) - (delay/oneFrame) ;
//            }
//            else {
//                backpos=int(buffer->getTotalFrames()-int(position));
//                backpos=CLAMP(backpos,0,buffer_size-1);
//                nextPos= (buffer_size-1) - backpos;
//            }
//            nextPos = CLAMP(nextPos+offsetFrames,0,buffer_size-1);
//            return nextPos;
//            
//            break;
//            
//        case 1 :
//            // here speed should not modify frame ! because audio is driving this 
//            if(playing) oneFrame=(TimeDiff)(1000000.0/fps);
//            else oneFrame=(TimeDiff)(1000000.0/fps/1.0);
//            
//            // position driven by audio trough calls to delay !!
//            nextPos= int(buffer_size-1) - int(double(delay)/double(oneFrame));
//            nextPos = CLAMP(nextPos+offsetFrames,0,buffer_size-1);
//            return nextPos;
//            
//            break;
//        default:
//            return 0;
//            break;
//    }
//    
}


//------------------------------------------------------
float VideoHeader::getFps(){
    return fps;
}
//------------------------------------------------------
void VideoHeader::setFps(float fps){
    this->fps=fps;
}
//------------------------------------------------------
VideoFrame VideoHeader::getVideoFrame(int index)
{
	buffer->lock();
		int indexFrame = CLAMP(index,0,buffer->size()-1);
		VideoFrame frame = buffer->getVideoFrame(indexFrame);
	buffer->unlock();
	return frame;
}
//------------------------------------------------------
VideoBuffer *VideoHeader::getBuffer()
{
    return buffer;
}

//------------------------------------------------------
void VideoHeader::setSpeed(double speed)
{
	this->speed = speed;
}
//------------------------------------------------------
double VideoHeader::getSpeed()
{
    return speed;
}
//------------------------------------------------------
void VideoHeader::setOpacity(int opacity) 
{
	this->opacity = CLAMP (opacity,0,255);
}
//------------------------------------------------------
int VideoHeader::getOpacity() 
{
	return opacity;
}


	
	
	
//------------------------------------------------------
// get & set delay 
//------------------------------------------------------
int VideoHeader::getDelayMs() 
{
	return delayInMs;
}
//------------------------------------------------------
int VideoHeader::getDelayFrames() 
{
	return this->getDelayMs()/(TimeDiff)(1000.0/fps/1.0);
}
//------------------------------------------------------
double VideoHeader::getDelayPct()
{
	double res = this->getDelayFrames()/(buffer->size()-1);
	return res;
}
//------------------------------------------------------
void VideoHeader::setDelayMs(double _delayMs)
{
    delayInMs = _delayMs;
}
//------------------------------------------------------
void VideoHeader::setDelayFrames(int delayFrames)
{
	this->setDelayMs(int((double(delayFrames) / double(this->getFps()))*1000.0));
}
//------------------------------------------------------
void VideoHeader::setDelayPct(double pct)
{
	this->setDelayFrames(int(double(pct) * double(buffer->size())));
}

	
//------------------------------------------------------
// get & set in & out & length
//------------------------------------------------------
double VideoHeader::getInMs()
{
	return inMs;
}

//------------------------------------------------------
double VideoHeader::getInFrames()
{
	return inMs/oneFrameMs;
}

//------------------------------------------------------
void VideoHeader::setInMs(double _in)
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
    
    this->setLengthMs(outMs-inMs);
    cout << "Header: SetInMs :: OutMs = " << outMs << " :: InMs = " << inMs << " :: LengthMs" << lengthMs <<endl;
}
//------------------------------------------------------
void VideoHeader::setInPct(double _in)
{
	//this->in=CLAMP(in,this->out,1.0);
	// from pct to ms
	//this->in = in;
	this->setInMs(_in*(totalBufferSizeInMs));
}
//------------------------------------------------------
void VideoHeader::setInFrames(int _in)
{
	this->setInMs(double(_in) * oneFrameMs);
}
	
//------------------------------------------------------
double VideoHeader::getOutMs()
{
	return outMs;
}
//------------------------------------------------------
double VideoHeader::getOutFrames()
{
	return outMs/oneFrameMs;
}
	
//------------------------------------------------------
void VideoHeader::setOutMs(double _out)
{
	if(windowPriority=="in")
    {
        this->outMs = _out;
        
    }
    this->setLengthMs(outMs-inMs);

    cout << "Header: SetOutMs :: OutMs = " << outMs << " :: InMs = " << inMs << " :: LengthMs" << lengthMs <<endl;
}
//------------------------------------------------------
void VideoHeader::setOutPct(double _out)
{
	this->setOutMs(_out*(totalBufferSizeInMs));
}
//------------------------------------------------------
void VideoHeader::setOutFrames(int _out)
{
	this->setOutMs(double(_out)*oneFrameMs);
}
	
//------------------------------------------------------
double VideoHeader::getLength()
{
	return lengthMs;
}
//------------------------------------------------------
double VideoHeader::getLengthFrames()
{
	return lengthMs/oneFrameMs;
}
//------------------------------------------------------
void VideoHeader::setLengthMs(double _length)
{
    this->lengthMs = _length;
}
//------------------------------------------------------
void VideoHeader::setLengthPct(double _pct)
{
	this->setLengthMs(totalBufferSizeInMs*_pct);
}
//------------------------------------------------------
void VideoHeader::setLengthFrames(int length)
{
	this->setLengthMs(double(length)*oneFrameMs);
}

	
//------------------------------------------------------
// get & set loop & playing
//------------------------------------------------------
int VideoHeader::getLoopMode()
{
	return loopMode;
}
//------------------------------------------------------
void VideoHeader::setLoopMode(int loop)
{
	/*
	if((loopMode!=OF_LOOP_NORMAL) || (loopMode!=OF_LOOP_NONE) || (loopMode!=OF_LOOP_PALINDROME))
	{
		printf("ofxPlaymodes::VideoHeader:: Incorrect loop mode!\n");
	}
	else loopMode = loop;
	 */
	if (loop==3) loopMode = OF_LOOP_NORMAL;
	else if (loop==2) loopMode = OF_LOOP_PALINDROME;
	else if (loop==1) loopMode = OF_LOOP_NONE;
}
    
//------------------------------------------------------
void VideoHeader::audioRateTrigger(int bufferSize){
    for(int i = phasors.size()-1 ; i >= 0 ; i--){
        phasors[i]->audioIn(bufferSize);
    }
}
	
//------------------------------------------------------
void VideoHeader::setLoopToStart()
{
	loopStart=true;
}
//------------------------------------------------------
bool VideoHeader::isPlaying() 
{
	return playing;
}
//------------------------------------------------------
void VideoHeader::setPlaying(bool isPlaying, double speed)
{
}		
//------------------------------------------------------
void VideoHeader::setPlaying(bool isPlaying)
{
	if(isPlaying)
	{
        //create a phasor
        phasors.push_back(new phasorClass());
        phasors[0]->getParameterGroup()->getFloat("BPM") = 60;
        phasors[0]->getParameterGroup()->getBool("Bounce") = true;
		// if we're entering loop mode move position to in point
		// this behaviour is to sync entering loop mode with starting at inPoint or outPoint depending on speed
		this->playing = isPlaying;
		int	loopFrame;
		if(speed>0.0f) loopFrame = this->getInFrames();
		else loopFrame = this->getOutFrames();
		//		if(speed>0.0f) loopFrame = int(double(buffer->size()-1)*(in));
		//		else loopFrame = int(double(buffer->size()-1)*(out));
//		int	inAbsFrame  = buffer->getTotalFrames() -  loopFrame;
//		position = inAbsFrame; 
	}
	else
	{
        delete phasors[0];
        phasors.pop_back();
		// if we're gettint out of loop mode move delay to actual position
		// this behaviour is to let the header (set by delay on no loop) where the loop was when deactivated
		// other beahaviour could be to let the header on delay / inPoint / outPoint position when loop is turned off
		this->playing = isPlaying;
//		delay = (actualFrame-1)*oneFrame;
	}
}
	

//------------------------------------------------------
void VideoHeader::receivedLoopEvent(int &i)
{
	setLoopToStart();
}	

//------------------------------------------------------
void VideoHeader::setDriveMode(int mode)
{
	driveMode = mode;
}
	



//------------------------------------------------------
string VideoHeader::getInfo()
{
	string s;

	if(playing) oneFrameMs=(TimeDiff)(1000000.0/fps/speed);
	else oneFrameMs=(TimeDiff)(1000000.0/fps/1.0);
	
	int buffer_size=buffer->size();
	int totalNumFr = buffer->getTotalFrames();
	int lastAbsFrame = totalNumFr - buffer_size; 
	int inFrame  = this->getInFrames();
	int	outFrame = this->getOutFrames();
	int inAbsFrame  = totalNumFr -  inFrame;
	int outAbsFrame = totalNumFr - outFrame;
	
	s = "Video Header >> Buff_" +ofToString(buffer_size) 
		+" || TotalF_" + ofToString(totalNumFr)
		+" || LastAbsF_" + ofToString(lastAbsFrame)
		+" || inF_" + ofToString(inFrame) 
		+" || in_" + ofToString(inMs)
		+" || outF_" + ofToString(outFrame)
		+" || out_" + ofToString(outMs)
		+" || inAbsF_" + ofToString(inAbsFrame)
		+" || outAbFs_" + ofToString(outAbsFrame)
	    +"\n"


	;

	return s;
}
	
//------------------------------------------------------
void VideoHeader::setWindowPriority(string s)
{
	// in : is always respected (changing length) length : length is always respected (constraining in)
	if(s=="in")				windowPriority="in";
	else if(s=="length")	windowPriority = "length";	
}

//------------------------------------------------------
void	VideoHeader::setOffsetInFrames(int o)
{
	offsetFrames = o;
}


}
