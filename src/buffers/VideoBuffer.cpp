/*
 * VideoBuffer.cpp
 *
 *  Created on: 09-oct-2008
 *      Author: arturo castro
 */

#include "VideoBuffer.h"
#include "VideoHeader.h"
#include "parametersControl.h"

namespace ofxPm
{

    //------------------------------------------------------
    VideoBuffer::VideoBuffer(VideoSource & source, int size)
    {
        setup(source,size,true);
        
    }

    //------------------------------------------------------
    VideoBuffer::VideoBuffer()
    {
        source = NULL;
        totalFrames=0;
        stopped = false;
        maxSize = 0;
        microsOneSec=0;
//        realFps = 0.0;
        framesOneSec = 0;
        
    //    stopTime=0;
    }



    //------------------------------------------------------
    void VideoBuffer::setup(VideoSource & _source, int size, bool allocateOnSetup)
    {
        source=&_source;
        totalFrames=0;
        maxSize = size;
        isNodeBased=false;
        
        VideoSource::width = _source.getWidth();
        VideoSource::height = _source.getHeight();

        if(allocateOnSetup){
            printf("VideoBuffer:: allocating on setup %d %d : ",VideoSource::getWidth(),VideoSource::getHeight());
            for(int i=0;i<size;i++){
                VideoFrame videoFrame = VideoFrame::newVideoFrame(source->getNextVideoFrame().getPixelsRef());
                //videoFrame.getTextureRef();
                newVideoFrame(videoFrame);
                printf("%d-",i);
            }
            printf("//\n");
        }
        
        microsOneSec=-1;
            
        // do this the last to avoid sending nullptr frame
        resume();
    }

    //------------------------------------------------------
    void VideoBuffer::setupNodeBased(int size, bool allocateOnSetup)
    {
        source=NULL;
        totalFrames=0;
        maxSize = size;
        isNodeBased=true;
        
        VideoSource::width = -1;
        VideoSource::height = -1;
        
        if(allocateOnSetup && VideoSource::width!=-1)
        {
            printf("VideoBuffer:: allocating on setup %d %d : ",VideoSource::getWidth(),VideoSource::getHeight());
            for(int i=0;i<size;i++){
                VideoFrame videoFrame = VideoFrame::newVideoFrame(source->getNextVideoFrame().getPixelsRef());
                //videoFrame.getTextureRef();
                newVideoFrame(videoFrame);
                printf("%d-",i);
            }
            printf("//\n");
        }
        else cout << "Video Buffer in 'node based' was not allocated on setup." << endl;
        
        microsOneSec=-1;
        
        // parametersGroup
        parameters = new ofParameterGroup();
        parameters->setName("Video Buffer");
        parameters->add(paramFPS.set("FPS",60,0,60));
        parameters->add(paramFrameIn.set("Frame Input",VideoFrame::VideoFrame()));
        parameters->add(paramFrameOut.set("Frame Output",VideoFrame::VideoFrame()));
        parameters->add(paramVideoBufferOut.set("Buffer Output",nullptr));
        
        //    parameters->add(paramFrameIn.set("Frame Output",vf));
        
        parametersControl::getInstance().createGuiFromParams(parameters,ofColor::red);
        
        // do this the last to avoid sending nullptr frame
        resume();
    }

VideoBuffer::~VideoBuffer() {

}

    //----------------------------------------------------
    void VideoBuffer::newVideoFrame(VideoFrame & frame)
    {
        if(width<=0)
        {
            cout << "BuffNewVideoFrame = " << frame.getWidth() << " , " << frame.getHeight() << endl;
            this->setWidth(frame.getWidth());
            this->setHeigth(frame.getHeight());
            
//            printf("VideoBuffer:: allocating NOW %d %d : ",frame.getWidth(),frame.getHeight());
//            for(int i=0;i<size;i++)
//            {
//                if(!isNodeBased)
//                {
//                    VideoFrame videoFrame = VideoFrame::newVideoFrame(source->getNextVideoFrame().getPixelsRef());
//                }
//                else
//                {
//                    
//                }
//                //videoFrame.getTextureRef();
//                newVideoFrame(videoFrame);
//                printf("%d-",i);
//            }
//            printf("//\n");
        }

        
            
            
        int64_t time = frame.getTimestamp().epochMicroseconds();
        if(microsOneSec==-1) microsOneSec=time;
        framesOneSec++;
        int64_t diff = time-microsOneSec;
        if(diff>=1000000){
            //realFps = double(framesOneSec*1000000.)/double(diff);
            framesOneSec = 0;
            microsOneSec = time-(diff-1000000);
        }
        totalFrames++;
    //    if(size()==0)initTime=frame.getTimestamp();
        TimeDiff tdiff = frame.getTimestamp() - initTime;
            //cout << "Buff::NewVideoFrame:: with TS = " << tdiff << " Which comes from frameTS : " << frame.getTimestamp().raw() << " - initTime " << initTime.raw() << endl;
        frame.setTimestamp(tdiff );
        //timeMutex.lock();
        frames.push_back(frame);
        //cout << "Buffer : newVideoFrame with TS : " << frame.getTimestamp().raw() << endl;
        while(getSizeInFrames()>maxSize){
            frames.erase(frames.begin());
        }
        //timeMutex.unlock();
        newFrameEvent.notify(this,frame);

            parameters->get("Frame Output").cast<ofxPm::VideoFrame>() = frame;

            parameters->get("Buffer Output").cast<ofxPm::VideoBuffer*>() = (ofxPm::VideoBuffer*)this;

    }

Timestamp VideoBuffer::getLastTimestamp(){
    if(getSizeInFrames()>0)
        return frames.back().getTimestamp();
    else
        return Timestamp();
}

TimeDiff VideoBuffer::getTotalTime(){
    return getLastTimestamp()-getInitTime();
}

Timestamp VideoBuffer::getInitTime(){
    return initTime;
}



unsigned int VideoBuffer::getMaxSize(){
	return maxSize;
}


float VideoBuffer::getFps(){
    if(!isNodeBased) return source->getFps();
    else return paramFPS;
}
    
//---- TS
VideoFrame VideoBuffer::getVideoFrame(Timestamp ts)
{
    VideoFrame frame;
    int closestPosition=0;
    if(frames.size()>0)
    {
        TimeDiff tdiff = 1000000000000000;
        for(int i=0;i<getSizeInFrames();i++)
        {
            TimeDiff tdiff2 = abs(ts - frames[i].getTimestamp());
            //cout << "Buffer:GetVFr:: Frame : "<< i << " has a TS of : " << frames[i].getTimestamp().raw()  <<  " and we look for " << ts.raw() << " . The diff is = " << tdiff2 << endl;
            if(tdiff2<tdiff)
            {
                //cout << "!! Found a closest position !! : "<< i << endl;
                closestPosition=i;
                tdiff=tdiff2;
            }
            
        }
        //cout<<"Buffer : Getting frame at closest TS : " << ts.raw()<< " :: Closest Position :: " << closestPosition<<endl;
        
        frame = frames[closestPosition];
    }
    // ??? is this a good way to go ?
    // i've added a "index position" to a videoFrame ... this allows us to draw header based on pos, not TS
    frame.setBufferIndex(closestPosition);
    return frame;
}

//---- TS

    
VideoFrame VideoBuffer::getVideoFrame(TimeDiff time)
{
    VideoFrame frame;
    if(getSizeInFrames()>0)
    {
        int frameback = CLAMP((int)((float)time/1000000.0*(float)getFps()),1,int(getSizeInFrames()));
        int currentPos = CLAMP(getSizeInFrames()-frameback,0,getSizeInFrames()-1);
        frame = frames[currentPos];
    }

    return frame;
}
 
    
VideoFrame VideoBuffer::getVideoFrame(int position){
    //return buffer.find(times[times.size()-position])->second;
    if(getSizeInFrames()){
        position = CLAMP(position,0,int(getSizeInFrames())-1);
        //cout << "frame " << position << " retained " << frames[position]->_useCountOfThisObject << "\n";
        return frames[position];
    }else{
        return VideoFrame();
    }
}

    //----------------------------------------------
    VideoFrame VideoBuffer::getVideoFrame(float pct)
    {
        return getVideoFrame(getLastTimestamp()-(getInitTime()+getTotalTime()*pct));
    }

    //----------------------------------------------
    VideoFrame VideoBuffer::getNextVideoFrame()
    {
        return getVideoFrame((int)getSizeInFrames()-1);
    }

    //----------------------------------------------
    unsigned int VideoBuffer::getSizeInFrames()
    {
        int res = 0;
        if(!frames.empty()) res=frames.size();
        return res;
    }

    long VideoBuffer::getTotalFrames()
    {
        return totalFrames;
    }

//double VideoBuffer::getRealFPS(){
//    return realFps;
//}


void VideoBuffer::draw(){
		
    float length = (float(getSizeInFrames())/float(maxSize))*(ofGetWidth()-(PMDRAWSPACING*2));
    float oneLength=float(ofGetWidth()-PMDRAWSPACING*2)/(float)(maxSize);
	int sizeInOneLengths= oneLength*getSizeInFrames();
	int drawBufferY = ofGetHeight() -80;
	int originXAtEnd = ofGetWidth() - PMDRAWSPACING;

	ofEnableAlphaBlending();
	
	ofSetColor(25,25,25,240);
	ofRect(PMDRAWSPACING,ofGetHeight()-140,ofGetWidth()-2*PMDRAWSPACING,100);
	
    if(!stopped) ofSetColor(255,0,0);
	else ofSetColor(255);
    ofSetLineWidth(3);
	ofLine(originXAtEnd-int(sizeInOneLengths),drawBufferY,originXAtEnd,drawBufferY);
	
	ofSetColor(255);
	
	int fps = getFps();
    char measureMessage[10];
    for(int i=0;i<getSizeInFrames()+1;i=i+5){
		
       if((getSizeInFrames()-i)%fps==0){
		   ofSetColor(255,128);
		   //ofRect(originXAtEnd-(oneLength*(i)),drawBufferY+10,oneLength-1,-10);
           ofLine(originXAtEnd-(oneLength*(i)),drawBufferY,originXAtEnd-(oneLength*(i)),drawBufferY-10);
		   //ofDrawBitmapString(ofToString(int((float(size())-float(i))/float(fps)))+"s",originXAtEnd-(oneLength*(i)),drawBufferY+25);
//            ofLine(originXAtEnd - (oneLength*i),710,originXAtEnd - (oneLength*i),700);
//            sprintf(measureMessage,"%0.2f",(float)(frames[i]->getTimestamp()-initTime)/1000000.0);
//            ofDrawBitmapString(measureMessage,originXAtEnd - (oneLength*i),695);
        }
	   else {
		   ofSetColor(64);
		   ofLine(originXAtEnd-(oneLength*(i)),drawBufferY,originXAtEnd-(oneLength*(i)),drawBufferY-10);
	   }
    }
	ofDisableAlphaBlending();
    
    // drawing TimeStamp of certain frames ...
    int steps = this->getMaxSize()/3;
    for(int i=frames.size()-1;i>0;i=i-steps)
    {
//        ofSetColor(255,128,0);
//        ofDrawBitmapString(ofToString(i), oneLength*(i)+oneLength, drawBufferY-25);
        Timestamp tA;
        if(i<=this->getMaxSize())
        {
            ofSetColor(255,0,0,128);
            tA = frames[i].getTimestamp();
            ofDrawBitmapString(ofToString(tA.raw()/1000000.0), oneLength*(i)+oneLength, drawBufferY+25);
        }
        
    }
    
    ofDrawBitmapString(ofToString(this->getInitTime().raw()/1000000.0),10,drawBufferY+45);

//    cout << "......" << endl;
//    if(ofGetElapsedTimeMillis()>4000)
//    {
//        
//        for(int i=0;i<getMaxSize();i++)
//        {
//            cout << "Buff: Frame " << i << "has timestamp of " << frames[i].getTimestamp().raw() << endl;
//            
//        }
//        
//    }
    
	ofSetColor(255);

}


void VideoBuffer::stop()
{
    if(!isNodeBased)
    {
        ofRemoveListener(source->newFrameEvent,this,&VideoBuffer::newVideoFrame);
    }
    else paramFrameIn.removeListener(this,&VideoBuffer::newVideoFrame);
    
    stopped = true;
    
    stopTime = initTime.elapsed();
    
    

    cout << "Buffer: Stop! : " << endl;
}

void VideoBuffer::resume()
{
	if(!isNodeBased)
    {
        ofAddListener(source->newFrameEvent,this,&VideoBuffer::newVideoFrame);
    }
    else paramFrameIn.addListener(this, &VideoBuffer::newVideoFrame);
    
    stopped = false;
    
    initTime.update();
    initTime -= stopTime;

}

bool VideoBuffer::isStopped()
{
    
	return stopped;
}

void VideoBuffer::clear(){
    while(!frames.empty()){
        frames.erase(frames.begin());
    }
}
}


