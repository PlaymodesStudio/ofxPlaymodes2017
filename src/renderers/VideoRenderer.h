/*
 * VideoRenderer.h
 *
 *  Created on: 09-oct-2008
 *      Author: arturo castro
 */

#ifndef VIDEORENDERER_H_
#define VIDEORENDERER_H_

#include "VideoSink.h"
#include "VideoSource.h"
#include "ofShader.h"


namespace ofxPm{

class VideoRenderer: public VideoSink {

public:
	VideoRenderer();
	VideoRenderer(VideoSource & source);
	~VideoRenderer();

	virtual void setup(VideoSource & source);

	void draw();
    void draw(int x,int y,int w,int h);
    
private:
    virtual void drawNextFrame(int x,int y,int w,int h);
    VideoSource *source;
};
}

#endif /* VIDEORENDERER_H_ */
