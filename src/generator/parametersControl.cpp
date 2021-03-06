//
//  parametersControl.cpp
//  DGTL_Generator
//
//  Created by Eduard Frigola on 03/08/16.
//
//

#include "parametersControl.h"
#include <random>


parametersControl::~parametersControl(){
    datGuis.clear();
    parameterGroups.clear();
    connections.clear();
}

void parametersControl::createGuiFromParams(ofParameterGroup *paramGroup, ofColor guiColor, ofPoint pos){
    
    ofxDatGuiLog::quiet();
    ofxDatGui::setAssetPath("");
    
    //Put parameterGroup into vector
    parameterGroups.push_back(paramGroup);
    
    if(ofStringTimesInString(paramGroup->getName(), "senderManager") != 0){
        senderGroups.push_back(paramGroup);
    }
    
    if(ofStringTimesInString(paramGroup->getName(), "phasor") != 0 && paramGroup->getFloat("BPM").getName() == "BPM" && datGui != nullptr)
        paramGroup->getFloat("BPM") = datGui->getSlider("Global BPM")->getValue();
    
    shared_ptr<ofxDatGui> tempDatGui = make_shared<ofxDatGui>(0,0);
//    tempDatGui->setAutoDraw(false);
    
    tempDatGui->setTransformMatrix(transformMatrix);
    
    ofxDatGuiTheme* theme = new ofxDatGuiThemeCharcoal;
    theme->color.slider.fill = guiColor;
    theme->color.textInput.text = guiColor;
    theme->color.icons = guiColor;
    tempDatGui->setTheme(theme);
    
    tempDatGui->setWidth(290);
    tempDatGui->addHeader(paramGroup->getName());
    tempDatGui->addFooter();
    if(pos == ofPoint(-1, -1)){
        if(datGuis.size() == 0)
            tempDatGui->setPosition(0, 0);
        else
            tempDatGui->setPosition((datGuis[datGuis.size()-1]->getPosition().x + datGuis[datGuis.size()-1]->getWidth()) + 20, 0);
    }else{
        tempDatGui->setPosition(pos.x, pos.y);
    }
    
    for(int i=0 ; i<paramGroup->size(); i++){
        ofAbstractParameter &absParam = paramGroup->get(i);
        if(absParam.type() == typeid(ofParameter<float>).name())
            tempDatGui->addSlider(paramGroup->getFloat(i))->setPrecision(1000);
        else if(absParam.type() == typeid(ofParameter<int>).name())
            tempDatGui->addSlider(paramGroup->getInt(i));
        else if(absParam.type() == typeid(ofParameter<bool>).name())
            tempDatGui->addToggle(paramGroup->getName(i))->setChecked(paramGroup->getBool(i).get());
        else if(absParam.type() == typeid(ofParameter<string>).name()){
            if(ofSplitString(absParam.getName(), "_").size() > 1){
                if(ofSplitString(absParam.getName(), "_")[1] == "label"){
                    ofxDatGuiLabel* l = tempDatGui->addLabel(ofSplitString(absParam.getName(), "_")[0]);
                    l->setStripe(ofColor(255,128,0), 4);
                    l->setStripeVisible(true);
                }else if(ofSplitString(absParam.getName(), "_")[1] == "paragraph"){
                    tempDatGui->addParagraph(ofSplitString(absParam.getName(), "_")[0], absParam.cast<string>());
                }
            }
            else
                tempDatGui->addTextInput(absParam.getName(), absParam.cast<string>());
        }
        else if(absParam.type() == typeid(ofParameter<ofColor>).name())
            tempDatGui->addColorPicker(paramGroup->getName(i), absParam.cast<ofColor>());
        else if(absParam.type() == typeid(ofParameterGroup).name()){
            tempDatGui->addLabel(paramGroup->getGroup(i).getName());
            tempDatGui->addDropdown(paramGroup->getGroup(i).getName(), ofSplitString(paramGroup->getGroup(i).getString(0), "-|-"))->select(paramGroup->getGroup(i).getInt(1));
        }
        else if(absParam.type() == typeid(ofParameter<vector<vector<bool>>>).name()){
            
        }else{
            if(absParam.getName() != "ReindexGrid")
                tempDatGui->addLabel(absParam.getName());
        }
    }
    
    //GUIS EVENT LISTERNERS
    tempDatGui->onButtonEvent(this, &parametersControl::onGuiButtonEvent);
    tempDatGui->onToggleEvent(this, &parametersControl::onGuiToggleEvent);
    tempDatGui->onDropdownEvent(this, &parametersControl::onGuiDropdownEvent);
    //        gui->onSliderEvent(this, &parametersControl::onGuiSliderEvent);
    tempDatGui->onTextInputEvent(this, &parametersControl::onGuiTextInputEvent);
    tempDatGui->onColorPickerEvent(this, &parametersControl::onGuiColorPickerEvent);
    tempDatGui->onRightClickEvent(this, &parametersControl::onGuiRightClickEvent);
    tempDatGui->onParagraphEvent(this, &parametersControl::onGuiParagraphEvent);
    
    //OF PARAMETERS LISTERENRS
    ofAddListener(paramGroup->parameterChangedE(), this, &parametersControl::listenerFunction);
    
    datGuis.push_back(tempDatGui);
//    delete theme;
}

void parametersControl::setup(){
    
    arrowCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    
    auto createCursorFromImage = [](string filename)->GLFWcursor*{
        ofImage img;
        img.load("Cursors/" + filename);
        GLFWimage glfwImg;
        glfwImg.height = img.getHeight();
        glfwImg.width = img.getWidth();
        glfwImg.pixels = img.getPixels().getData();
        return glfwCreateCursor(&glfwImg, 0, 0);
    };
    
    openedHandCursor = createCursorFromImage("openedHand.png");
    closedHandCursor = createCursorFromImage("closedHand.png");
    arrowListCursor = createCursorFromImage("arrowList.png");
    zoomInCursor = createCursorFromImage("zoomIn.png");
    zoomOutCursor = createCursorFromImage("zoomOut.png");
    crossCursor = createCursorFromImage("cross.png");
    
    ofAddListener(ofEvents().update, this, &parametersControl::update);
    ofAddListener(ofEvents().draw, this, &parametersControl::draw);
    
    ofRegisterKeyEvents(this);
    ofRegisterMouseEvents(this);
    
    
    //DatGui
    
    ofxDatGuiLog::quiet();
    ofxDatGui::setAssetPath("");
    
    datGui = new ofxDatGui();
//    datGui->setAutoDraw(false);
    
    mainGuiTheme = new ofxDatGuiThemeCharcoal;
    ofColor randColor =  ofColor::indianRed;
    mainGuiTheme->color.slider.fill = randColor;
    mainGuiTheme->color.textInput.text = randColor;
    mainGuiTheme->color.icons = randColor;
    datGui->setTheme(mainGuiTheme);//, true);
    
    datGui->setWidth(290);
    datGui->addHeader("Presets Control");
    datGui->addFooter();

    datGui->addButton("Global Reset Phase");
    datGui->addSlider("Global BPM", 0, 999, 120);
    
    //Preset Control
    ofDirectory dir;
    vector<string> banks;
    dir.open("Presets");
    for(int i = 0; i < dir.listDir() ; i++)
        banks.push_back(dir.getName(i));
    banks.push_back(" -- NEW BANK -- ");
    bankSelect = datGui->addDropdown("Bank Select", banks);
    bankSelect->select(0);
    datGui->addLabel("<== Presets List ==>")->setStripe(ofColor::red, 10);
    
    presetsList = datGui->addScrollView("test", 10);
    
    loadBank();
    
    datGui->addTextInput("New Preset");
    
    
    datGui->addToggle("BPM Sync")->setChecked(false);
    //datGui->addToggle("Automatic Preset");
    //datGui->addButton("Reload Sequence");
    datGui->addSlider(fadeTime.set("Fade Time", 0, 0, 10));
//    datGui->addSlider(presetChangeBeatsPeriod.set("Beats Period", 4, 1, 120));
    
    datGui->addLabel("Midi settings");
    datGui->addToggle("Midi Learn", false);
    midiListenActive = false;
    
    datGui->setPosition(ofxDatGuiAnchor::BOTTOM_LEFT);

    
    //ControlGui Events
    datGui->onButtonEvent(this, &parametersControl::onGuiButtonEvent);
    datGui->onToggleEvent(this, &parametersControl::onGuiToggleEvent);
    datGui->onSliderEvent(this, &parametersControl::onGuiSliderEvent);
    datGui->onDropdownEvent(this, &parametersControl::onGuiDropdownEvent);
    datGui->onScrollViewEvent(this, &parametersControl::onGuiScrollViewEvent);
    datGui->onTextInputEvent(this, &parametersControl::newPresetListener);

    
    //OSC
    ofLog() << "OSC Info:";
    ofXml xml;
    if(xml.load("OSCConfig.xml")){
        string host = xml.getValue("host");
        if(host != ""){
            oscReceiver.setup(xml.getIntValue("ReceiverPort"));
            oscSender.setup(host, xml.getIntValue("SenderPort"));
            ofLog() << "Listening on port " << xml.getIntValue("ReceiverPort");
            ofLog() << "Sending to host: " << host << " on port: " << xml.getIntValue("SenderPort");
        }else
            ofLog() << "NO OSC";
    }
    
    //MIDI
    ofLog() << "MIDI Info:";
    ofxMidiOut::listPorts();
    for(int i = 0; i< ofxMidiOut::getNumPorts(); i++){
        midiOut.push_back(ofxMidiOut());
        midiOut.back().openPort(i);
    }
    
    
    ofxMidiIn::listPorts();
    for(int i = 0; i< ofxMidiIn::getNumPorts(); i++){
        midiIn.push_back(ofxMidiIn());
        midiIn.back().openPort(i);
        midiIn.back().addListener(this);
    }
    
    
    autoPreset = false;
    
    datGui->getButton("Reload Sequence")->setBackgroundColor(loadPresetsSequence() ? ofColor::green : ofColor::red);
    
    Tweenzor::init();
    
    beatTracker = &bpmControl::getInstance();
    
    
    ///POP UP MENuS
    popUpMenu = new ofxDatGui();
    popUpMenu->setVisible(false);
    popUpMenu->setPosition(-1, -1);
    ofColor randColor2 =  ofColor::indianRed;
//    theme->color.slider.fill = randColor2;
//    theme->color.textInput.text = randColor2;
//    theme->color.icons = randColor2;
    popUpMenu->setTheme(mainGuiTheme);
    popUpMenu->addDropdown("Choose module", {"Phasor", "Oscillator", "Oscillator Bank", "Oscillator Bank Group", "Envelope Generator", "Midi Gate In", "Delta", "Expression Operator", "Mapper", "Vector Mapper", "Manual Osc Bank", "Type Converter", "Vector Getter", "Vector Chainer", "Smoother", "Vec Smoother", "Vector Operations", "subDimensionCombinator", "bankDimensionCombinator", "bidimensionalOscillatorBank"})->expand();
    
    popUpMenu->onDropdownEvent(this, &parametersControl::newModuleListener);
}


void parametersControl::update(ofEventArgs &args){

    Tweenzor::update(ofGetElapsedTimeMillis());
    
    while(oscReceiver.hasWaitingMessages()){
        ofxOscMessage m;
        oscReceiver.getNextMessage(m);
        
        vector<string> splitAddress = ofSplitString(m.getAddress(), "/");
        if(splitAddress.size() >= 2){
            ofStringReplace(splitAddress[1], "_", " ");
            if(splitAddress[1] == "presetLoad"){
                loadPreset(m.getArgAsString(0), m.getArgAsString(1));
            }else if(splitAddress[1] == "presetLoadi"){
                string bankName = m.getArgAsString(0);
                
                ofDirectory dir;
                map<int, string> presets;
                dir.open("Presets/" + bankName);
                if(!dir.exists())
                    dir.createDirectory("Presets/" + bankName);
                dir.sort();
                int numPresets = dir.listDir();
                for ( int i = 0 ; i < numPresets; i++)
                    presets[ofToInt(ofSplitString(dir.getName(i), "|")[0])] = ofSplitString(dir.getName(i), ".")[0];
                
                loadPreset(presets[m.getArgAsInt(1)], bankName);
            }else if(splitAddress[1] == "presetSave"){
                savePreset(m.getArgAsString(0), m.getArgAsString(1));
            }else if(splitAddress[1] == "phaseReset"){
                for(auto groupParam : parameterGroups){
                    if(ofStringTimesInString(groupParam->getName(), "phasor") != 0)
                        groupParam->getBool("Reset Phase") = 0;
                }
            }else if(splitAddress[1] == "bpm"){
                setGlobalBPM(m.getArgAsFloat(0));
            }else if(splitAddress[1] == "nextFrame"){
                ofNotifyEvent(nextFrameEvent, this);
            }else{
                for(auto groupParam : parameterGroups){
                    if(groupParam->getName() == splitAddress[1]){
                        ofAbstractParameter &absParam = groupParam->get(splitAddress[2]);
                        if(absParam.type() == typeid(ofParameter<float>).name()){
                            ofParameter<float> castedParam = absParam.cast<float>();
                            castedParam = ofMap(m.getArgAsFloat(0), 0, 1, castedParam.getMin(), castedParam.getMax(), true);
                        }else if(absParam.type() == typeid(ofParameter<int>).name()){
                            ofParameter<int> castedParam = absParam.cast<int>();
                            castedParam = ofMap(m.getArgAsFloat(0), 0, 1, castedParam.getMin(), castedParam.getMax(), true);
                        }else if(absParam.type() == typeid(ofParameter<bool>).name())
                            groupParam->getBool(splitAddress[2]) = m.getArgAsBool(0);
                        else if(absParam.type() == typeid(ofParameter<string>).name())
                            groupParam->getString(splitAddress[2]) = m.getArgAsString(0);
                        else
                            groupParam->getGroup(splitAddress[2]).getInt(1) = m.getArgAsInt(0); //DropDown
                    }
                }
            }
        }
    }
    
//    if(isFading){
//        ofxOscMessage m;
//        m.setAddress("presetFade");
//        m.addFloatArg(parameterGroups[parameterGroups.size()-1]->getFloat("Master Fader").get());
//        oscSender.sendMessage(m);
//    }
    
    //MIDI - Process the midi deque
    for(auto m : midiMessagesFiller)
        midiMessages.push_front(m);
    midiMessagesFiller.clear();
    
    while(midiMessages.size() > 0){
        int parameterChan = midiMessages[0].channel;
        int parameterNum = midiMessages[0].status == MIDI_NOTE_ON ? midiMessages[0].pitch : midiMessages[0].control;
        int parameterVal = midiMessages[0].status == MIDI_NOTE_ON ? midiMessages[0].velocity : midiMessages[0].value;
        string parameterPort = midiMessages[0].portName;
        
        for (auto &connection : midiIntConnections){
            if(connection.isListening())
                connection.assignMidiControl(parameterPort, parameterChan, parameterNum);
            else if(connection.getDevice() == parameterPort && connection.getChannel() == parameterChan && connection.getControl() == parameterNum){
                connection.setValue(parameterVal);
            }
        }
        for (auto &connection : midiFloatConnections){
            if(connection.isListening())
                connection.assignMidiControl(parameterPort, parameterChan, parameterNum);
            else if(connection.getDevice() == parameterPort && connection.getChannel() == parameterChan && connection.getControl() == parameterNum){
                connection.setValue(parameterVal);
            }
        }
        for (auto &connection : midiBoolConnections){
            if(connection.isListening())
                connection.assignMidiControl(parameterPort, parameterChan, parameterNum);
            else if(connection.getDevice() == parameterPort && connection.getChannel() == parameterChan && connection.getControl() == parameterNum){
                connection.setValue(parameterVal);
            }
        }
        
        midiMessages.pop_front();
    }

    
    //Auto preset
//    if(randomPresetsArrange.size()>0 && autoPreset && (ofGetElapsedTimef()-presetChangedTimeStamp) > periodTime){
//        presetChangedTimeStamp = presetChangedTimeStamp+periodTime;
//        int index = randomPresetsArrange[presetChangeCounter];
//        loadPresetWithFade(presetNumbersAndBanks.at(index).first, presetNumbersAndBanks.at(index).second);
//        periodTime = presetsTime[index];
//        presetChangeCounter++;
//        if(presetChangeCounter >= randomPresetsArrange.size()){
//            presetChangeCounter = 0;
//            mt19937 g(static_cast<uint32_t>(time(0)));
//            shuffle(randomPresetsArrange.begin(), randomPresetsArrange.end(), g);
//        }
//    }
    
    if(newBpm != 0){
        setGlobalBPM(newBpm);
        newBpm = 0;
    }

}

void parametersControl::draw(ofEventArgs &args){
    ofPushMatrix();
    ofMultMatrix(transformMatrix);
    ofPushStyle();
    ofSetColor(ofColor::white);
    ofSetLineWidth(2);
    for(auto connection : connections){
        ofPath path = connection->getPath();
        if(connection->closedLine){
            if(connection->getParentGuis(1)->getHeader()->getName() == "waveScope"){
                path.setColor(ofColor(50,50,50));
            }
            if(connection->getMin() != 0 || connection->getMax() != 1)
                path.setColor(ofColor::navajoWhite);
        }
        path.draw();
    }
    ofPopStyle();
    ofPopMatrix();
    
    if(ofGetKeyPressed('r')){
        ofPushStyle();
        ofSetColor(255, 0, 0, 50);
        ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
        ofPopStyle();
    }
    else if(midiListenActive){
        ofPushStyle();
        ofSetColor(ofColor::greenYellow,50);
        ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
        ofSetColor(ofColor::black);
        ofDrawBitmapStringHighlight("MIDI LEARN", ofPoint(ofGetWidth() - 100, ofGetHeight() - 10));
        ofPopStyle();
    }
}

void parametersControl::saveGuiArrangement(){
    /*
    //xml.load("Preset_"+ofToString(presetNum)+".xml");
    xml.clear();
    
    //Root Element
    xml.addChild("GUIARRANGEMENT");
    
    // now we set our current element so we're on the right
    // element, so when add new nodes, they are still inside
    //the root element
    xml.setTo("GUIARRANGEMENT");
    
    xml.addChild("GuiWindow");
    xml.setTo("GuiWindow");
    xml.addValue("x", guiWindow->getWindowPosition().x);
    xml.addValue("y", guiWindow->getWindowPosition().y);
    xml.addValue("width", guiWindow->getWindowSize().x);
    xml.addValue("height", guiWindow->getWindowSize().y);
    xml.setToParent();
    
    xml.addChild("PrevWindow");
    xml.setTo("PrevWindow");
    xml.addValue("x", prevWindow->getWindowPosition().x);
    xml.addValue("y", prevWindow->getWindowPosition().y);
    xml.addValue("width", prevWindow->getWindowSize().x);
    xml.addValue("height", prevWindow->getWindowSize().y);
    xml.setToParent();
    
    xml.addChild("Main");
    xml.setTo("Main");
    xml.addValue("x", ofToString(datGui->getPosition().x));
    xml.addValue("y", ofToString(datGui->getPosition().y));
    xml.setToParent();
    
    for ( int i = 0; i < datGuis.size() ; i++){
        xml.addChild("Gui_" + ofToString(i));
        xml.setTo("Gui_" + ofToString(i));
        xml.addValue("x", ofToString(datGuis[i]->getPosition().x));
        xml.addValue("y", ofToString(datGuis[i]->getPosition().y));
        xml.setToParent();
    }
    xml.save("GuiArrangement_"+bankSelect->getSelected()->getName()+".xml");
    */
}

void parametersControl::loadGuiArrangement(){
    //Test if there is no problem with the file
    /*if(!xml.load("GuiArrangement_"+bankSelect->getSelected()->getName()+".xml"))
        return;
    
    if(xml.exists("GuiWindow")){
        xml.setTo("GuiWindow");
        guiWindow->setWindowPosition(xml.getIntValue("x"), xml.getIntValue("y"));
        guiWindow->setWindowShape(xml.getIntValue("width"), xml.getIntValue("height"));
        xml.setToParent();
    }
    
    if(xml.exists("PrevWindow")){
        xml.setTo("PrevWindow");
        prevWindow->setWindowPosition(xml.getIntValue("x"), xml.getIntValue("y"));
        prevWindow->setWindowShape(xml.getIntValue("width"), xml.getIntValue("height"));
        xml.setToParent();
    }

    if(xml.exists("Main")){
        xml.setTo("Main");
        datGui->setPosition(xml.getIntValue("x"), xml.getIntValue("y"));
        xml.setToParent();
    }

    for ( int i = 0; i < datGuis.size() ; i++){
        if(xml.exists("Gui_" + ofToString(i))){
            xml.setTo("Gui_" + ofToString(i));
            datGuis[i]->setPosition(xml.getIntValue("x"), xml.getIntValue("y"));
            xml.setToParent();
        }
    }
    
    ofLog()<<"Load Arrangement_" + bankSelect->getSelected()->getName();
     */
}

void parametersControl::loadBank(){
    string bankName = bankSelect->getSelected()->getName();
    
    ofDirectory dir;
    vector<pair<int, string>> presets;
    dir.open("Presets/" + bankName);
    if(!dir.exists())
        dir.createDirectory("Presets/" + bankName);
    dir.sort();
    int numPresets = dir.listDir();
    ofLog() << "Dir size: " << ofToString(numPresets);
    for ( int i = 0 ; i < numPresets; i++)
        presets.push_back(pair<int, string>(ofToInt(ofSplitString(dir.getName(i), "|")[0]), ofSplitString(dir.getName(i), ".")[0]));
    
    std::sort(presets.begin(), presets.end(), [](pair<int, string> &left, pair<int, string> &right) {
        return left.first< right.first;
    });
    
    presetsList->clear();
    
    for(auto preset : presets)
        presetsList->add(preset.second);
}

void parametersControl::changePresetLabelHighliht(ofxDatGuiButton *presetToHighlight){
    if(oldPresetButton != nullptr) oldPresetButton->setTheme(mainGuiTheme);
    presetToHighlight->setLabelColor(ofColor::red);
    oldPresetButton = presetToHighlight;
}


void parametersControl::setGlobalBPM(float bpm){
    datGui->getSlider("Global BPM")->setValue(bpm);
    for(auto groupParam : parameterGroups){
        if(ofStringTimesInString(groupParam->getName(), "phasor") != 0 && groupParam->getFloat("BPM").getName() == "BPM")
            groupParam->getFloat("BPM") = bpm;
    }
}

bool parametersControl::loadPresetsSequence(){
    // this is our buffer to stroe the text data
    ofBuffer buffer = ofBufferFromFile("PresetsSequencing.txt");
    
    if(buffer.size()) {
        presetNumbersAndBanks.clear();
        presetsTime.clear();
        for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
            string line = *it;
            
            // make sure its not a empty line
            if(!line.empty()){
                vector<string> splitedStr = ofSplitString(line,"-");
                pair<int, string> tempPair;
                tempPair.first = ofToInt(splitedStr[0]);
                tempPair.second = splitedStr[1];
                presetNumbersAndBanks.push_back(tempPair);
                presetsTime.push_back(ofToInt(splitedStr[2]));
            }
        }
    }
    else{
        return false;
    }
    
    if(presetNumbersAndBanks.size() != presetsTime.size()){
        presetNumbersAndBanks.clear();
        presetsTime.clear();
        return false;
    }
    
    randomPresetsArrange.clear();
    for(int i = 0 ; i < presetNumbersAndBanks.size(); i++)
        randomPresetsArrange.push_back(i);
    
    presetChangeCounter = 0;
    presetChangedTimeStamp = ofGetElapsedTimef();
    
    mt19937 g(static_cast<uint32_t>(time(0)));
    shuffle(randomPresetsArrange.begin(), randomPresetsArrange.end(), g);
    
    return true;
}

void parametersControl::saveMidiMapping(string presetName, string bank){
    ofXml xml2;
    xml2.clear();
    
    //Root Element
    xml2.addChild("MIDIMAPPING");
    
    // now we set our current element so we're on the right
    // element, so when add new nodes, they are still inside
    //the root element
    xml2.setTo("MIDIMAPPING");
    
    for (int i = 0; i < midiIntConnections.size(); i++){
        xml2.addChild("MidiInt_" + ofToString(i));
        xml2.setTo("MidiInt_" + ofToString(i));
        if(midiIntConnections[i].getParameter()->getGroupHierarchyNames().size() == 3){
            xml2.addValue("isDropdown", "true");
            string sGroup = midiIntConnections[i].getParameter()->getGroupHierarchyNames()[0];
            ofStringReplace(sGroup, "_", " ");
            xml2.addValue("SourceGroup", sGroup);
            xml2.addValue("SourceName", midiIntConnections[i].getParameter()->getFirstParent().getName());
        }else{
            xml2.addValue("isDropdown", "false");
            xml2.addValue("SourceGroup", midiIntConnections[i].getParameter()->getFirstParent().getName());
            xml2.addValue("SourceName", midiIntConnections[i].getParameter()->getName());
        }
        xml2.addValue("MidiDevice", midiIntConnections[i].getDevice());
        xml2.addValue("MidiChannel", ofToString(midiIntConnections[i].getChannel()));
        xml2.addValue("MidiControl", ofToString(midiIntConnections[i].getControl()));
        xml2.setToParent();
    }
    
    for (int i = 0; i < midiFloatConnections.size(); i++){
        xml2.addChild("MidiFloat_" + ofToString(i));
        xml2.setTo("MidiFloat_" + ofToString(i));
        xml2.addValue("SourceGroup", midiFloatConnections[i].getParameter()->getFirstParent().getName());
        xml2.addValue("SourceName", midiFloatConnections[i].getParameter()->getName());
        xml2.addValue("MidiDevice", midiFloatConnections[i].getDevice());
        xml2.addValue("MidiChannel", ofToString(midiFloatConnections[i].getChannel()));
        xml2.addValue("MidiControl", ofToString(midiFloatConnections[i].getControl()));
        xml2.setToParent();
    }
    
    for (int i = 0; i < midiBoolConnections.size(); i++){
        xml2.addChild("MidiBool_" + ofToString(i));
        xml2.setTo("MidiBool_" + ofToString(i));
        xml2.addValue("SourceGroup", midiBoolConnections[i].getParameter()->getFirstParent().getName());
        xml2.addValue("SourceName", midiBoolConnections[i].getParameter()->getName());
        xml2.addValue("MidiDevice", midiBoolConnections[i].getDevice());
        xml2.addValue("MidiChannel", ofToString(midiBoolConnections[i].getChannel()));
        xml2.addValue("MidiControl", ofToString(midiBoolConnections[i].getControl()));
        xml2.addValue("MidiToggleMode", ofToString(midiBoolConnections[i].isToggle()));
        xml2.setToParent();
    }
    
    xml2.save("Presets/" + bank + "/.midi/" + presetName + "_midimap.xml");
}

void parametersControl::loadMidiMapping(string presetName, string bank){
    midiIntConnections.clear();
    midiFloatConnections.clear();
    midiBoolConnections.clear();
    
    //Test if there is no problem with the file
    if(!xml.load("Presets/" + bank + "/.midi/" + presetName + "_midimap.xml"))
        return;
    
    int i = 0;
    while(xml.exists("MidiInt_" + ofToString(i))){
        xml.setTo("MidiInt_" + ofToString(i));
        string isDrop = xml.getValue("isDropdown");
        for(auto &paramGroup : parameterGroups){
            if(paramGroup->getName() == xml.getValue("SourceGroup")){
                if(isDrop == "true")
                    midiIntConnections.push_back(midiConnection<int>(&paramGroup->getGroup(xml.getValue("SourceName")).getInt(1)));
                else
                    midiIntConnections.push_back(midiConnection<int>(&paramGroup->getInt(xml.getValue("SourceName"))));
            }
            
        }
        midiIntConnections.back().assignMidiControl(xml.getValue("MidiDevice"), xml.getIntValue("MidiChannel"), xml.getIntValue("MidiControl"));
        xml.setToParent();
        i++;
    }
    i = 0;
    while(xml.exists("MidiFloat_" + ofToString(i))){
        xml.setTo("MidiFloat_" + ofToString(i));
        for(auto &paramGroup : parameterGroups){
            if(paramGroup->getName() == xml.getValue("SourceGroup")){
                midiFloatConnections.push_back(midiConnection<float>(&paramGroup->getFloat(xml.getValue("SourceName"))));
                midiFloatConnections.back().assignMidiControl(xml.getValue("MidiDevice"), xml.getIntValue("MidiChannel"), xml.getIntValue("MidiControl"));
            }
        }
//        midiFloatConnections.back().assignMidiControl(xml.getValue("MidiDevice"), xml.getIntValue("MidiChannel"), xml.getIntValue("MidiControl"));
        xml.setToParent();
        i++;
    }
    i = 0;
    while(xml.exists("MidiBool_" + ofToString(i))){
        xml.setTo("MidiBool_" + ofToString(i));
        for(auto &paramGroup : parameterGroups){
            if(paramGroup->getName() == xml.getValue("SourceGroup"))
                midiBoolConnections.push_back(midiConnection<bool>(&paramGroup->getBool(xml.getValue("SourceName"))));
        }
        midiBoolConnections.back().assignMidiControl(xml.getValue("MidiDevice"), xml.getIntValue("MidiChannel"), xml.getIntValue("MidiControl"));
        midiBoolConnections.back().setToggle(xml.getBoolValue("MidiToggleMode"));
        
        xml.setToParent();
        i++;
    }
}

void parametersControl::savePreset(string presetName, string bank){
    //xml.load("Preset_"+ofToString(presetNum)+".xml");
    xml.clear();
    
    //Root Element
    xml.addChild("PRESET");
    
    // now we set our current element so we're on the right
    // element, so when add new nodes, they are still inside
    //the root element
    xml.setTo("PRESET");
    
    xml.addChild("DYNAMIC_NODES");
    xml.setTo("DYNAMIC_NODES");
    
    vector<ofPoint> toCreatePhasors;
    vector<ofPoint> toCreateOscillators;
    vector<ofPoint> toCreateOscillatorBanks;
    int toCreateWaveScope = 0;
    ofPoint toCreateColorApplier = ofPoint(-1, -1);
    
    int modulesToCreate = 0;
    
    for(int i = 0; i < parameterGroups.size() ; i++){
        string moduleName = ofSplitString(parameterGroups[i]->getName(), " ")[0];
        ofXml tempXml;
        tempXml.addChild("MODULE");
        tempXml.setTo("MODULE");
        tempXml.addValue("name", parameterGroups[i]->getName());
        ofPoint modulePosition = datGuis[i]->getPosition();
        if(moduleName == "oscillatorBank" || moduleName == "manualOscillatorBank")
            modulePosition.z = parameterGroups[i]->getInt("Index Modulo").getMax();
        else if(moduleName == "typeConverter"){
            modulePosition.z = ofToInt(ofSplitString(parameterGroups[i]->getName(0), " ")[0]);
        }
        else if(moduleName == "vectorChain" || moduleName == "expressionOperator"){
            modulePosition.z = 0;
            while(datGuis[i]->getLabel("Input "+ofToString((int)modulePosition.z))->getName() != "X")
                modulePosition.z += 1;
        }else if(moduleName == "bidimensionalOscillatorBank"){
            int xSize = parameterGroups[i]->getInt("Index Modulo X").getMax();
            int ySize = parameterGroups[i]->getInt("Index Modulo Y").getMax();
            modulePosition.z = (float)xSize + ((float)ySize / 1000.0);
        }
        tempXml.addValue("pos", ofToString(modulePosition.x) + "_" + ofToString(modulePosition.y) + "_" + ofToString(modulePosition.z));
        modulesToCreate++;
        xml.addXml(tempXml);
//        xml.setToParent();
    }
    xml.setToParent();
    
    
    
    //Iterate for all the parameterGroups
    for (auto groupParam : parameterGroups){
        //set XML structure to parameterGroup
        string noSpacesGroupName = groupParam->getName();
        if((ofSplitString(noSpacesGroupName, " ")[0] != "waveScope" && ofSplitString(noSpacesGroupName, " ")[0] != "senderManager")){
            ofStringReplace(noSpacesGroupName, " ", "_");
            xml.addChild(noSpacesGroupName);
            xml.setTo(noSpacesGroupName);
            
            //Iterate for all parameters in parametergroup and look for the type of the parameter
            for (int j = 0; j < groupParam->size() ; j++){
                ofAbstractParameter &absParam = groupParam->get(j);
                if(absParam.type() == typeid(ofParameter<float>).name()){
                    //Cast it
                    ofParameter<float> castedParam = absParam.cast<float>();
                    
                    //Replace blank spaces with underscore
                    string noSpaces = castedParam.getName();
                    ofStringReplace(noSpaces, " ", "_");
                    
                    //add the value of that parameter into xml
                    xml.addValue(noSpaces, ofMap(castedParam.get(), castedParam.getMin(), castedParam.getMax(), 0, 1, true));
                }
                else if(absParam.type() == typeid(ofParameter<int>).name()){
                    ofParameter<int> castedParam = absParam.cast<int>();
                    string noSpaces = castedParam.getName();
                    ofStringReplace(noSpaces, " ", "_");
                    xml.addValue(noSpaces, ofMap(castedParam.get(), castedParam.getMin(), castedParam.getMax(), 0, 1, true));
                }
                else if(absParam.type() == typeid(ofParameter<bool>).name()){
                    ofParameter<bool> castedParam = absParam.cast<bool>();
                    string noSpaces = castedParam.getName();
                    ofStringReplace(noSpaces, " ", "_");
                    xml.addValue(noSpaces, castedParam.get());
                }
                else if(absParam.type() == typeid(ofParameter<string>).name()){
                    if(ofSplitString(absParam.getName(), "_").back() != "label"){
                        ofParameter<string> castedParam = absParam.cast<string>();
                        string noSpaces = castedParam.getName();
                        ofStringReplace(noSpaces, " ", "_");
                        xml.addValue(noSpaces, castedParam.get());
                    }
                }
                else if(absParam.type() == typeid(ofParameter<ofColor>).name()){
                    ofParameter<ofColor> castedParam = absParam.cast<ofColor>();
                    string noSpaces = castedParam.getName();
                    ofStringReplace(noSpaces, " ", "_");
                    xml.addValue(noSpaces, ofToString((int)castedParam.get().r) + "-" + ofToString((int)castedParam.get().g) + "-" + ofToString((int)castedParam.get().b));
                }
                else if(absParam.type() == typeid(ofParameterGroup).name()){
                    string noSpaces = groupParam->getGroup(j).getName();
                    ofStringReplace(noSpaces, " ", "_");
                    xml.addValue(noSpaces, groupParam->getGroup(j).getInt(1).get());
                }
                else if(absParam.getName() == "ReindexGrid"){
                    ofParameter<vector<vector<bool>>> castedParam = absParam.cast<vector<vector<bool>>>();
                    string matrixInfo;
                    for(int i = 0; i < castedParam.get().size(); i++){
                        for(int j = 0; j < castedParam.get().size(); j++){
                            matrixInfo += ofToString(castedParam.get()[i][j]);
                        }
                    }
                    xml.addValue(absParam.getName(), matrixInfo);
                }
            }
            xml.setToParent();
        }
    }
    
    
    xml.addChild("CONNECTIONS");
    xml.setTo("CONNECTIONS");
    for(int i = 0; i < connections.size(); i++){
        ofXml tempXml;
        tempXml.addChild("CONNECTION");
        tempXml.setTo("CONNECTION");
        
        vector<string> groupNames = connections[i]->getSourceParameter()->getGroupHierarchyNames();
        tempXml.addValue("source", groupNames[0] + "-|-" + groupNames[1]);
        
        groupNames = connections[i]->getSinkParameter()->getGroupHierarchyNames();
        tempXml.addValue("sink", groupNames[0] + "-|-" + groupNames[1]);
        
        float min = connections[i]->getMin();
        float max = connections[i]->getMax();
        tempXml.addValue("minmax", ofToString(min) + "-|-" + ofToString(max));
        xml.addXml(tempXml);
    }
    
    ofLog() <<"Save " << presetName;
    xml.save("Presets/" + bank + "/" + presetName + ".xml");
    
    saveMidiMapping(presetName, bank);
    
    ofxOscMessage m;
    m.setAddress("presetSave");
    m.addStringArg(presetName);
    m.addStringArg(bank);
    oscSender.sendMessage(m);
}

void parametersControl::loadPreset(string presetName, string bank){
    //Test if there is no problem with the file
    
    bool isColorLoaded = false;
    
    if(!xml.load("Presets/" + bank + "/" + presetName + ".xml"))
        return;
    
    vector<pair<string, ofPoint>> modulesToCreate;
    
    
    if(xml.exists("DYNAMIC_NODES")){
        xml.setTo("DYNAMIC_NODES");
        
        int i = 0;
        while(xml.exists("MODULE[" + ofToString(i) + "]")){
            xml.setTo("MODULE[" + ofToString(i) + "]");
            pair<string, ofPoint> newModule;
            newModule.first = xml.getValue("name");
            vector<string> strPoint = ofSplitString(xml.getValue("pos"), "_");
            newModule.second = ofPoint(ofToFloat(strPoint[0]), ofToFloat(strPoint[1]), ofToFloat(strPoint[2]));
            modulesToCreate.push_back(newModule);
            i++;
            xml.setToParent();
        }
    }
    
    xml.setToParent();
    
    bool newModulesCreated = false;
    
    connections.clear();
    
    //Iterate for all the parameterGroups
    for (int i = 0; i < parameterGroups.size();){
        auto &groupParam = parameterGroups[i];
        bool hasToBeDestroyed = false;
        
        //Move, edit or destroy nodes
        string moduleName = ofSplitString(groupParam->getName(), " ")[0];
        if(!newModulesCreated){
            hasToBeDestroyed = true;
            for(auto module : modulesToCreate){
                if(groupParam->getName() == module.first){
                    hasToBeDestroyed = false;
                    if(moduleName == "oscillatorBank" || moduleName == "manualOscillatorBank"){
                        if(groupParam->getInt("Index Modulo").getMax() != module.second.z){
                            hasToBeDestroyed = true;
                        }
                    }
                    if(moduleName == "typeConverter"){
                        if(ofToInt(ofSplitString(groupParam->getString(0), ".")[0]) != module.second.z){
                            hasToBeDestroyed = true;
                        }
                    }
                    if(moduleName == "vectorChain" || moduleName == "expressionOperator"){
                        int inputSize = 0;
                        while(datGuis[i]->getLabel("Input "+ofToString((int)inputSize))->getName() != "X")
                            inputSize += 1;
                        if(inputSize != module.second.z){
                            hasToBeDestroyed = true;
                        }
                    }
                    
                    if(!hasToBeDestroyed){
                        datGuis[i]->setPosition(module.second.x, module.second.y);
                        modulesToCreate.erase(remove(modulesToCreate.begin(), modulesToCreate.end(), module));
                    }
                    break;
                }
            }
        }
        
        //if we have to destroy the module we do it
        if(hasToBeDestroyed && moduleName != "senderManager" && moduleName != "waveScope" && moduleName != "audioControls" && moduleName != "colorApplier" && moduleName != "chartresTextureUnifier" && moduleName != "oscillatorGroup" && moduleName != "dataRecorder" && moduleName != "audioAnalyzer" && moduleName != "oscInput" && moduleName != "speakerPowerCalculator" && moduleName != "textureUnifier"){
            destroyModuleAndConnections(i);
        }
        else{
            if(moduleName != "waveScope" && moduleName != "senderManager" && moduleName != "typeConverter"){
                //Put xml in the place of the parametergroup
                string noSpacesGroupName = groupParam->getName();
                ofStringReplace(noSpacesGroupName, " ", "_");
                if(xml.exists(noSpacesGroupName)){
                    xml.setTo(noSpacesGroupName);
                    
                    //Iterate for all parameters in parametergroup and look for the type of the parameter
                    for (int j = 0; j < groupParam->size() ; j++){
                        ofAbstractParameter &absParam = groupParam->get(j);
                        if(absParam.type() == typeid(ofParameter<float>).name()){
                            //Cast it
                            ofParameter<float> castedParam = absParam.cast<float>();
                            
                            //Replace blank spaces with underscore
                            string noSpaces = castedParam.getName();
                            ofStringReplace(noSpaces, " ", "_");
                            
                            //get the value of that parameter if it's not bpm, we don't want to lose sync
                            if(castedParam.getName() != "BPM" && xml.exists(noSpaces) && !ofStringTimesInString(groupParam->getName(), "master") && castedParam.getName() != "Output" && castedParam.getName() != "Input" && castedParam.getName() != "Phasor Monitor" && !ofStringTimesInString(groupParam->getName(), "Amplitude"))
                                castedParam = ofMap(xml.getFloatValue(noSpaces), 0, 1, castedParam.getMin(), castedParam.getMax(), true);
                        }
                        else if(absParam.type() == typeid(ofParameter<int>).name()){
                            ofParameter<int> castedParam = absParam.cast<int>();
                            string noSpaces = castedParam.getName();
                            ofStringReplace(noSpaces, " ", "_");
                            if(xml.exists(noSpaces) && !ofStringTimesInString(groupParam->getName(), "master"))
                                castedParam = round(ofMap(xml.getFloatValue(noSpaces), 0, 1, castedParam.getMin(), castedParam.getMax(), true));
                        }
                        else if(absParam.type() == typeid(ofParameter<bool>).name()){
                            ofParameter<bool> castedParam = absParam.cast<bool>();
                            string noSpaces = castedParam.getName();
                            ofStringReplace(noSpaces, " ", "_");
                            if(xml.exists(noSpaces) && noSpaces != "Manual_Reindex" && !ofStringTimesInString(groupParam->getName(), "master"))
                                castedParam = xml.getValue(noSpaces, castedParam.get());
                        }
                        else if(absParam.type() == typeid(ofParameter<string>).name()){
                            ofParameter<string> castedParam = absParam.cast<string>();
                            string noSpaces = castedParam.getName();
                            ofStringReplace(noSpaces, " ", "_");
                            if(moduleName != "audioControls" && xml.exists(noSpaces) && !ofStringTimesInString(groupParam->getName(), "master")){
                                castedParam.set(xml.getValue(noSpaces, castedParam.get()));
                            }
                        }
                        else if(absParam.type() == typeid(ofParameter<ofColor>).name()){
                            ofParameter<ofColor> castedParam = absParam.cast<ofColor>();
                            string noSpaces = castedParam.getName();
                            ofStringReplace(noSpaces, " ", "_");
                            if(xml.exists(noSpaces)){
                                vector<string> colors = ofSplitString(xml.getValue(noSpaces), "-");
                                ofColor_<unsigned char> color = ofColor(ofToInt(colors[0]), ofToInt(colors[1]), ofToInt(colors[2]));
                                //TODO: hack, arreglar
//                                groupParam->getInt("R Channel") = ofToInt(colors[0]);
//                                groupParam->getInt("G Channel") = ofToInt(colors[1]);
//                                groupParam->getInt("B Channel") = ofToInt(colors[2]);
                                castedParam = color;
                                isColorLoaded = true;
                            }
                        }
                        else if(absParam.type() == typeid(ofParameterGroup).name()){
                            string noSpaces = groupParam->getGroup(j).getName();
                            ofStringReplace(noSpaces, " ", "_");
                            if(xml.exists(noSpaces) && !ofStringTimesInString(groupParam->getName(), "master"))
                                groupParam->getGroup(j).getInt(1) = xml.getValue(noSpaces, groupParam->getGroup(j).getInt(1));
                        }
                        else if(absParam.getName() == "ReindexGrid"){
                            ofParameter<vector<vector<bool>>> castedParam = absParam.cast<vector<vector<bool>>>();
                            
                            if(xml.exists(absParam.getName())){
                                string matrixInfo = xml.getValue(absParam.getName());
                                vector<vector<bool>> matrixCopy = castedParam.get();
                                for(int i = 0; i < castedParam.get().size(); i++){
                                    for(int j = 0; j < castedParam.get().size(); j++){
                                        matrixCopy[i][j] = matrixInfo[(i*matrixCopy.size()) + j] == '1' ? true : false;
                                    }
                                }
                                castedParam = matrixCopy;
                            }
                            
                        }
                    }
                    //Jump one label before in xml structure
                    xml.setToParent();
                    //reset Phasor
                    if(ofSplitString(groupParam->getName(), " ")[0] == "phasor")
                        groupParam->getBool("Reset Phase") = false;
                }
            }
            else if(moduleName == "waveScope"){
                int waveScopes = 0;
                while(datGuis[i]->getLabel("Osc Bank "+ofToString(waveScopes))->getName() != "X"){
                    parameterGroups[i]->get("Osc Bank "+ofToString(waveScopes)).cast<vector<float>>() = {0};
                    waveScopes++;
                }
                
            }
            
            //increase if it is not destroyed
            i++;
            
        }
        if(i == parameterGroups.size() && !newModulesCreated){
            for(pair<string, ofPoint> module : modulesToCreate){
                ofNotifyEvent(createNewModule, module, this);
            }
            newModulesCreated = true;
        }
    }
    
    
    if(xml.exists("CONNECTIONS")){
        xml.setTo("CONNECTIONS");
        int i = 0;
        while(xml.exists("CONNECTION[" + ofToString(i) + "]")){
            xml.setTo("CONNECTION[" + ofToString(i) + "]");
            vector<string> sourceInfo = ofSplitString(xml.getValue("source"), "-|-");
            vector<string> sinkInfo = ofSplitString(xml.getValue("sink"), "-|-");
            vector<string> minMaxInfo = ofSplitString(xml.getValue("minmax"), "-|-");
            if(minMaxInfo.size() != 2) minMaxInfo = {"0", "1"};
            ofStringReplace(sourceInfo[0], "_", " ");
            ofStringReplace(sinkInfo[0], "_", " ");
            ofStringReplace(sourceInfo[1], "_", " ");
            ofStringReplace(sinkInfo[1], "_", " ");
            for(int j = 0; j < parameterGroups.size(); j++){
                if(parameterGroups[j]->getName() == sourceInfo[0]){
                    connections.push_back(make_shared<nodeConnection>(datGuis[j]->getComponent(sourceInfo[1]), datGuis[j], &parameterGroups[j]->get(sourceInfo[1])));
                    ofAddListener(connections.back()->destroyEvent, this, &parametersControl::destroyedConnection);
                    break;
                }
            }
            for(int j = 0; j < parameterGroups.size(); j++){
                if(parameterGroups[j]->getName() == sinkInfo[0]){
                    connections.back()->connectTo(datGuis[j]->getComponent(sinkInfo[1]), datGuis[j], &parameterGroups[j]->get(sinkInfo[1]));
                    connections.back()->setMin(ofToFloat(minMaxInfo[0]));
                    connections.back()->setMax(ofToFloat(minMaxInfo[1]));
                    break;
                }
            }
            i++;
            xml.setToParent();
        }
    }
    
    
    for(auto paramGroup : parameterGroups){
        string moduleName = ofSplitString(paramGroup->getName(), " ")[0];
        if((moduleName == "oscillatorBank") ||( moduleName == "oscillator")){
            //If there is a rand wave that does not have a phasor atach we need to trigger the random creation, that means sending a value less that the value that actually is there, becouse we have previously assigned a value, we need to send a value less than the actual value for the oscillators to compute a new random.
            if(paramGroup->getGroup("Wave Select").getInt(1) == 6)
                paramGroup->getFloat("Phasor In") = paramGroup->getFloat("Phasor In") - 0.01;
            else
                paramGroup->getFloat("Phasor In") = paramGroup->getFloat("Phasor In");
        }
    }
    
    
    ofLog()<<"Load " << presetName;
    loadMidiMapping(presetName, bank);
    
    ofxOscMessage m;
    m.setAddress("presetLoad");
    m.addStringArg(presetName);
    m.addStringArg(bank);
    oscSender.sendMessage(m);
    
    for(auto groupParam : parameterGroups){
        if(ofStringTimesInString(groupParam->getName(), "phasor") != 0)
            groupParam->getBool("Reset_Phase") = 0;
    }
}

void parametersControl::loadPresetWithFade(string presetName, string bank){
    ofXml xml2;
    if(fadeTime > 0){
        if(xml2.load("Presets/" + bank + "/" + presetName + ".xml")){
            presetToLoad = presetName;
            bankToLoad = bank;
            ofxOscMessage m;
            m.setAddress("bankSelect");
            m.addStringArg(bank);
            oscSender.sendMessage(m);
            for(auto &s : senderGroups){
                Tweenzor::add((float*)&s->getFloat("Master Fader").get(), s->getFloat("Master Fader").get(), 0.0f, 0.0f, fadeTime);
            }
            if(senderGroups.size() > 0){
                Tweenzor::addCompleteListener(Tweenzor::getTween((float*)&senderGroups[0]->getFloat("Master Fader").get()), this, &parametersControl::loadPresetWhenFadeOutCompletes);
            }
            isFading = true;
        }
    }
    else{
        loadPreset(presetName, bank);
    }
}

void parametersControl::loadPresetWhenFadeOutCompletes(float *arg){
    if(*arg == 0){
        loadPreset(presetToLoad, bankToLoad);
        for(auto &s : senderGroups){
            Tweenzor::add((float*)&s->getFloat("Master Fader").get(), 0.0f, 1.0f, 0.0f, fadeTime);
        }
        if(senderGroups.size() > 0){
            Tweenzor::addCompleteListener(Tweenzor::getTween((float*)&senderGroups[0]->getFloat("Master Fader").get()), this, &parametersControl::loadPresetWhenFadeOutCompletes);
        }
    }
    else if(*arg == 1.0f){
        isFading = false;
    }
}

void parametersControl::onGuiButtonEvent(ofxDatGuiButtonEvent e){
    if(datGui->getButton(e.target->getName())){
        if(ofStringTimesInString(e.target->getName(), "Global")){
            string nameNoGlobal = e.target->getName();
            ofStringReplace(nameNoGlobal, "Global ", "");
            for(auto groupParam : parameterGroups){
                if(ofStringTimesInString(groupParam->getName(), "phasor") != 0)
                    groupParam->getBool(nameNoGlobal) = 0;
            }
        }else if(e.target->getName() == "Reload Sequence"){
            e.target->setBackgroundColor(loadPresetsSequence() ? ofColor(0,50,0) : ofColor(50,0,0));
        }
    }
}
void parametersControl::onGuiToggleEvent(ofxDatGuiToggleEvent e){
    for (int i=0; i < datGuis.size() ; i++){
        if(datGuis[i]->getToggle(e.target->getName()) == e.target)
            parameterGroups[i]->getBool(e.target->getName()) = e.target->getChecked();
    }
    if(e.target->getName() == "Automatic Preset"){
        autoPreset = e.checked;
        presetChangedTimeStamp = ofGetElapsedTimef();
        srand(time(0));
        random_shuffle(randomPresetsArrange.begin(), randomPresetsArrange.end());
    }else if(e.target->getName() == "BPM Sync"){
        if(e.checked)
            ofAddListener(beatTracker->bpmChanged, this, &parametersControl::bpmChangedListener);
        else
            ofRemoveListener(beatTracker->bpmChanged, this, &parametersControl::bpmChangedListener);
        
    }
    else if(e.target->getName() == "Midi Learn"){
        midiListenActive = e.checked;
    }
}

void parametersControl::onGuiDropdownEvent(ofxDatGuiDropdownEvent e){
    if(e.target == bankSelect){
        oldPresetButton = nullptr;
        if(e.child == bankSelect->getNumOptions()-1){
            bankSelect->addOption("Bank " + ofGetTimestampString(), bankSelect->getNumOptions()-1);
            bankSelect->select(bankSelect->getNumOptions()-2);
        }
        loadBank();
    }
    else{
        for (int i=0; i < datGuis.size() ; i++){
            if(datGuis[i]->getDropdown(e.target->getName()) == e.target){
                parameterGroups[i]->getGroup(e.target->getName()).getInt(1) = e.child;
                //            if(datGuis[i]->getHeight() > ofGetHeight())
                //                ofSetWindowShape(ofGetWidth(), datGuis[i]->getHeight());
            }
        }
    }
}


void parametersControl::onGuiSliderEvent(ofxDatGuiSliderEvent e){
    if(datGui->getSlider(e.target->getName())->getName() == e.target->getName()){
        string nameNoGlobal = e.target->getName();
        ofStringReplace(nameNoGlobal, "Global ", "");
        for(auto groupParam : parameterGroups){
            if(ofStringTimesInString(groupParam->getName(), "phasor") != 0 && groupParam->getFloat(nameNoGlobal).getName() == nameNoGlobal)
                groupParam->getFloat(nameNoGlobal) = e.value;
        }
    }
    if(e.target->getName() == "Beats Period")
        periodTime = e.value / datGui->getSlider("Global BPM")->getValue() * 60.;
}

void parametersControl::onGuiTextInputEvent(ofxDatGuiTextInputEvent e){
    for (int i=0; i < datGuis.size() ; i++){
        if(datGuis[i]->getTextInput(e.target->getName()) == e.target)
            parameterGroups[i]->getString(e.target->getName()) = e.text;
    }
}

void parametersControl::onGuiColorPickerEvent(ofxDatGuiColorPickerEvent e){
    for (int i=0; i < datGuis.size() ; i++){
        if(datGuis[i]->getColorPicker(e.target->getName()) == e.target)
            parameterGroups[i]->getColor(e.target->getName()) = e.color;
    }
}

void parametersControl::onGuiRightClickEvent(ofxDatGuiRightClickEvent e){
    if(e.down == 1){
        for (int i=0; i < datGuis.size() ; i++){
            if(datGuis[i]->getComponent(e.target->getType(), e.target->getName()) == e.target){
                ofAbstractParameter &parameter = parameterGroups[i]->get(e.target->getName());
                if(midiListenActive){
                    if(parameter.type() == typeid(ofParameter<float>).name()){
                        bool erasedConnection = false;
                        if(ofGetKeyPressed(OF_KEY_SHIFT)){
                            for(int j = 0 ; j < midiFloatConnections.size(); j++){
                                if(midiFloatConnections[i].getParameter() == &parameter){
                                    erasedConnection = true;
                                    midiFloatConnections.erase(midiFloatConnections.begin()+j);
                                    return;
                                }
                            }
                        }
                        if(!erasedConnection)
                            midiFloatConnections.push_back(midiConnection<float>(&parameter.cast<float>()));
                    }
                    else if(parameter.type() == typeid(ofParameter<int>).name()){
                        bool erasedConnection = false;
                        if(ofGetKeyPressed(OF_KEY_SHIFT)){
                            for(int j = 0 ; j < midiIntConnections.size(
                                ); j++){
                                if(midiIntConnections[j].getParameter() == &parameter){
                                    erasedConnection = true;
                                    midiIntConnections.erase(midiIntConnections.begin()+j);
                                    return;
                                }
                            }
                        }
                        if(!erasedConnection)
                            midiIntConnections.push_back(midiConnection<int>(&parameter.cast<int>()));
                    }
                    else if(parameter.type() == typeid(ofParameter<bool>).name()){
                        bool erasedConnection = false;
                        if(ofGetKeyPressed(OF_KEY_SHIFT)){
                            for(int j = 0 ; j < midiBoolConnections.size(); j++){
                                if(midiBoolConnections[j].getParameter() == &parameter){
                                    erasedConnection = true;
                                    midiBoolConnections.erase(midiBoolConnections.begin()+j);
                                    return;
                                }
                            }
                        }
                        if(!erasedConnection)
                            midiBoolConnections.push_back(midiConnection<bool>(&parameter.cast<bool>()));
                    }
                    else if(parameter.type() == typeid(ofParameterGroup).name()){
                        ofParameter<int> &intParameter = parameterGroups[i]->getGroup(e.target->getName()).getInt(1);
                        bool erasedConnection = false;
                        if(ofGetKeyPressed(OF_KEY_SHIFT)){
                            for(int i = 0 ; i < midiIntConnections.size(); i++){
                                if(midiIntConnections[i].getParameter() == &intParameter){
                                    erasedConnection = true;
                                    midiIntConnections.erase(midiIntConnections.begin()+i);
                                    return;
                                }
                            }
                        }
                        if(!erasedConnection)
                            midiIntConnections.push_back(midiConnection<int>(&intParameter));
                    }
                    else
                        ofLog() << "Cannot midi to parameter " << parameter.getName();
                }
                else{
                    bool foundParameter = false;
                    for(int j = 0 ; j < connections.size() ; j++){
                        if(connections[j]->getSinkParameter() == &parameter){
                            swap(connections[j], connections.back());
                            connections.back()->disconnect();
                            foundParameter = true;
                            break;
                        }
                    }
                    if(!foundParameter){
                        connections.push_back(make_shared<nodeConnection>(e.target, datGuis[i], &parameter));
                        ofAddListener(connections.back()->destroyEvent, this, &parametersControl::destroyedConnection);
                    }
                }
            }
        }
    }else if(connections.size() > 0){
        for (int i=0; i < datGuis.size() ; i++){
            if(datGuis[i]->getComponent(e.target->getType(), e.target->getName()) == e.target
               && !connections.back()->closedLine
               && connections.back()->getSourceParameter() != &parameterGroups[i]->get(e.target->getName())){
                connections.back()->connectTo(e.target, datGuis[i], &parameterGroups[i]->get(e.target->getName()));
                for(int i = 0; i<connections.size()-1 ; i++){
                    if(connections.back()->getSinkParameter() == connections[i]->getSourceParameter() ||
                       connections.back()->getSinkParameter() == connections[i]->getSinkParameter() ||
                       connections.back()->getSourceParameter() == connections[i]->getSinkParameter())
                        connections.erase(connections.begin()+i);
                }
            }
        }
    }
}

void parametersControl::onGuiScrollViewEvent(ofxDatGuiScrollViewEvent e){
    if(ofGetKeyPressed(OF_KEY_SHIFT)){
        changePresetLabelHighliht(e.target);
        savePreset(e.target->getName(), bankSelect->getSelected()->getName());
    }else{
        changePresetLabelHighliht(e.target);
        loadPresetWithFade(e.target->getName(), bankSelect->getSelected()->getName());
        if(autoPreset)
            presetChangedTimeStamp = ofGetElapsedTimef();
    }
}

void parametersControl::onGuiParagraphEvent(ofxDatGuiParagraphEvent e){
    for (int i=0; i < datGuis.size() ; i++){
        if(datGuis[i]->getParagraph(e.target->getName()) == e.target)
            parameterGroups[i]->getString(e.target->getName() + "_paragraph") = e.text;
    }
}

void parametersControl::newModuleListener(ofxDatGuiDropdownEvent e){
    vector<string> moduleNames = {"phasor", "oscillator", "oscillatorBank", "oscillatorGroup", "envelopeGenerator", "midiGateIn", "delta", "expressionOperator", "mapper", "vectorMapper", "manualOscillatorBank", "typeConverter", "vectorGetter", "vectorChain", "valueSmoother", "vectorValueSmoother", "vectorOperator", "subDimensionCombinator", "bankDimensionCombinator", "bidimensionalOscillatorBank"};
    pair<string, ofPoint> pairToSend;
    pairToSend.first = moduleNames[e.child];
    ofVec4f transformedPos = popUpMenu->getPosition();
    transformedPos -= transformMatrix.getTranslation();
    transformedPos = transformMatrix.getInverse().postMult(transformedPos);
    pairToSend.second = transformedPos;
    if(pairToSend.first == "waveScope"){
        for(int i = 0; i < datGuis.size(); i++)
            if(datGuis[i]->getHeader()->getName() == "waveScope") destroyModuleAndConnections(i);
    }
    ofNotifyEvent(createNewModule, pairToSend, this);
    
    popUpMenu->setVisible(false);
    popUpMenu->setPosition(-1, -1);
    ofxDatGuiDropdown* drop = popUpMenu->getDropdown("Choose module");
    drop->setLabel("Choose module");
}

void parametersControl::newPresetListener(ofxDatGuiTextInputEvent e){
    if(e.text != ""){
        string newPresetName;
        if(presetsList->getNumItems() != 0){
            string lastPreset = presetsList->get(presetsList->getNumItems()-1)->getName();
            newPresetName = ofToString(ofToInt(ofSplitString(lastPreset, "|")[0])+1) + "|" + e.text;
        }else
            newPresetName = "1|" + e.text;
        
        presetsList->add(newPresetName);
        changePresetLabelHighliht(presetsList->get(presetsList->getNumItems()-1));
        savePreset(newPresetName, bankSelect->getSelected()->getName());
        
        e.text = "";
    }
}



#pragma mark --Keyboard and mouse Listenerrs--

void parametersControl::keyPressed(ofKeyEventArgs &e){
    if(ofGetKeyPressed(OF_KEY_CONTROL) && e.key == OF_KEY_LEFT_SHIFT){
        for (int i=0; i < datGuis.size() ; i++){
            if(datGuis[i]->getHeader()->getName() == "waveScope"){
                ofVec4f transformedPos(ofGetMouseX() + 20, ofGetMouseY() - (transformMatrix.getScale()[1]*datGuis[i]->getHeight()/2), 0, 0);
                transformedPos -= transformMatrix.getTranslation();
                transformedPos = transformMatrix.getInverse().postMult(transformedPos);
                datGuis[i]->setPosition(transformedPos.x, transformedPos.y);
            }
        }
    }
    if(e.key == OF_KEY_COMMAND){
        commandPressed = true;
        glfwSetCursor((GLFWwindow*)ofGetWindowPtr()->getWindowContext(), arrowListCursor);
    }
    else if(e.key == OF_KEY_ESC){
        popUpMenu->setVisible(false);
        for(auto connection : connections){
            connection->toggleGui(false);
        }
    }
    else if(e.key == ' '  && !canvasDragging){
        glfwSetCursor((GLFWwindow*)ofGetWindowPtr()->getWindowContext(), openedHandCursor);
        if(ofGetMousePressed()) dragCanvasInitialPoint = ofPoint(ofGetMouseX(), ofGetMouseY());
        canvasDragging = true;
    }
        
}

void parametersControl::keyReleased(ofKeyEventArgs &e){
    if(e.key == OF_KEY_COMMAND){
        commandPressed = false;
        glfwSetCursor((GLFWwindow*)ofGetWindowPtr()->getWindowContext(), arrowCursor);
    }
    else if(e.key == ' '){
        glfwSetCursor((GLFWwindow*)ofGetWindowPtr()->getWindowContext(), arrowCursor);
        canvasDragging = false;
    }
}

void parametersControl::mouseMoved(ofMouseEventArgs &e){
    
}

void parametersControl::mouseDragged(ofMouseEventArgs &e){
    ofVec4f transformedPos = e;
    transformedPos -= transformMatrix.getTranslation();
    transformedPos = transformMatrix.getInverse().postMult(transformedPos);
    if(e.button == 2 && connections.size() > 0){
        if(!connections.back()->closedLine){
            connections.back()->moveLine(transformedPos);
        }
    }if(canvasDragging){
        transformMatrix.translate(e - dragCanvasInitialPoint);
        for(auto &gui : datGuis)
            gui->setTransformMatrix(transformMatrix);//gui->setTransformMatrix(ofMatrix4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1));
        dragCanvasInitialPoint = e;
    }
}

void parametersControl::mousePressed(ofMouseEventArgs &e){
    ofVec4f transformedPos = e;
    transformedPos -= transformMatrix.getTranslation();
    transformedPos = transformMatrix.getInverse().postMult(transformedPos);
    if(commandPressed){
       if(e.button == 0){
        popUpMenu->setPosition(e.x, e.y);
        popUpMenu->setVisible(true);
        popUpMenu->getDropdown("Choose module")->expand();
       }
       else if(e.button == 1){
           transformMatrix.translate(-transformedPos * (1-transformMatrix.getScale()));
           transformMatrix = ofMatrix4x4::newTranslationMatrix(transformMatrix.getTranslation());
           for(auto &gui : datGuis)
               gui->setTransformMatrix(transformMatrix);//gui->setTransformMatrix(ofMatrix4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1));
       }
    }if(canvasDragging){
        glfwSetCursor((GLFWwindow*)ofGetWindowPtr()->getWindowContext(), closedHandCursor);
        dragCanvasInitialPoint = e;
    }if(ofGetKeyPressed(OF_KEY_CONTROL)){
        bool cablePressed = false;
        for(auto connection : connections){
            if(connection->hitTest(transformedPos) && !cablePressed){
                connection->toggleGui(true, e);
                cablePressed = true;
            }
            else if(connection->closedLine){
                connection->toggleGui(false);
            }
        }
    }if(ofGetKeyPressed('r')){
        for(int i = 0; i<datGuis.size(); i++){
            string moduleName = ofSplitString(parameterGroups[i]->getName(), " ")[0];
            if(datGuis[i]->hitTest(e)
               && moduleName != "senderManager" && moduleName != "waveScope" && moduleName != "colorApplier" && moduleName != "audioControls" && moduleName != "chartresTextureUnifier" && moduleName != "oscillatorGroup" && moduleName != "speakerPowerCalculator" && moduleName != "dataRecorder" && moduleName != "textureUnifier"){
                destroyModuleAndConnections(i);
            }
        }
    }
}

void parametersControl::mouseReleased(ofMouseEventArgs &e){
    if(e.button == 2 && connections.size() > 0){
        if(!connections.back()->closedLine)
            connections.pop_back();
    }
    if(canvasDragging){
        glfwSetCursor((GLFWwindow*)ofGetWindowPtr()->getWindowContext(), openedHandCursor);
    }
}

void parametersControl::mouseScrolled(ofMouseEventArgs &e){
    ofVec4f transformedPos = e;
    transformedPos -= transformMatrix.getTranslation();
    transformedPos = transformMatrix.getInverse().postMult(transformedPos);
    if(commandPressed){
        transformMatrix.translate(transformedPos * transformMatrix.getScale()*e.scrollY/100);
        transformMatrix.glScale(ofVec3f(1-(e.scrollY/100), 1-(e.scrollY/100), 1));
        for(auto &gui : datGuis)
            gui->setTransformMatrix(transformMatrix);//gui->setTransformMatrix(ofMatrix4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1));
        if(e.scrollY < 0)
            glfwSetCursor((GLFWwindow*)ofGetWindowPtr()->getWindowContext(), zoomInCursor);
        else
            glfwSetCursor((GLFWwindow*)ofGetWindowPtr()->getWindowContext(), zoomOutCursor);
    }
}

void parametersControl::mouseEntered(ofMouseEventArgs &e){
    
}

void parametersControl::mouseExited(ofMouseEventArgs &e){
    
}


#pragma mark --parameter listenerrs--
void parametersControl::bpmChangedListener(float &bpm){
    newBpm = bpm;
}

void parametersControl::listenerFunction(ofAbstractParameter& e){
    int position = 0;
    vector<shared_ptr<nodeConnection>> validConnections;
    
    if(e.getName() == "Phasor Monitor"){
        //return;
    }
    
    auto parent = e.getGroupHierarchyNames()[0];
    int parentIndex = 0;
    ofStringReplace(parent, "_", " ");
    for(int i = 0; i<parameterGroups.size(); i++){
        if(parameterGroups[i]->getName() == parent){
            parentIndex = i;
            break;
        }
    }
    
    //ParameterBinding
    for(auto &connection : connections){
        if(connection != nullptr){
            ofAbstractParameter* possibleSource = connection->getSourceParameter();
            if(possibleSource == &e && connection->closedLine){
                validConnections.push_back(connection);
            }
        }
    }
    
    //Midi and to gui
    float normalizedVal = 0;
    if(e.type() == typeid(ofParameter<float>).name()){
        ofParameter<float> castedParam = e.cast<float>();
        normalizedVal = ofMap(castedParam, castedParam.getMin(), castedParam.getMax(), 0, 1);
        if(castedParam.getName() == "BPM")
            periodTime = presetChangeBeatsPeriod / castedParam * 60.;

        for(auto &midiConnection : midiFloatConnections){
            ofAbstractParameter* possibleSource = midiConnection.getParameter();
            if(possibleSource == &e && !midiConnection.isListening()){
                for(auto &mOut : midiOut){
                    if(mOut.getName() == midiConnection.getDevice())
                        mOut.sendControlChange(midiConnection.getChannel(), midiConnection.getControl(), midiConnection.sendValue());
                }
            }
        }
        
        for(auto validConnection : validConnections)
            setFromNormalizedValue(validConnection->getSinkParameter(), ofMap(normalizedVal, 0, 1, validConnection->getMin(), validConnection->getMax()));
        
    }
    else if(e.type() == typeid(ofParameter<int>).name()){
        ofParameter<int> castedParam = e.cast<int>();
        normalizedVal = ofMap(castedParam, castedParam.getMin(), castedParam.getMax(), 0, 1);
        
        for(auto &midiConnection : midiIntConnections){
            ofAbstractParameter* possibleSource = midiConnection.getParameter();
            if(possibleSource == &e && !midiConnection.isListening()){
                for(auto &mOut : midiOut){
                    if(mOut.getName() == midiConnection.getDevice())
                        mOut.sendControlChange(midiConnection.getChannel(), midiConnection.getControl(), midiConnection.sendValue());
                }
            }
        }
        
        if(ofStringTimesInString(castedParam.getName(), "Select") == 1){
            datGuis[parentIndex]->getDropdown(castedParam.getName())->select(castedParam);
        }
        
        for(auto validConnection : validConnections)
            setFromNormalizedValue(validConnection->getSinkParameter(), ofMap(normalizedVal, 0, 1, validConnection->getMin(), validConnection->getMax()));
    }
    else if(e.type() == typeid(ofParameter<bool>).name()){
        ofParameter<bool> castedParam = e.cast<bool>();
        normalizedVal = castedParam ? 1 : 0;
        
        for(auto &midiConnection : midiBoolConnections){
            ofAbstractParameter* possibleSource = midiConnection.getParameter();
            if(possibleSource == &e && !midiConnection.isListening()){
                for(auto &mOut : midiOut){
                    if(mOut.getName() == midiConnection.getDevice())
                        mOut.sendControlChange(midiConnection.getChannel(), midiConnection.getControl(), midiConnection.sendValue());
                }
            }
        }
        
        //Update to datGuis
        datGuis[parentIndex]->getToggle(castedParam.getName())->setChecked(normalizedVal);
        
        for(auto validConnection : validConnections)
            setFromNormalizedValue(validConnection->getSinkParameter(), ofMap(normalizedVal, 0, 1, validConnection->getMin(), validConnection->getMax()));
    }
    else if(e.type() == typeid(ofParameter<string>).name()){
        ofParameter<string> castedParam = e.cast<string>();
        
        if(ofSplitString(castedParam.getName(), "_").size() > 1){
            if(ofSplitString(castedParam.getName(), "_")[1] == "paragraph"){
                datGuis[parentIndex]->getParagraph(ofSplitString(castedParam.getName(), "_")[0])->setTextWithoutEvent(castedParam);
            }
        }
        else{
            datGuis[parentIndex]->getTextInput(castedParam.getName())->setTextWithoutEvent(castedParam);
        }
        
        for(auto validConnection : validConnections)
            setFromNormalizedValue(validConnection->getSinkParameter(), ofMap(normalizedVal, 0, 1, validConnection->getMin(), validConnection->getMax()));
    }
    else if(e.type() == typeid(ofParameter<ofColor>).name()){
        ofParameter<ofColor> castedParam = e.cast<ofColor>();
        
        datGuis[parentIndex]->getColorPicker(castedParam.getName())->setColor(castedParam);
    }else if(e.type() == typeid(ofParameterGroup).name()){
        ofParameter<int> castedParam = parameterGroups[parentIndex]->getGroup(e.getName()).getInt(1);
        datGuis[parentIndex]->getDropdown(e.getName())->select(castedParam);
        
        normalizedVal = ofMap(castedParam, castedParam.getMin(), castedParam.getMax(), 0, 1);
        
        for(auto validConnection : validConnections)
            setFromNormalizedValue(validConnection->getSinkParameter(), ofMap(normalizedVal, 0, 1, validConnection->getMin(), validConnection->getMax()));
    }else{
        for(auto &validConnection : validConnections)
            setFromSameTypeValue(validConnection);
    }
}

void parametersControl::newMidiMessage(ofxMidiMessage &eventArgs){
    //Save all midi messages into a que;
    midiMessages.push_front(eventArgs);
}

void parametersControl::setFromNormalizedValue(ofAbstractParameter* e, float v){
    if(e->type() == typeid(ofParameter<float>).name()){
        ofParameter<float> castedParam = e->cast<float>();
        castedParam.set(ofMap(v, 0, 1, castedParam.getMin(), castedParam.getMax()));
    }
    else if(e->type() == typeid(ofParameter<int>).name()){
        ofParameter<int> castedParam = e->cast<int>();
        castedParam.set(ofMap(v, 0, 1, castedParam.getMin(), castedParam.getMax()));
    }
    else if(e->type() == typeid(ofParameter<bool>).name()){
        ofParameter<bool> castedParam = e->cast<bool>();
        castedParam = v >= 0.5 ? true : false;
    }
    else if(e->type() == typeid(ofParameter<ofColor>).name()){
        ofParameter<ofColor> castedParam = e->cast<ofColor>();
        castedParam.set(ofColor::fromHsb(v*255, 255, 255));
    }
    else if(e->type() == typeid(ofParameterGroup).name()){
        ofParameterGroup adoptiveGroup;
        adoptiveGroup.add(*e);
        ofParameterGroup nestedGroup = adoptiveGroup.getGroup(0);
        ofParameter<int> castedParam = nestedGroup.getInt(1);
        castedParam.set(ofMap(v, 0, 1, castedParam.getMin(), castedParam.getMax()));
    }
}

void parametersControl::setFromSameTypeValue(shared_ptr<nodeConnection> connection){
    ofAbstractParameter* source = connection->getSourceParameter();
    ofAbstractParameter* sink = connection->getSinkParameter();
    if(source != nullptr && sink != nullptr){
        if(source->type() == sink->type()){
            if(source->type() == typeid(ofParameter<vector<float>>).name()){
                vector<float> tempVec = source->cast<vector<float>>();
                for (auto &pos : tempVec)
                    pos = ofMap(pos, 0, 1, connection->getMin(), connection->getMax());
                sink->cast<vector<float>>() = tempVec;
            }
            else if(source->type() == typeid(ofParameter<vector<vector<float>>>).name()){
                vector<vector<float>> tempVec = source->cast<vector<vector<float>>>();
                for (auto &pos : tempVec){
                    for(auto &pos2 : pos)
                        pos2 = ofMap(pos2, 0, 1, connection->getMin(), connection->getMax());
                }
                sink->cast<vector<vector<float>>>() = tempVec;
            }
            else if(source->type() == typeid(ofParameter<vector<int>>).name()){
                vector<int> tempVec = source->cast<vector<int>>();
                for (auto &pos : tempVec)
                    pos = ofMap(pos, 0, 1, connection->getMin(), connection->getMax());
                sink->cast<vector<int>>() = tempVec;
            }
            else if(source->type() == typeid(ofParameter<vector<vector<int>>>).name()){
                vector<vector<int>> tempVec = source->cast<vector<vector<int>>>();
                for (auto &pos : tempVec){
                    for(auto &pos2 : pos)
                        pos2 = ofMap(pos2, 0, 1, connection->getMin(), connection->getMax());
                }
                sink->cast<vector<vector<int>>>() = tempVec;
            }
            else if(source->type() == typeid(ofParameter<vector<vector<ofColor>>>).name()){
                vector<vector<ofColor>> tempVec = source->cast<vector<vector<ofColor>>>();
                for (auto &pos : tempVec){
                    for(auto &pos2 : pos)
                        pos2.setHue(ofMap(pos2.getHue(), 0, 1, connection->getMin(), connection->getMax()));
                }
                sink->cast<vector<vector<ofColor>>>() = tempVec;
            }
        }else{
            for(auto &connection : connections){
                if(connection->getSourceParameter() == source && connection->getSinkParameter() == sink){
                    connections.erase(remove(connections.begin(), connections.end(), connection));
                    break;
                }
            }
        }
    }
    
}

void parametersControl::destroyModuleAndConnections(int index){
    for(int j = 0; j < connections.size();){
        auto &connection = connections[j];
        if(connection->getParentGuis(0) == datGuis[index] || connection->getParentGuis(1) == datGuis[index])
            connections.erase(remove(connections.begin(), connections.end(), connection));
        else
            j++;
    }
    datGuis.erase(datGuis.begin()+index);
    string moduleName = parameterGroups[index]->getName();
    ofRemoveListener(parameterGroups[index]->parameterChangedE(), this, &parametersControl::listenerFunction);
    ofNotifyEvent(destroyModule, moduleName, this);
    parameterGroups.erase(parameterGroups.begin()+index);
}

void parametersControl::destroyedConnection(ofAbstractParameter &disconnectedParameter){
    for(auto paramGroup : parameterGroups){
        if(paramGroup->contains(disconnectedParameter.getName())){
            if(&paramGroup->get(disconnectedParameter.getName()) == &disconnectedParameter){
                if(ofStringTimesInString(disconnectedParameter.getName(), "Vector")){
                    string paramNameWithoutVector = disconnectedParameter.getName();
                    paramNameWithoutVector.erase(paramNameWithoutVector.end() - 7, paramNameWithoutVector.end());
                    if(paramGroup->get(paramNameWithoutVector).type() == typeid(ofParameter<float>).name())
                        paramGroup->getFloat(paramNameWithoutVector) = paramGroup->getFloat(paramNameWithoutVector);
                    else if(paramGroup->get(paramNameWithoutVector).type() == typeid(ofParameter<int>).name())
                        paramGroup->getInt(paramNameWithoutVector) = paramGroup->getInt(paramNameWithoutVector);
                    else if(paramGroup->get(paramNameWithoutVector).type() == typeid(ofParameter<bool>).name())
                        paramGroup->getBool(paramNameWithoutVector) = paramGroup->getBool(paramNameWithoutVector);
                    else if(paramGroup->get(paramNameWithoutVector).type() == typeid(ofParameterGroup).name())
                        paramGroup->getGroup(paramGroup->get(paramNameWithoutVector).getName()).getInt(1) = paramGroup->getGroup(paramGroup->get(paramNameWithoutVector).getName()).getInt(1);
                }
                break;
            }
        }
    }
}

float parametersControl::getGlobalBPM()
{
    return datGui->getSlider("Global BPM")->getValue();
}
