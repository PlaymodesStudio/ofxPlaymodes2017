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
    position    = 0;
    oneFrame    = (TimeDiff)round(1000000.0/(double)fps);
	oneFrameMs	= 1000.0 / fps;
    speed       = 1;
    in          = 1;
    out         = 0;
	length		= in - out;
    delay       = 0;
	opacity		= 255;
    playing     = false;
	loopStart	= false;
	loopMode	= OF_LOOP_NORMAL;
	driveMode	= 0;
	currentPos  = 0;
	buffer 		= NULL;
	windowPriority = "in";
	length = 0;
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
    position    = _buffer.size();
	totalBufferSizeInMs = _buffer.size() * (1000.0/fps);
    oneFrame    = (TimeDiff)round(1000000.0/(double)fps);
	oneFrameMs	= 1000.0 / fps;
    speed       = 1;
    in          = totalBufferSizeInMs;
    out         = 0;
	length		= totalBufferSizeInMs;
    playing   	= false;
    delay       = 0;
	opacity		= 255;
	loopStart	= false;
	loopMode	= OF_LOOP_NORMAL;
	driveMode	= 0;
	currentPos  = 0;
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
    //double currentLength=double(currentPos)/(double(this->buffer->getMaxSize()))*(double)(ofGetWidth()-PMDRAWSPACING*2);
    
    // old way ... changed by the TS Header
    //int headerPos = (buffer->size()-currentPos+offsetFrames) * oneLength;
	int drawHeaderY = ofGetHeight() -140;
	int originXAtEnd = ofGetWidth() - PMDRAWSPACING;
	
    // TS HEADER
    int headerPos = (buffer->getMaxSize() - currentFrameIndex)*oneLength;
    float headerPosBasedOnDelayMs = ((delayInMs/1000.0) / (buffer->getMaxSize()*(1.0/buffer->getFps())))*((double)(ofGetWidth()-PMDRAWSPACING*2));
    cout << "Header Drawing : DelayInMs : " << delayInMs << "BufferSize " << buffer->getMaxSize() << " FPS :: " << buffer->getFps() << endl;
    
	// draw HEADER LINE
    ///////////////////
    ofSetLineWidth(1);
    
	if(playing) ofSetColor(0,128,0,128);
	else ofSetColor(255,0,0,128);

    ofLine(originXAtEnd-headerPos+1,drawHeaderY,originXAtEnd-headerPos,drawHeaderY+100);
    ofRect(originXAtEnd-headerPos-offsetFrames,drawHeaderY+50,oneLength,10);
    ofCircle(originXAtEnd-headerPosBasedOnDelayMs,drawHeaderY,5);
	ofDrawBitmapString(ofToString(currentFrameIndex),ofPoint(originXAtEnd-headerPos-offsetFrames+oneLength/2,drawHeaderY+45));
	
	//int	inFrame  = int(double(buffer->size()-1)*(in));
	//int outFrame = int(double(buffer->size()-1)*(out));
	int inFrame = this->getInFrames();
	int outFrame = this->getOutFrames();
	int inPos;
	//inPos= PMDRAWSPACING + ((int(buffer->size())-inFrame) * oneLength) + oneLength/2;
	inPos = originXAtEnd - (oneLength*(inFrame+1));
	//int outPos = PMDRAWSPACING + (int((buffer->size())-outFrame) * oneLength) + oneLength/2;
	int outPos = originXAtEnd - (oneLength*outFrame);
	
	//printf("inPos %d outPos %d :: in %f inF %d oneL %f buffS %d:: %d\n",inPos,outPos,in,inFrame,oneLength, ((buffer->size()-1-inFrame) * oneLength),PMDRAWSPACING + ((buffer->size()-1-inFrame) * oneLength) + (oneLength/2));
	

	// draw in & out lines
	ofSetLineWidth(1.0);
	ofLine(inPos,drawHeaderY,inPos,drawHeaderY+100);
    ofLine(outPos,drawHeaderY,outPos,drawHeaderY+100);
	// draw in & out rectangle
	ofRect(inPos,drawHeaderY,outPos-inPos,40);
	
	//ofLine(inPos,drawHeaderY+60,outPos,drawHeaderY+60);
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
	
	ofDrawBitmapString("<" + ofToString(inFrame+1),ofPoint(inPos+0,drawHeaderY+37));
	ofDrawBitmapString(ofToString(outFrame+1) + ">" ,ofPoint(outPos-30,drawHeaderY+20));
	ofDrawBitmapString("*" +ofToString(this->getLengthFrames()) + "" ,ofPoint(((outPos-inPos)/2)+inPos-30,drawHeaderY-5));
	
	ofSetColor(255,255,255);
	
	ofDisableAlphaBlending();
}
//------------------------------------------------------
VideoFrame VideoHeader::getNextVideoFrame(){
    
    // old style
    //			currentPos=getNextPosition();
    //			VideoFrame frame = buffer->getVideoFrame(currentPos);
    
    // frame to be returned;
    VideoFrame frame;
    
    buffer->lock();
    {
        currentFrameTs = getNextTimestampFrame();
        frame = buffer->getVideoFrame(currentFrameTs);
        currentFrameIndex = frame.getBufferIndex();

    }
    buffer->unlock();
    return frame;
}

//------------------------------------------------------
Timestamp   VideoHeader::getNextTimestampFrame()
{
    Timestamp ts = 0.0;
    
    if(!buffer->isStopped())
    {
        delayTS.update();
        //inTS.update();
        //outTS.update();
        
        //inTS = inTS + TimeDiff(in*1000);
        //outTS = outTS + TimeDiff(out*1000);
    }
    
    if(!isPlaying())
    {
    
        // just a delay
        ts = delayTS - TimeDiff(delayInMs*1000);

    }
    else
    {
        /*
        if(isPlaying()) oneFrame=(TimeDiff)(1000000.0/fps/speed);
        else oneFrame=(TimeDiff)(1000000.0/fps/1.0);
        
        // if time spend since last positionTS.update() + portion to next frame is >= oneFrame
        // means that we need to update the position !!
        // position is expressed in frames since started (0..N)
        //if((double)positionTS.elapsed()+(position-floor(position))*(double)abs(oneFrame)>=abs(oneFrame))
        if( positionTS.elapsed() >= abs(oneFrame))
        {
            //        if(oneFrame!=0)
            //        {
            //            position=position + (double(positionTS.elapsed())/(double)oneFrame);
            //        }
            // updates the time-stamp with the current time
            positionTS.update();
        }
        
        // if we're playing in loop and we're reaching the outpoint        
        if(isPlaying() && (positionTS>outTS))
        {
            
            if(loopMode==OF_LOOP_NORMAL) positionTS = inTS;
            else if (loopMode==OF_LOOP_NONE)
            {
                setPlaying(false);
            }
            else if (loopMode==OF_LOOP_PALINDROME)
            {
                speed=-speed;
            }
        }
        cout << "Header : PosTS = "<< positionTS.raw() << " // outTS " << outTS.raw() << " DIFF == " << positionTS.raw() - outTS.raw() << endl;
        return positionTS;
        */
        
    }
    
    return ts;
}

//------------------------------------------------------
int VideoHeader::getNextPosition()
{
    
    
    
    // returns the real position in the buffer . calculate the next position in frames
    // from the beginning of the recording based on speed
    // position expresses number of frames since start
    
    // calculate how much microseconds is a frame
    // if we're playing, speed has sense, if not not ?
    
    unsigned int buffer_size;
    unsigned int totalNumFr;
    unsigned int lastAbsFrame;
    int	inFrame;
    int outFrame;
    int	inAbsFrame;
    int	outAbsFrame;
    int backpos;
    int nextPos;
    
    buffer_size=buffer->size();
    totalNumFr = buffer->getTotalFrames();
    lastAbsFrame = totalNumFr - buffer_size;
    //			inFrame  = int(double(buffer_size-1)*(in));
    //			outFrame = int(double(buffer_size-1)*(out));
    inFrame = this->getInFrames();
    outFrame = this->getOutFrames();
    inAbsFrame  = totalNumFr -  inFrame;
    outAbsFrame = totalNumFr - outFrame;
    
    switch (driveMode)
    {
        case 0 :
            // normal mode, based on time
            
            if(playing) oneFrame=(TimeDiff)(1000000.0/fps/speed);
            else oneFrame=(TimeDiff)(1000000.0/fps/1.0);
            
            
            // if time spend since last positionTS.update() + portion to next frame is >= oneFrame
            // means that we need to update the position !!
            // position is expressed in frames since started (0..N)
            if((double)positionTS.elapsed()+(position-floor(position))*(double)abs(oneFrame)>=abs(oneFrame))
            {
                if(oneFrame!=0)
                {
                    position=position + (double(positionTS.elapsed())/(double)oneFrame);
                }
                // updates the time-stamp with the current time
                positionTS.update();
            }
            
            // if header is playing and loopStart is requested, set position to inPoint or outPoint depending on speed's sign !
            if(playing && loopStart)
            {
                if(speed>0.0) position=double(inAbsFrame);
                else position=double(outAbsFrame);
                loopStart=false;
            }
            
            // if we're playing in loop and we're reaching the outpoint
            if(playing && (int(position) > (outAbsFrame)))
            {
                if(loopMode==OF_LOOP_NORMAL) position = double(inAbsFrame);
                else if (loopMode==OF_LOOP_NONE)
                {
                    setPlaying(false);
                }
                else if (loopMode==OF_LOOP_PALINDROME)
                {
                    speed=-speed;
                }
            }
            // if we're in playing in loop and we're reaching the inpoint (while speed is negative probably)
            else if(playing && (int(position) < (inAbsFrame)) && speed<0.0)
            {
                if(loopMode==OF_LOOP_NORMAL) position = double(outAbsFrame);
                else if (loopMode==OF_LOOP_NONE) setPlaying(false);
                else if (loopMode==OF_LOOP_PALINDROME)
                {
                    speed=-speed;
                }
            }
            
            // clamp position to it's limits ...
            
            if(playing) position=CLAMP(position,double(inAbsFrame),double(outAbsFrame));
            else position=CLAMP(position,double(lastAbsFrame),double(totalNumFr));
            
            
            // backpos
            backpos=0;	
            if (!playing)
            { 
                backpos=0;
                nextPos= float(buffer_size-1) - (delay/oneFrame) ;
            }
            else {
                backpos=int(buffer->getTotalFrames()-int(position));
                backpos=CLAMP(backpos,0,buffer_size-1);
                nextPos= (buffer_size-1) - backpos;
            }
            nextPos = CLAMP(nextPos+offsetFrames,0,buffer_size-1);
            return nextPos;
            
            break;
            
        case 1 :
            // here speed should not modify frame ! because audio is driving this 
            if(playing) oneFrame=(TimeDiff)(1000000.0/fps);
            else oneFrame=(TimeDiff)(1000000.0/fps/1.0);
            
            // position driven by audio trough calls to delay !!
            nextPos= int(buffer_size-1) - int(double(delay)/double(oneFrame));
            nextPos = CLAMP(nextPos+offsetFrames,0,buffer_size-1);
            return nextPos;
            
            break;
        default:
            return 0;
            break;
    }
    
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
	return delay/1000;
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
    //---------- TS
    delayInMs = _delayMs;
    //---------- TS
    
	oneFrame=(TimeDiff)(1000000.0/fps/1.0);
	int delayToSet = int(double(_delayMs*1000.0));

	// control not out of bounds !! needs more precise control related to bufferMarkers !! (TO DO)
	if(delayToSet<0) delayToSet = 0;
	else if (delayToSet>int(double(buffer->getMaxSize()-1)*double(oneFrame))) delayToSet = int(double(buffer->getMaxSize()-1)*double(oneFrame));

	
	//int totalNumFr = buffer->getTotalFrames();
	//int	outFrame = this->getOutFrames();
	//int outAbsFrame = totalNumFr - outFrame;
	//position = outAbsFrame-(delayToSet/oneFrame) ;
	//printf("VH::setDelayMs Position %f in frames\n",position);
	this->delay = delayToSet;
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
double VideoHeader::getIn()
{
	return in;
}

//------------------------------------------------------
double VideoHeader::getInFrames()
{
	return in/oneFrameMs;
}

//------------------------------------------------------
void VideoHeader::setInMs(double in)
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
		this->in = in;		
	}
	else if (windowPriority=="length")
	{
		if(in<=length) 
		{
			this->in=length;
		}
		else 
		{
			this->in = in;
		}
		printf("in is %f out is %f length is %f\n",in,out,length);
		// update the state of out 
		this->out = this->in - length;
	}
	
	// we clamp it to the out limit ... in could not be "smaller"(bigger really) then in
	//this->in=CLAMP(in,out,totalBufferSizeInMs);
	
	
	//printf("vH :: in %d = pct %f\n",in,double(in*1000.0f) / (oneFrameMs*double(buffer->size())));
	//printf("ms to set %f translated to %f fps %f\n",in,double(in) / (oneFrameMs*double(buffer->size())),this->fps);
	//printf("VIDEO inMs %d :: %f \n",in,double(in) / (oneFrameMs*double(buffer->size())));
    
    this->setLengthMs(out-in);
    cout << "Header: SetInMs Out-In : " << out-in << " :: Out = " << out << " :: In = " << in << endl;
}
//------------------------------------------------------
void VideoHeader::setInPct(double in)
{
	//this->in=CLAMP(in,this->out,1.0);
	// from pct to ms
	//this->in = in;
	this->setInMs(in*(totalBufferSizeInMs));
}
//------------------------------------------------------
void VideoHeader::setInFrames(int in)
{
/*	double pct = double(in)/double(buffer->size());
	this->setInPct(pct);
	printf("vH::setInFrames !\n");
*/
	this->setInMs(double(in) * oneFrameMs);
}
	
//------------------------------------------------------
double VideoHeader::getOut()
{
	return out;
}
//------------------------------------------------------
double VideoHeader::getOutFrames()
{
	return out/oneFrameMs;
}
	
//------------------------------------------------------
void VideoHeader::setOutMs(double _out)
{
	/*
	double oneFrameMs=(TimeDiff)(1000000.0/fps/1.0);
	double fAux = double(out*1000.0f) / (oneFrameMs*double(buffer->size()));
	this->setOutPct(CLAMP(fAux,0.0,1.0));    
	 */
	
	if(windowPriority=="in")
    {
        this->out = _out;
        
    }
    this->setLengthMs(out-in);

    cout << "Header: SetInMs Out-In : " << out-in << " :: Out = " << out << " :: In = " << in << endl;
    
}
//------------------------------------------------------
void VideoHeader::setOutPct(double out)
{
	/*
	this->out=CLAMP(out,0.0f,this->in);
	this->out=out;
	 */
	this->setOutMs(out*(totalBufferSizeInMs));
}
//------------------------------------------------------
void VideoHeader::setOutFrames(int out)
{
	/*
	double pct = double(out)/double(buffer->size());
	this->setInPct(pct);
	 */
	this->setOutMs(double(out)*oneFrameMs);
}
	
//------------------------------------------------------
double VideoHeader::getLength()
{
	return length;
}
//------------------------------------------------------
double VideoHeader::getLengthFrames()
{
	return length/oneFrameMs;
}
//------------------------------------------------------
void VideoHeader::setLengthMs(double _length)
{
	double oneFrameMs=(TimeDiff)(1000000.0/fps/1.0);
	double fAux = double(out*1000.0f) / (oneFrameMs*double(buffer->size()));
    this->length = _length;
	//this->setOutPct(CLAMP(fAux,0.0,1.0));
//	if(this->in-_length >= 0.0)
//    {
//		this->length = _length;
//		this->out = this->in-_length;
//	}
}
//------------------------------------------------------
void VideoHeader::setLengthPct(double _pct)
{
//	this->out=CLAMP(out,0.0f,this->in);
//	this->out=out;
	this->setLengthMs(totalBufferSizeInMs*_pct);
}
//------------------------------------------------------
void VideoHeader::setLengthFrames(int length)
{
//	double pct = double(out)/double(buffer->size());
//	this->setInPct(pct);
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
		// if we're entering loop mode move position to in point
		// this behaviour is to sync entering loop mode with starting at inPoint or outPoint depending on speed
		this->playing = isPlaying;
		int	loopFrame;
		if(speed>0.0f) loopFrame = this->getInFrames();
		else loopFrame = this->getOutFrames();
		//		if(speed>0.0f) loopFrame = int(double(buffer->size()-1)*(in));
		//		else loopFrame = int(double(buffer->size()-1)*(out));
		int	inAbsFrame  = buffer->getTotalFrames() -  loopFrame;
		position = inAbsFrame; 
	}
	else
	{
		// if we're gettint out of loop mode move delay to actual position
		// this behaviour is to let the header (set by delay on no loop) where the loop was when deactivated
		// other beahaviour could be to let the header on delay / inPoint / outPoint position when loop is turned off
		this->playing = isPlaying;
		double	actualFrame  = double(buffer->getTotalFrames()-1) - (position);
		TimeDiff oneFrame=(TimeDiff)(1000000.0/fps/1.0);		
		delay = (actualFrame-1)*oneFrame;
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

	if(playing) oneFrame=(TimeDiff)(1000000.0/fps/speed);
	else oneFrame=(TimeDiff)(1000000.0/fps/1.0);
	
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
		+" || in_" + ofToString(in) 
		+" || outF_" + ofToString(outFrame)
		+" || out_" + ofToString(out) 
		+" || inAbsF_" + ofToString(inAbsFrame)
		+" || outAbFs_" + ofToString(outAbsFrame)
	    +"\n"
		+" || currentPosF_" +ofToString(currentPos,2)
		+" || positionF_" +ofToString(position,2)
		+" || delay_ " +ofToString(delay,2)
	
	;
/*
		+" || LastAbsF_" + ofToString(buffer->getTotalFrames() - buffer->size())
		+" || InF_" + ofToString(int(double(buffer->size()-1)*(in)))
		+" || OutF_" + ofToString(int(double(buffer->size()-1)*(out)))
		);
 */
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
//	position = position + offsetFrames;
}

//--
}
