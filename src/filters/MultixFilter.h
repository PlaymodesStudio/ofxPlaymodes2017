/*
 * AudioFrame.h
 *
 *  Created on: 09-oct-2008
 *      Author: arturo castro
 */

#ifndef MULTIXFILTER_H
#define MULTIXFILTER_H

#include "VideoBuffer.h"
#include "VideoHeader.h"
#include "VideoRenderer.h"

#include "ofMain.h"

namespace ofxPm{
        
class MultixFilter: public VideoSource
{
public:
    MultixFilter();
    MultixFilter(ofxPm::VideoBuffer & buffer, int numHeaders=2);

    virtual ~MultixFilter();

    void    setup(ofxPm::VideoBuffer & buffer, int numHeaders=2);

    void    updateValuesPct(vector<float> _vf);
    void    updateValuesMs(vector<float> _vf);
    void    drawIntoFbo(int x, int y,int w, int h);

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
    
    // + 
    void                                    newVideoFrame(VideoFrame & _frame);
    ofEvent<VideoFrame>                     newFrameEvent;
    VideoFrame                              getNextVideoFrame();
    float                                   getFps(){return videoBuffer->getFps();};
    void                                    setOpacityMode(int _m){opacityMode=_m;};
    
protected:
    vector<ofxPm::VideoHeader>              videoHeader;
    vector<ofxPm::VideoRenderer>            videoRenderer;
    ofxPm::VideoBuffer *                    videoBuffer;


private:

    int numHeaders;

    // rendering
    VideoFrame      frame;
    ofFbo           fbo;
    bool            minmaxBlend;
    int             opacityMode;

};
}

#endif // MULTIXFILTER_H
