/*
 * VideoMixerGPU.h
 *
 *  Created on: Jan 18, 2013
 *      Author: arturo
 */

#ifndef VIDEOTESTSHADERFX_H_
#define VIDEOTESTSHADERFX_H_

#include "VideoFilter.h"

namespace ofxPm{
    
class VideoTestShaderFX: public VideoFilter {
public:
	VideoTestShaderFX();
	virtual ~VideoTestShaderFX();

	void setup(VideoSource & source1);
    void setupNodeBased();
    
    VideoFrame getNextVideoFrame();
	void newVideoFrame(VideoFrame & frame);
    float getFps(){return source->getFps();};
    void setLumaThreshold(float &f){lumaThreshold=f;};
    void setLumaSmooth(float &f){lumaSmooth=f;};

private:
	VideoFrame      frame;
    ofFbo           fbo;
	VideoSource     *source;
	ofShader        shader;
	bool            newFrame;
    ofPlanePrimitive plane;
	//ofMutex swapBuffersMutex;
    
    float lumaThreshold;
    float lumaSmooth;
    float fps;
    
    // FEATURE NODE
    ofParameterGroup*                   parameters;
    ofParameter<float>                  paramLumaThrshold;
    ofParameter<float>                  paramLumaSmooth;
    bool                                isNodeBased;
//    ofParameter<ofxPm::VideoFrame>      paramFrameIn;
//    ofParameter<ofxPm::VideoFrame>      paramFrameOut;

};
}

#endif /* VIDEOTESTSHADERFX_H_ */
