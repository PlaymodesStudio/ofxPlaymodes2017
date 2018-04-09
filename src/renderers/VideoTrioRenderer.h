/*
 * VideoRenderer.h
 *
 *  Created on: 09-oct-2008
 *      Author: arturo castro
 */

#ifndef VIDEOTRIORENDERER_H_
#define VIDEOTRIORENDERER_H_

#include "VideoSink.h"
#include "VideoSource.h"
#include "ofShader.h"


namespace ofxPm{

class VideoTrioRenderer: public VideoSink {

public:
	VideoTrioRenderer();
	VideoTrioRenderer(VideoSource & source);
	~VideoTrioRenderer();

	virtual void setup(VideoSource & source);

	void draw();
    void draw(int x,int y,int w,int h);
    
private:
    virtual void drawNextFrame(int x,int y,int w,int h);
    VideoSource *source;
};
}

#endif /* VIDEOTRIORENDERER_H_ */
