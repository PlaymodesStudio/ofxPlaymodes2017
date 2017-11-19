/*
 * AudioFrame.h
 *
 *  Created on: 09-oct-2008
 *      Author: arturo castro
 */

#ifndef VIDEOHEADER_H_INCLUDED
#define VIDEOHEADER_H_INCLUDED

#include "VideoSink.h"
#include "VideoSource.h"
#include "VideoBuffer.h"

// acts as a video source and sink
// controls the position in a buffer
// based on speed, delay, fps and loop in/out

namespace ofxPm{
    
class VideoHeader:public VideoSink, public VideoSource{
public:
	
    VideoHeader(VideoBuffer & buffer);
    VideoHeader();
    ~VideoHeader();

    void setup(VideoBuffer & buffer);
    void draw();
    
    VideoBuffer *getBuffer();
    
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
    VideoBuffer		*buffer;			// pointer to the buffer of video frames
	double			totalBufferSizeInMs;
    double			oneFrameMs;
    double			fps;				// framerate
    double 			inMs, outMs, lengthMs;	// expressed in ms from realtime ... in is bigger then out
	int				offsetFrames;				// to adjust av sync in frames
    bool            playing;
    
    //---------- TS HEADER CONTROL VARS

    Timestamp		nowTS;
    double           delayInMs;
    Timestamp       inTS, outTS;
    Timestamp       currentFrameTs;
    int             currentFrameIndex;

};
}
#endif // VIDEOHEADER_H_INCLUDED
