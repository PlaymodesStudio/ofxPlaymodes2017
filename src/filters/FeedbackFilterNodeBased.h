/*
 * VideoMixerGPU.h
 *
 *  Created on: Jan 18, 2013
 *      Author: arturo
 */

#ifndef FEEEDBACK_FILTER_H
#define FEEEDBACK_FILTER_H

#include "VideoFilter.h"

namespace ofxPm{
    
class FeedbackFilterNodeBased: public VideoFilter {
public:
	FeedbackFilterNodeBased();
	virtual ~FeedbackFilterNodeBased();

    void setup(VideoSource & source1);
    void setupNodeBased();
    
    VideoFrame getNextVideoFrame();
	void newVideoFrame(VideoFrame & frame);
    float getFps(){return source->getFps();};

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
    ofParameter<float>                  paramScale;
    ofParameter<float>                  paramMixAmmount;
    
    
    bool                                isNodeBased;

    VideoFrame                          lastFrameRendered;
};
}

#endif /* FEEEDBACK_FILTER_H */
