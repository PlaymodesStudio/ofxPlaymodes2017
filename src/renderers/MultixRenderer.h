/*
 * AudioFrame.h
 *
 *  Created on: 09-oct-2008
 *      Author: arturo castro
 */

#ifndef MULTIXRENDERER_H
#define MULTIXRENDERER_H

#include "VideoBuffer.h"
#include "VideoHeader.h"
#include "VideoRenderer.h"
#include "VideoRendererLumaKey.h"

#include "ofMain.h"

namespace ofxPm{
    
class MultixRenderer
{
public:
	MultixRenderer();
	MultixRenderer(ofxPm::VideoBuffer & buffer, int numHeaders=2);

	virtual ~MultixRenderer();

	void    setup(ofxPm::VideoBuffer & buffer, int numHeaders=2);


    void    update();
    void    updateValuesPct(vector<float> _vf);
    void    updateValuesMs(vector<float> _vf);
	void    draw(int x, int y,int w, int h);

    void    setNumHeaders(int numHeaders);
    int     getNumHeaders();

	ofxPm::VideoHeader * getHeader(int header);
	ofxPm::VideoRenderer * getRenderer(int renderer);
    
    float   getDelayOffset() const;
    float   getIn() const;
    int     getLoopMode() const;
    float   getOut() const;
    float   getSpeedOffset() const;
    ofxPm::VideoBuffer *getVideoBuffer() const;
    void    setDelayOffset(float delayOffset);
    void    setIn(float in);
    void    setLoopMode(int loopMode);
    void    setOut(float out);
    void    setSpeedOffset(float speedOffset);
    void    setVideoBuffer(ofxPm::VideoBuffer *videoBuffer);
    void    setVideoHeader(vector<ofxPm::VideoHeader*> videoHeader);
    void    setVideoRenderer(vector<ofxPm::VideoRenderer*> videoRenderer);
    
    ofColor getTint() const;
    bool    isMinmaxBlend() const;

    void    setMinmaxBlend(bool minmaxBlend);
    void    setTint(ofColor tint);
    
//    void    setLumaThreshold(float f) {lumaThreshold = f;cout << f << endl;};
//    void    setLumaSmooth(float f) {lumaSmooth = f;cout << f << endl;};
    
protected:
	vector<ofxPm::VideoHeader>              videoHeader;
	vector<ofxPm::VideoRenderer>            videoRenderer;
	ofxPm::VideoBuffer *                    videoBuffer;


private:

	float delayOffset;
	float speedOffset;

	float in;
	float out;
	int loopMode;
	int numHeaders;

    // rendering
    ofColor     tint;
    bool        minmaxBlend;
    
//    // Shader related
//    bool        shaderActive;
//    ofShader    shader;
//    float       lumaThreshold;
//    float       lumaSmooth;

};
}

#endif // MULTIXRENDERER_H
