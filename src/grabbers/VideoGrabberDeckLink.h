/*
 * AudioFrame.h
 *
 *  Created on: 09-oct-2008
 *      Author: arturo castro
 */


#ifndef VIDEOGRABBERDECKLINK_H_INCLUDED
#define VIDEOGRABBERDECKLINK_H_INCLUDED

#include "VideoSource.h"
#include "ofMain.h"
#include "ofxDeckLink.h"


namespace ofxPm{
class VideoGrabberDeckLink: public VideoSource, public ofVideoGrabber{
public:
    
    VideoGrabberDeckLink();
    
    virtual ~VideoGrabberDeckLink();

    bool initGrabber(int w, int h);

	VideoFrame getNextVideoFrame();
    
    float getFps();
    
	// ugly! need to be able to override the fps of the grabber. ofVideoGrabber doesn't have a getFps !!
	void setFps(float fps);
	
    void update();




private:
    void newFrame(ofPixels & frame);
	float fps;
	VideoFrame frame;
    
    ofxDeckLinkAPI::Input   input;
    ofFbo                   fbo;
    ofPixels                pixels;
};
}

#endif // VIDEOGRABBERDECKLINK_H_INCLUDED
