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
    void setupNodeBased();
    
    VideoFrame getNextVideoFrame();
	void newVideoFrame(VideoFrame & frame);
    float getFps(){return source->getFps();};
    void setGradientWidth(float &_f){paramGradientWidth=_f;};
    void setGradientXorY(int &_i){paramGradientXorY=_i;};

private:
	VideoFrame      frame;
    ofFbo           fbo;
	VideoSource     *source;
	ofShader        shader;
	bool            newFrame;
    ofPlanePrimitive plane;
	//ofMutex swapBuffersMutex;

    float fps;
    
    //    float gradientWidth;
    //    int gradientXorY;
    // FEATURE NODE
    ofParameterGroup*                   parameters;
    ofParameter<float>                  paramGradientWidth;
    ofParameter<int>                    paramGradientXorY;
//    ofParameter<ofxPm::VideoFrame>      paramFrameIn;
//    ofParameter<ofxPm::VideoFrame>      paramFrameOut;
    bool                                isNodeBased;

};
}

#endif /* GradientEdgesFilter */
