/*
 * VideoRenderer.h
 *
 *  Created on: 09-oct-2008
 *      Author: arturo castro
 */

#ifndef VIDEORENDERERNODEBASED_H_
#define VIDEORENDERERNODEBASED_H_

#include "VideoSink.h"
#include "VideoSource.h"
#include "ofShader.h"


namespace ofxPm{

class VideoRendererNodeBased: public VideoSink {

public:
	VideoRendererNodeBased();
	~VideoRendererNodeBased();

	void setup();
    void draw(int x,int y,int w,int h);
    void draw();
    virtual void newVideoFrame(VideoFrame & frame);

    ofTexture getLastFrameTexture();

private:
    VideoFrame  vFrame;
    
    // NODE related
    ofParameterGroup*                   parameters;
    //    ofParameter<ofxPm::VideoFrame>      paramFrameIn;

};
}

#endif /* VIDEORENDERERNODEBASED_H_ */
