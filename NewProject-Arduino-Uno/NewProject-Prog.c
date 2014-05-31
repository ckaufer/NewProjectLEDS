#include "NewProject.h"
#include "Hal.h"
#include "arduinoCode/arduinoCode.h"
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#define FRAMEADDRESS 0
#define offsetof_elem(type, memb, index) \ 
                 ((size_t)(offsetof(type, memb[0]) + \ 
                           (size_t)(index) * sizeof(((type *)0)->memb[0]) ))
#define FRAME_OFFSET(index) offsetof_elem(Database, frameTable, index)
#define SCENE_OFFSET() offsetof_elem(Database, sceneTable, currentSceneId)
#define LEDSTATE(mask, n) (mask & (1 << n) ? NewProject_ON : NewProject_OFF)
//declare helper functions above
//identities, | and &
//cleanup the code, test everything, readup. How to compute offset of an element of a structure of arrays?
//top of a file, only decleration (header file)
//constants, types, functions get declared -> first (with semicolon). global variables -> 2nd, functions -> 3rd alphabetise
typedef uint16_t FrameMask;
typedef struct  {
    NewProject_Scene sceneTable[NewProject_SceneId_max];
    uint16_t frameTable[NewProject_FrameId_max];
} Database;

Database d;
FrameMask frameList[NewProject_FrameId_max];
NewProject_Scene sceneList[NewProject_SceneId_max];
NewProject_currentScene_t currentScene;
NewProject_currentFrameId_t currentFrameId = 1;
NewProject_currentSceneId_t currentSceneId = 1;
NewProject_currentMode_t currentMode;
NewProject_delay_t delayVal = 0.8 * NewProject_delay_scale;
NewProject_delay_t currentTime = 0;
NewProject_SaveToEEProm saveState = 0;
uint16_t currentFrameIdx = 1;

static void tickHandler(void);
static void decodeFrame(FrameMask, NewProject_Frame*);
FrameMask encodeFrame(NewProject_currentFrame_t*);
static void updateLed(uint8_t, FrameMask);
static void updateFrameLeds(void);
static void updateSceneFrames(void);


void main() {
    Hal_init();
    //loadFramesFromEEProm();
    Hal_tickStart(NewProject_delay_step, tickHandler);
    NewProject_start();
    Hal_idleLoop();
}

FrameMask encodeFrame(NewProject_currentFrame_t* input) {
    FrameMask result = 0;
    for(uint16_t i = 0; i <= NewProject_currentFrameId_max; i++) {
       result |= (input->led[i + 1]) << i + 1; 
    }
    return result;
}

void decodeFrame(FrameMask mask, NewProject_Frame* output) {
    for(uint16_t i = 0; i <= NewProject_currentFrameId_max; i++) {
        output->led[i + 1] = LEDSTATE(mask, i + 1);
    }
}

void updateLed(uint8_t id, FrameMask ledState) {
    if (ledState) {
        Hal_User_ledOn(id);
    }
    else {
        Hal_User_ledOff(id);
    }
}

void updateFrameLeds() {
    //NewProject_currentFrame_t* framePtr = &frameList[currentFrameId];
    FrameMask mask = frameList[currentFrameId];
    for(uint16_t i = 0; i <= NewProject_currentFrameId_max; i++) {
        updateLed(i + 1, LEDSTATE(mask, i + 1));
    }
}

void updateSceneFrames() {
    currentFrameId = sceneList[currentSceneId].frameList[sceneList[currentSceneId].seqLength];
    //frameList[currentFrameId]->frameId = currentFrameId;
    updateFrameLeds();
}

void tickHandler() {
    
    if (currentTime < delayVal) {
        currentTime += NewProject_delay_step;
        return;
    }
    
    if (currentMode == NewProject_PLAY) { 
        if (currentFrameIdx < sceneList[currentSceneId].maxRange) {
            sceneList[currentSceneId].seqLength = currentFrameIdx++;
        }
        else {
            currentFrameIdx = 0;
        }
        updateSceneFrames();
    }
    currentTime = 0;
}

void loadFrameFromEEProm(FrameMask frameNum) {
    FrameMask* temp = 0;
    Hal_User_eepromRead(FRAME_OFFSET(frameNum), &frameList[frameNum], sizeof(FrameMask));
    updateFrameLeds();
}

void saveFrameToEEProm(FrameMask frameNum) {
    //Hal_User_eepromWrite(500, &frameList[frameNum], sizeof(FrameMask));
    FrameMask* t;
    Hal_User_eepromWrite(FRAME_OFFSET(frameNum), &frameList[frameNum], sizeof(FrameMask));
}

void saveFramesToEEProm() {
    for(uint16_t i = 0; i < NewProject_FrameId_max; i++) {
        saveFrameToEEProm(i);
    }
}

void loadFramesFromEEProm() {
    for(uint16_t i = 0; i < NewProject_FrameId_max; i++) {
        loadFrameFromEEProm(i);
    }
}

void loadFrameFromDatabase(FrameMask frameNum) {
    frameList[frameNum] = d.frameTable[frameNum];
}

void saveFrameToDatabase(FrameMask frameNum) {
    d.frameTable[frameNum] = frameList[frameNum];
}

void emptyDatabaseFrames() {
    for(uint16_t i = 0; i < NewProject_FrameId_max; i++) {
        d.frameTable[i] = NULL;
    }
}

void NewProject_connectHandler(void) {
    Hal_connected();
}

void NewProject_disconnectHandler(void) {
    Hal_disconnected();
}

void NewProject_currentFrameId_store(NewProject_currentFrameId_t* input) {
    if (currentMode == NewProject_EDIT) {
        currentFrameId = *input;
        updateFrameLeds();
    }
}

void NewProject_currentFrame_fetch(NewProject_currentFrame_t* output) {
    loadFrameFromDatabase(currentFrameId);
    decodeFrame(frameList[currentFrameId], output);
    output->frameId = currentFrameId;
}

void NewProject_currentFrame_store(NewProject_currentFrame_t* input) {
    if (currentMode == NewProject_EDIT) {
        frameList[currentFrameId] = encodeFrame(input);
        saveFrameToDatabase(currentFrameId);
        updateFrameLeds();
    }
}

void NewProject_currentSceneId_store(NewProject_currentSceneId_t* input) {
    if (currentMode == NewProject_EDIT) {
        currentSceneId = *input;
        updateSceneFrames();
    }
}

void NewProject_currentScene_fetch(NewProject_currentScene_t* output) {
    *output = sceneList[currentSceneId];
    output->sceneId = currentSceneId;
}

void NewProject_currentScene_store(NewProject_currentScene_t* input) {
    if (currentMode == NewProject_EDIT) {
        sceneList[currentSceneId] = *input;
        updateSceneFrames();
    }
}

void NewProject_currentMode_fetch(NewProject_currentMode_t* output) {
    *output = currentMode;
}

void NewProject_currentMode_store(NewProject_currentMode_t* input) {
    currentMode = *input;
}

void NewProject_delay_fetch(NewProject_delay_t* output) {
    *output = delayVal;
}

void NewProject_delay_store(NewProject_delay_t* input) {
    delayVal = *input;
}

void NewProject_frameState_fetch(NewProject_frameState_t* output) {
    /* TODO: read resource 'frameState' into 'output' */
}

void NewProject_saveToEEProm_fetch(NewProject_saveToEEProm_t* output) {
   
   *output = saveState;
}

void NewProject_saveToEEProm_store(NewProject_saveToEEProm_t* input) {
    /* TODO: write resource 'saveToEEProm' from 'input' */
    saveState = *input;
    if(saveState == NewProject_SAVE) {
        saveFramesToEEProm();
    } else {
        loadFramesFromEEProm();//loadFramesFromEEProm();
    }
    
}

void NewProject_printout_fetch(NewProject_printout_t* output) {
    *output = frameList[currentFrameId];
}

void NewProject_printout_store(NewProject_printout_t* input) {
    //updateFrameLeds();
}
