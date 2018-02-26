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
    void    setupNodeBased();

    void    updateValuesPct(vector<float> _vf);
    void    updateValuesMs(vector<float> _vf);
    void    drawIntoFbo(int x, int y,int w, int h);

    void    setNumHeaders(int numHeaders);
    int     getNumHeaders();

    ofxPm::VideoHeader * getHeader(int header);
    ofxPm::VideoRenderer * getRenderer(int renderer);
    
    ofxPm::VideoBuffer *getVideoBuffer() const;
    void    setVideoBuffer(ofxPm::VideoBuffer* &videoBuffer);
    void    setVideoHeader(vector<ofxPm::VideoHeader*> videoHeader);
    void    setVideoRenderer(vector<ofxPm::VideoRenderer*> videoRenderer);
    
    ofMutex     mutex;
    bool        isMinmaxBlend() const;
    
    // + 
    void                                    newVideoFrame(VideoFrame & _frame);
    ofEvent<VideoFrame>                     newFrameEvent;
    VideoFrame                              getNextVideoFrame();
    float                                   getFps(){return videoBuffer->getFps();};

    void                                    setMinmaxBlend(bool &_b){paramMinMaxBlend = _b;}
    void                                    setOpacityMode(int &_m){paramOpacityMode=_m;};
    void                                    setOffsetBeatDiv(int _n){paramOffsetBeatDiv = _n;};
    void                                    setOffsetBeatMult(int _n){paramOffsetBeatMult = _n;};

    void                                    recalculate(int &_i);
    void                                    changedDistributionVector(vector<float> &_b);
    
protected:
    vector<ofxPm::VideoHeader>              videoHeader;
    vector<ofxPm::VideoRenderer>            videoRenderer;
    ofxPm::VideoBuffer *                    videoBuffer;
    
    //NodeBased
    ofParameterGroup*                   parameters;

    ofParameter<int>                    paramNumHeaders;
    ofParameter<int>                    paramOpacityMode;
    ofParameter<bool>                   paramMinMaxBlend;
    ofParameter<bool>                   paramLinearDistribution;
    ofParameter<bool>                   paramUseBPM;
    ofParameter<float>                  paramManualOffsetMs;
    ofParameter<int>                    paramOffsetBeatDiv;
    ofParameter<int>                    paramOffsetBeatMult;
    ofParameter<ofxPm::VideoBuffer*>    paramVideoBufferInput;
    ofParameter<vector<float>>          paramDistributionVector;
    
    bool                                isNodeBased;


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
