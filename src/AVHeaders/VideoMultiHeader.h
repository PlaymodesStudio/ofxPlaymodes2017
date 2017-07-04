/*
 * AudioFrame.h
 *
 *  Created on: 09-oct-2008
 *      Author: arturo castro
 */

#ifndef VIDEOMULTIHEADER_H_INCLUDED
#define VIDEOMULTIHEADER_H_INCLUDED

#include "VideoSink.h"
#include "VideoSource.h"
#include "VideoBuffer.h"

// acts as a video source and sink
// controls the position in a buffer
// based on speed, delay, fps and loop in/out

namespace ofxPm{
class VideoMultiHeader:public VideoSink, public VideoSource{
public:
	
    VideoMultiHeader(VideoBuffer & buffer);
    VideoMultiHeader();
    ~VideoMultiHeader();

    void setup(VideoBuffer & buffer);
    void draw();
    
    VideoBuffer *getBuffer();

    VideoFrame  getNextVideoFrame();
	VideoFrame  getVideoFrame(int index);
    Timestamp   getNextFrameTimestamp();
    //    int         getNextPosition();
	
	// in&out drawing
    void setInMs(double _in);
    void setOutMs(double _out);
    
    vector<double>	getMultiDelayMs(){return multiDelayInMs;};
    void	setMultiDelayMs(vector<double> _delays);

    float	getFps();
    void	setFps(float fps);
	
	// events
	void 	receivedLoopEvent(int &i);
	string	getInfo();
	void	setOffsetInFrames(int o);

    
protected:
    VideoBuffer		*buffer;			// pointer to the buffer of video frames
	double			totalBufferSizeInMs;
    double			oneFrameMs;
	

    double			fps;				// framerate
//    double			position;			// position expresses number of frames since start (0..N)
//    int               delay;
//    int				currentPos;
    double 			speed;
    double 			inMs, outMs, lengthMs;	// expressed in ms from realtime ... in is bigger then out
	int				opacity;
	string			windowPriority;		// in : is always respected (changing length) length : length is always respected (constraining in)
	int				offsetFrames;				// to adjust av sync in frames
	
	//	OF_LOOP_NONE=0x01,
	//	OF_LOOP_PALINDROME=0x02,
	//	OF_LOOP_NORMAL=0x03

    bool			playing;
	int				loopMode;
	bool			loopStart;
	
	int				driveMode; // 0 : based on time // 1 : based on audio which controls it through delay param.
    
    //---------- TS HEADER CONTROL VARS

    Timestamp		nowTS;
    vector<double>   multiDelayInMs;
    Timestamp       inTS, outTS;
    Timestamp       currentFrameTs;
    int             currentFrameIndex;

};
}
#endif // VIDEOMULTIHEADER_H_INCLUDED
