/*
 * AudioFrame.h
 *
 *  Created on: 09-oct-2008
 *      Author: arturo castro
 */


#ifndef VIDEOGRABBERNODEBASED_H_INCLUDED
#define VIDEOGRABBERNODEBASED_H_INCLUDED

#include "VideoSource.h"
#include "ofMain.h"


namespace ofxPm{
class VideoGrabberNodeBased: public VideoSource, public ofVideoGrabber{
public:
    VideoGrabberNodeBased();
    virtual ~VideoGrabberNodeBased();

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
    
    // FEATURE NODES
    ofParameterGroup*                   parameters;
    ofParameter<bool>                   paramSettings;
    ofParameter<bool>                   paramReconnect;
    ofParameter<bool>                   paramGrab;
    ofParameter<int>                    paramDeviceId;
    ofParameter<int>                    paramResolutionX;
    ofParameter<int>                    paramResolutionY;
    void                                setGrabberSettings(bool & _b);
    void                                setSetupGrabber(bool & _b);

    int                                 numGrabberDevices;
};
}

#endif // VIDEOGRABBERNODEBASED_H_INCLUDED
