/*
 * AudioFrame.h
 *
 *  Created on: 09-oct-2008
 *      Author: arturo castro
 */


#ifndef VIDEOGRABBERPS3EYE_H_INCLUDED
#define VIDEOGRABBERPS3EYE_H_INCLUDED

#include "VideoSource.h"
#include "ofMain.h"
#include "ps3eye.h"


namespace ofxPm{
class VideoGrabberPS3Eye: public VideoSource, public ofVideoGrabber{
public:
    VideoGrabberPS3Eye();
    virtual ~VideoGrabberPS3Eye();

    bool initGrabber(int w, int h);

	VideoFrame getNextVideoFrame();
    float getFps();
	// ugly! need to be able to override the fps of the grabber. ofVideoGrabber doesn't have a getFps !!
	void setFps(float fps);
	
    void update();

    void setAutoWhiteBalance(bool _b){eye->setAutoWhiteBalance(_b);};
    void setAutoGain(bool _b){eye->setAutogain(_b);};
    void setExposure(int _i){eye->setExposure(_i);};
    void setHue(int _i){eye->setHue((uint8_t)_i);};

private:
    void newFrame(ofPixels & frame);
	float fps;
	VideoFrame frame;
    
    ps3eye::PS3EYECam::PS3EYERef eye;
    unsigned char * videoFrame;
    ofTexture videoTexture;


};
}

#endif // VIDEOGRABBERPS3EYE_H_INCLUDED
