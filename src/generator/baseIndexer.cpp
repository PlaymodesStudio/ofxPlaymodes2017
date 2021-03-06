//
//  baseIndexer.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 09/01/2017.
//
//

#include "baseIndexer.h"

void window_no_close_indexer(GLFWwindow* window){
    glfwSetWindowShouldClose(window, GL_FALSE);
};

baseIndexer::baseIndexer(int numIndexs){
    indexCount = numIndexs;
    indexs.resize(indexCount, 0);
    indexRand.resize(indexCount , 0);
    for(int i = 0; i < indexRand.size(); i++)
        indexRand[i] = i-((float)indexRand.size()/2.f);
    indexRand_Param_previous = 0;
    
    indentityReindexMatrix.resize(indexCount, vector<bool>(indexCount, false));
    for(int i = 0; i < indexCount; i++){
        indentityReindexMatrix[i][i] = true;
    }
    isReindexIdentity = true;
    
    parameters = new ofParameterGroup;
    parameters->setName("Indexer");
    parameters->add(numWaves_Param.set("Num Waves", 1, 0, indexCount));
    parameters->add(indexInvert_Param.set("Index Invert", 0, 0, 1));
    parameters->add(symmetry_Param.set("Symmetry", 0, 0, 10));
    parameters->add(indexRand_Param.set("Index Random", 0, 0, 1));
    parameters->add(indexOffset_Param.set("Index Offset", 0, -indexCount/2, indexCount/2));
    parameters->add(indexQuant_Param.set("Index Quantization", indexCount, 1, indexCount));
    parameters->add(combination_Param.set("Index Combination", 0, 0, 1));
    parameters->add(modulo_Param.set("Index Modulo", indexCount, 1, indexCount));
    if(numIndexs < 100){
        parameters->add(manualReindex_Param.set("Manual Reindex", false));
        parameters->add(reindexGrid.set("ReindexGrid", indentityReindexMatrix));
    }
    
    numWaves_Param.addListener(this, &baseIndexer::parameterFloatListener);
    indexInvert_Param.addListener(this, &baseIndexer::parameterFloatListener);
    symmetry_Param.addListener(this, &baseIndexer::parameterIntListener);
    indexRand_Param.addListener(this, &baseIndexer::parameterFloatListener);
    indexOffset_Param.addListener(this, &baseIndexer::parameterFloatListener);
    indexQuant_Param.addListener(this, &baseIndexer::parameterIntListener);
    combination_Param.addListener(this, &baseIndexer::parameterFloatListener);
    modulo_Param.addListener(this, &baseIndexer::parameterIntListener);
    
    indexRand_Param.addListener(this, &baseIndexer::indexRandChanged);
    
    manualReindex_Param.addListener(this, &baseIndexer::drawManualReindex);
    
    reindexGrid.addListener(this, &baseIndexer::reindexChanged);

    reindexWindowRect.setPosition(-1, -1);
    
    recomputeIndexs();
}

void baseIndexer::recomputeIndexs(){
    for (int i = 0; i < indexCount ; i++){
        int index = i;
        
        //QUANTIZE
        int newNumOfPixels = indexQuant_Param;
        
        index = floor(index/((float)indexCount/(float)newNumOfPixels));
        
        
        while(symmetry_Param > newNumOfPixels-1)
            symmetry_Param--;
        
        bool odd = false;
        if((abs(indexOffset_Param) - (int)abs(indexOffset_Param)) > 0.5) odd = !odd;
        
        if((int)((index)/(newNumOfPixels/(symmetry_Param+1)))%2 == 1) odd = true;
        
        
        //SYMMETRY santi
        int veusSym = newNumOfPixels/(symmetry_Param+1);
        index = veusSym-abs((((int)(index/veusSym)%2) * veusSym)-(index%veusSym));
        
        
        if(newNumOfPixels % 2 == 0){
            index += odd ? 1 : 0;
        }
        else if(symmetry_Param > 0){
            index += 1;
            index %= newNumOfPixels;;
        }
        
        
        //INVERSE
        //Fisrt we invert the index to simulate the wave goes from left to right, inverting indexes, if we want to invertit we don't do this calc
        int nonInvertIndex = index-1;
        int invertedIndex = ((float)indexCount-(float)index);
        index = indexInvert_Param*invertedIndex + (1-indexInvert_Param)*nonInvertIndex;
        
        //random
        index += indexRand[index-1]*indexRand_Param;
        index %= indexCount;
        if(index < 0)
            index += indexCount;
        
        
        //COMB
        index = abs(((index%2)*indexCount*combination_Param)-index);
        
        //Modulo
        if(modulo_Param != modulo_Param.getMax())
            index %= modulo_Param;
        
        
        int shifted_i = i + round(indexOffset_Param);
        if(shifted_i < 0) shifted_i += indexCount;
        shifted_i %= indexCount;
        indexs[shifted_i] = (((float)index/(float)indexCount))*(numWaves_Param*((float)indexCount/(float)newNumOfPixels))*(symmetry_Param+1);
    }
    newIndexs();
}

void baseIndexer::indexRandChanged(float &val){
    if(indexRand_Param_previous == 0)
        random_shuffle(indexRand.begin(), indexRand.end());
    indexRand_Param_previous = val;
}



//Reindex

void baseIndexer::draw(ofEventArgs &e){
    ofBackground(127);
    ofSetColor(255);
    
    //Draw red lines when quqntize
    vector<int> indexQuantize(indexCount, 1);
    if(indexQuant_Param != indexCount){
        int oldVal = -1;
        for(int i = 0; i < indexCount; i++){
            int newVal = floor(i/((float)indexCount/(float)indexQuant_Param));
            if(newVal != oldVal)
                indexQuantize[i] = 1;
            else
                indexQuantize[i] = 0;
            oldVal = newVal;
        }
    }
    
    int x_margin = 10;
    int y_margin = 10;
    int x_labelWidth = 20;
    int y_labelHeight = 20;
    float x_step = (ofGetWidth() - x_margin*2 - x_labelWidth)/indexCount;
    float y_step = (ofGetHeight() - y_margin*2 - y_labelHeight)/indexCount;
    for(int i = 0; i < indexCount; i++){
        ofDrawBitmapString(ofToString(i), (i+0.5)*x_step + x_margin + x_labelWidth, 10);
        for(int j = 0; j < indexCount; j++){
            if(i == 0){
                ofDrawBitmapString(ofToString(j), 5, (j+0.5)*y_step + y_margin + y_labelHeight);
            }
            ofPushStyle();
            ofSetRectMode(OF_RECTMODE_CORNER);
         
            if(indexQuantize[i] == 1){
                ofFill();
                ofSetColor(255, 0, 0, 127);
                ofDrawRectangle(i*x_step + x_margin + x_labelWidth, j*y_step + y_margin + y_labelHeight, x_step, y_step);
            }
            ofNoFill();
            ofSetColor(255);
            ofDrawRectangle(i*x_step + x_margin + x_labelWidth, j*y_step + y_margin + y_labelHeight, x_step, y_step);
            ofPopStyle();
            if(reindexGrid.get()[i][j]){
                ofDrawLine((i+0.25)*x_step + x_margin + x_labelWidth
                           , (j+0.25)*y_step + y_margin + y_labelHeight
                           , (i+0.75)*x_step + x_margin + x_labelWidth
                           , (j+0.75)*y_step + y_margin + y_labelHeight);
                ofDrawLine((i+0.75)*x_step + x_margin + x_labelWidth
                           , (j+0.25)*y_step + y_margin + y_labelHeight
                           , (i+0.25)*x_step + x_margin + x_labelWidth
                           , (j+0.75)*y_step + y_margin + y_labelHeight);
            }
        }
    }
}



void baseIndexer::mouseMoved(ofMouseEventArgs &a){
    
}

void baseIndexer::mousePressed(ofMouseEventArgs &a){
    int x_margin = 10;
    int y_margin = 10;
    int x_labelWidth = 20;
    int y_labelHeight = 20;
    float x_step = (ofGetWidth() - x_margin*2 - x_labelWidth)/indexCount;
    float y_step = (ofGetHeight() - y_margin*2 - y_labelHeight)/indexCount;
    for(int i = 0; i < indexCount; i++){
        for(int j = 0; j < indexCount; j++){
            ofSetRectMode(OF_RECTMODE_CORNER);
            ofRectangle rect(i*x_step + x_margin + x_labelWidth, j*y_step + y_margin + y_labelHeight, x_step, y_step);
            if(rect.inside(a)){
                vector<vector<bool>> reindexCopy = reindexGrid.get();
                reindexCopy[i][j] = !reindexGrid.get()[i][j];
                reindexGrid = reindexCopy;
                break;
            }
        }
    }
}

void baseIndexer::mouseReleased(ofMouseEventArgs &a){
    
}

void baseIndexer::mouseDragged(ofMouseEventArgs &a){

}

void baseIndexer::drawManualReindex(bool &b){
    if(b){
        ofAppBaseWindow* mainWindow = ofGetWindowPtr();
        
        ofGLFWWindowSettings prevSettings;
        if(reindexWindowRect.getPosition() == ofPoint(-1, -1)){
            prevSettings.width = 1024;
            prevSettings.height = 1024;
            prevSettings.setPosition(ofVec2f(ofGetScreenWidth()-1024, ofGetScreenHeight()-1024));
        }
        else{
            prevSettings.width = reindexWindowRect.width;
            prevSettings.height = reindexWindowRect.height;
            prevSettings.setPosition(reindexWindowRect.position);
        }
        prevSettings.windowMode = OF_WINDOW;
        prevSettings.resizable = true;
        reindexWindow = ofCreateWindow(prevSettings);
        reindexWindow->setWindowTitle(parameters->getName());
        ofAddListener(reindexWindow->events().draw, this, &baseIndexer::draw);
        ofAddListener(reindexWindow->events().mouseMoved, this, &baseIndexer::mouseMoved);
        ofAddListener(reindexWindow->events().mousePressed, this, &baseIndexer::mousePressed);
        ofAddListener(reindexWindow->events().mouseReleased, this, &baseIndexer::mouseReleased);
        ofAddListener(reindexWindow->events().mouseDragged, this, &baseIndexer::mouseDragged);
        ofAppGLFWWindow * ofWindow = (ofAppGLFWWindow*)reindexWindow.get();
        GLFWwindow * glfwWindow = ofWindow->getGLFWWindow();
        glfwSetWindowCloseCallback(glfwWindow, window_no_close_indexer);
    }
    else if(reindexWindow != nullptr){
        reindexWindowRect.setPosition(reindexWindow->getWindowPosition());
        reindexWindowRect.setSize(reindexWindow->getWidth(), reindexWindow->getHeight());
        reindexWindow->setWindowShouldClose();
        reindexWindow = nullptr;
    }
}


void baseIndexer::reindexChanged(vector<vector<bool> > &vb){
    if(vb != indentityReindexMatrix){
        isReindexIdentity = false;
    }
    else{
        isReindexIdentity = true;
    }
}
