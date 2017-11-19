///*
// * VideoRenderer.h
// *
// *  Created on: 09-oct-2008
// *      Author: arturo castro
// */

#ifndef VIDEORENDERER_LUMAKEY_H_
#define VIDEORENDERER_LUMAKEY_H_

#include "VideoSink.h"
#include "VideoSource.h"
#include "VideoRenderer.h"
#include "ofShader.h"

namespace ofxPm{

class VideoRendererLumaKey: public VideoRenderer {

public:
	VideoRendererLumaKey();
	VideoRendererLumaKey(VideoSource & source);
	virtual ~VideoRendererLumaKey();

	void setup(VideoSource & source);

//	void draw();
//    void draw(int x,int y,int w,int h);
    
    ofColor     getTint() const;

    
    ofShader    getShader() const;
    bool        isShaderActive() const;
    void        setShader(ofShader shader);
    void        setShaderActive(bool shaderActive);

    bool        isMinmaxBlend() const;
    void        setMinmaxBlend(bool minmaxBlend);
    void        setTint(ofColor tint);

    void        setLumaThreshold(float f) {lumaThreshold = f;cout << f << endl;};
    void        setLumaSmooth(float f) {lumaSmooth = f;cout << f << endl;};

private:
    
    void drawNextFrame(int x,int y,int w,int h);
    VideoSource *source;
    
    ofColor     tint;
	bool        minmaxBlend;
    
    // Shader related
	bool        shaderActive;
    ofShader    shader;
    float       lumaThreshold;
    float       lumaSmooth;
};
}

#endif /* VIDEORENDERER_LUMAKEY_H_ */
