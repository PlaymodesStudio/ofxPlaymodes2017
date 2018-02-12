/*
 * VideoMixerGPU.h
 *
 *  Created on: Jan 18, 2013
 *      Author: arturo
 */

#ifndef GRADIENT_EDGES_FILTER_H
#define GRADIENT_EDGES_FILTER_H

#include "VideoFilter.h"

namespace ofxPm{
    
class GradientEdgesFilter: public VideoFilter {
public:
	GradientEdgesFilter();
	virtual ~GradientEdgesFilter();

	void setup(VideoSource & source1);

    VideoFrame getNextVideoFrame();
	void newVideoFrame(VideoFrame & frame);
    float getFps(){return source->getFps();};
    void setGradientWidth(float f){gradientWidth=f;};

private:
	VideoFrame      frame;
    ofFbo           fbo;
	VideoSource     *source;
	ofShader        shader;
	bool            newFrame;
    ofPlanePrimitive plane;
	//ofMutex swapBuffersMutex;

    float gradientWidth;
    int gradientXorY;
    float fps;
};
}

#endif /* GradientEdgesFilter */
