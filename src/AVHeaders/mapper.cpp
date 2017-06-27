//
//  phasorClass.cpp
//  DGTL_Generator
//
//  Created by Eduard Frigola on 28/07/16.
//
//

#include "phasorClass.h"
#include "parametersControl.h"


phasorClass::phasorClass(int index, ofPoint pos)
{
    phasor = 0;
    parameters = new ofParameterGroup;
    parameters->setName("phasor " + ofToString(index));
    parameters->add(bpm_Param.set("BPM", 120, 0, 999));
    parameters->add(beatsDiv_Param.set("Beats Div", 1, 1, 64));
    parameters->add(beatsMult_Param.set("Beats Mult", 1, 1, 12));
    parameters->add(quant_Param.set("Quantization", 40, 1, 40));
    parameters->add(initPhase_Param.set("Initial Phase", 0, 0, 1));
    parameters->add(minVal_Param.set("Min Value", 0, 0, 1));
    parameters->add(maxVal_Param.set("Max Value", 1, 0, 1));
    parameters->add(resetPhase_Param.set("Reset Phase", false));
    parameters->add(loop_Param.set("Loop", true));
    parameters->add(bounce_Param.set("Bounce", false));
    parameters->add(offlineMode_Param.set("Offline Mode", false));
    parameters->add(phasorMonitor.set("Phasor Monitor", 0, 0, 1));
    
    
    resetPhase_Param.addListener(this, &phasorClass::resetPhasor);
    loop_Param.addListener(this, &phasorClass::loopChanged);
    initPhase_Param.addListener(this, &phasorClass::initPhaseChanged);
    
//    parametersControl::getInstance().createGuiFromParams(*parameters, ofColor::green);
    parametersControl::getInstance().createGuiFromParams(parameters, ofColor::green, pos);
}

phasorClass::~phasorClass(){
    delete parameters;
}

float phasorClass::getPhasor(){
    if(!offlineMode_Param)
        parameters->getFloat("Phasor Monitor") = ofMap(phasorMod, 0, 1, minVal_Param, maxVal_Param);
    return (float)ofMap(phasorMod, 0, 1, minVal_Param, maxVal_Param);
}

void phasorClass::resetPhasor(bool &reset){
    phasor = initPhase_Param;
    phasorMod = initPhase_Param;
    resetPhase_Param = false;
}

void phasorClass::audioIn(int bufferSize){
    if(loop_Param && !offlineMode_Param){
        //tue phasor that goes from 0 to 1 at desired frequency
        double freq = (double)bpm_Param/(double)60;
        freq = freq * (double)beatsMult_Param;
        //TODO: This is not working
        freq = (double)freq / (double)beatsDiv_Param;
        
        double increment = (1.0f/(double)(((double)44100/(double)bufferSize)/(double)freq));
        
        // We want to use half speed with bounce param,
        // becouse we will make a triangle wave and it
        // will go and return with the same period
        // it we don't change it
        
        phasor = bounce_Param ? phasor + increment/2 : phasor + increment;
        phasor -= (int)phasor;
        
        //Assign a copy of the phasor to make some modifications
        phasorMod = phasor;
        
        //We make a triangle Wave
        if(bounce_Param)
            phasorMod = 1-(fabs((phasor * (-2))+ 1));
        
        //take the initPhase_Param as a phase offset param
        phasorMod += initPhase_Param;
        if(phasorMod >= 1.0)
            phasorMod -= 1.0;
        
        
        //Quantization -- only get the values we are interested
        if(quant_Param != 40){
            phasorMod = (int)(phasorMod*quant_Param);
            phasorMod /= quant_Param;
        }
    }
}

void phasorClass::nextFrame(){
    if(offlineMode_Param){
        //tue phasor that goes from 0 to 1 at desired frequency
        double freq = (double)bpm_Param/(double)60;
        freq = freq * (double)beatsMult_Param;
        //TODO: This is not working
        freq = (double)freq / (double)beatsDiv_Param;
        
        double increment = (1.0f/(double)(((double)60.0)/(double)freq));
        
        // We want to use half speed with bounce param,
        // becouse we will make a triangle wave and it
        // will go and return with the same period
        // it we don't change it
        
        phasor = bounce_Param ? phasor + increment/2 : phasor + increment;
        phasor -= (int)phasor;
        
        //Assign a copy of the phasor to make some modifications
        phasorMod = phasor;
        
        //We make a triangle Wave
        if(bounce_Param)
            phasorMod = 1-(fabs((phasor * (-2))+ 1));
        
        //take the initPhase_Param as a phase offset param
        phasorMod += initPhase_Param;
        if(phasorMod >= 1.0)
            phasorMod -= 1.0;
        
        
        //Quantization -- only get the values we are interested
        if(quant_Param != 40){
            phasorMod = (int)(phasorMod*quant_Param);
            phasorMod /= quant_Param;
        }
        parameters->getFloat("Phasor Monitor") = ofMap(phasorMod, 0, 1, minVal_Param, maxVal_Param);
    }
}

void phasorClass::loopChanged(bool &val){
    if(!val){
        phasorMod = initPhase_Param;
    }
}

void phasorClass::initPhaseChanged(float &f){
    if(loop_Param == false){
        phasorMod = initPhase_Param;
    }

}
