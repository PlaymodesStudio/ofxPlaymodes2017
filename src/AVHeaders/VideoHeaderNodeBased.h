/*
 * AudioFrame.h
 *
 *  Created on: 09-oct-2008
 *      Author: arturo castro
 */

#ifndef VIDEOHEADERNODEBASED_H_INCLUDED
#define VIDEOHEADERNODEBASED_H_INCLUDED

#include "VideoSink.h"
#include "VideoSource.h"
#include "VideoBuffer.h"

// acts as a video source and sink
// controls the position in a buffer
// based on speed, delay, fps and loop in/out

namespace ofxPm{
    
class VideoHeaderNodeBased:public VideoSink, public VideoSource{
public:
    VideoHeaderNodeBased();
    ~VideoHeaderNodeBased();

    void setupNodeBased();
    void draw();
    
    VideoBuffer *getBuffer();
    void        setVideoBuffer(ofxPm::VideoBuffer* &videoBuffer);

    VideoFrame  getNextVideoFrame();
    Timestamp   getNextFrameTimestamp();
    //VideoFrame  getVideoFrame(int index);
	
    void setInMs(double _in);
    void setOutMs(double _out);
    
    double  getDelayMs(){return delayInMs;};
    void	setDelayMs(double _delayMs); // !! This is the main function to manipulate the video header !

    float	getFps();
    void	setFps(float fps);

    string	getInfo();
    void	setOffsetInFrames(int _o);
	
    double  getBufferSizeInMs(){return totalBufferSizeInMs;};
    
protected:
//    VideoBuffer		*buffer;			// pointer to the buffer of video frames
	double			totalBufferSizeInMs;
    double			oneFrameMs;
    double			fps;				// framerate
    double 			inMs, outMs, lengthMs;	// expressed in ms from realtime ... in is bigger then out
	int				offsetFrames;				// to adjust av sync in frames
    bool            playing;
    
    //---------- TS HEADER CONTROL VARS

    Timestamp		nowTS;
    double          delayInMs;
    Timestamp       inTS, outTS;
    Timestamp       currentFrameTs;
    int             currentFrameIndex;
    
    // NODE BASED STUFF
    ofParameterGroup*                   parameters;
    ofParameter<ofxPm::VideoBuffer*>    paramVideoBufferInput;
    ofParameter<float>                  paramDelayMs;
    


    

};
}
#endif // VIDEOHEADERNODEBASED_H_INCLUDED
