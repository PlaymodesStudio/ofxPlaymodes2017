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

#include "ofMain.h"

namespace ofxPm{
    
class MultixRenderer
{
public:
	MultixRenderer();
	MultixRenderer(ofxPm::VideoBuffer & buffer, int numHeaders=2);

	virtual ~MultixRenderer();

	void    setup(ofxPm::VideoBuffer & buffer, int numHeaders=2);

    void    updateValuesPct(vector<float> _vf);
    void    updateValuesMs(vector<float> _vf);
	void    draw(int x, int y,int w, int h);

    void    setNumHeaders(int numHeaders);
    int     getNumHeaders();

	ofxPm::VideoHeader * getHeader(int header);
	ofxPm::VideoRenderer * getRenderer(int renderer);
    
    ofxPm::VideoBuffer *getVideoBuffer() const;
    void    setVideoBuffer(ofxPm::VideoBuffer *videoBuffer);
    void    setVideoHeader(vector<ofxPm::VideoHeader*> videoHeader);
    void    setVideoRenderer(vector<ofxPm::VideoRenderer*> videoRenderer);
    
    bool    isMinmaxBlend() const;
    void    setMinmaxBlend(bool minmaxBlend);
    
protected:
	vector<ofxPm::VideoHeader>              videoHeader;
	vector<ofxPm::VideoRenderer>            videoRenderer;
	ofxPm::VideoBuffer *                    videoBuffer;


private:

	int numHeaders;

    // rendering
    bool        minmaxBlend;
    
};
}

#endif // MULTIXRENDERER_H
