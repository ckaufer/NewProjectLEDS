#include "NewProject.h"
#include "Hal.h"
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#define LEDSTATE(mask, n) (mask & (1 << n) ? NewProject_ON : NewProject_OFF)
//declare helper functions above
//identities, | and &
//cleanup the code, test everything, readup. How to compute offset of an element of a structure of arrays?
//top of a file, only decleration (header file)
//constants, types, functions get declared -> first (with semicolon). global variables -> 2nd, functions -> 3rd alphabetise
typedef uint16_t SceneMask;

SceneMask sceneList[NewProject_SceneId_max];
NewProject_Sequence sequenceList[NewProject_SequenceId_max];
NewProject_currentSequence_t currentSequence;
NewProject_currentSceneId_t curSceneId;
NewProject_currentSequenceId_t curSequenceId;
NewProject_currentMode_t curMode;
NewProject_delay_t delayVal = 0.8 * NewProject_delay_scale;
NewProject_delay_t curTime = 0;
uint16_t curSceneIdx = 0;
static void tickHandler(void);
static void decodeScene(SceneMask, NewProject_Scene*);
SceneMask encodeScene(NewProject_currentScene_t*);
static void updateLed(uint8_t, SceneMask);
static void updateSceneLeds(void);
static void updateSequenceScenes(void);

void main() {
    Hal_init();
    Hal_tickStart(NewProject_delay_step, tickHandler);
    NewProject_start();    
    Hal_idleLoop();
}

SceneMask encodeScene(NewProject_currentScene_t* input) {
    SceneMask result = 0;
    for(uint16_t i = 0; i < 14; i++) {
       result |= (input->led[i + 2]) << i + 2; 
    }
    return result;
}

void decodeScene(SceneMask mask, NewProject_Scene* output) {
    for(uint16_t i = 0; i < 14; i++) {
        output->led[i + 2] = LEDSTATE(mask, i + 2);
    }
}

void updateLed(uint8_t id, SceneMask ledState) {
    if (ledState) {
        Hal_User_ledOn(id);
    }
    else {
        Hal_User_ledOff(id);
    }
}



void updateSceneLeds() {
    //NewProject_currentScene_t* scenePtr = &sceneList[curSceneId];
    SceneMask mask = sceneList[curSceneId];
    for(uint16_t i = 0; i < 14; i++) {
        updateLed(i + 2, LEDSTATE(mask, i + 2));
    }
}

void updateSequenceScenes() {
    curSceneId = sequenceList[curSequenceId].sceneList[sequenceList[curSequenceId].seqLength];
    //sceneList[curSceneId]->sceneId = curSceneId;
    updateSceneLeds();
}

void tickHandler() {
    
    /*if (curTime < delayVal) {
        curTime += NewProject_delay_step;
        return;
    }
    
    if (curMode == NewProject_PLAY) { 
        if (curSceneIdx <= sequenceList[curSequenceId].maxRange - 1) {
            sequenceList[curSequenceId].seqLength = curSceneIdx++;
            
        }
        else {
            curSceneIdx = sequenceList[curSequenceId].minRange;
        }
        updateSequenceScenes();
    }
    curTime = 0;*/
}

void NewProject_connectHandler(void) {
    Hal_connected();
}

void NewProject_disconnectHandler(void) {
    Hal_disconnected();
}

void NewProject_currentSceneId_store(NewProject_currentSceneId_t* input) {
    if (curMode == NewProject_EDIT) {
        curSceneId = *input;
        updateSceneLeds();
    }
}

void NewProject_currentScene_fetch(NewProject_currentScene_t* output) {
    decodeScene(sceneList[curSceneId], output);
    output->sceneId = curSceneId;
    updateSequenceScenes();
}

void NewProject_currentScene_store(NewProject_currentScene_t* input) {
    if (curMode == NewProject_EDIT) {
        sceneList[curSceneId] = encodeScene(input);
        updateSceneLeds();
    }
}

void NewProject_currentSequenceId_store(NewProject_currentSequenceId_t* input) {
    //if (curMode == NewProject_EDIT) {
       // curSequenceId = *input;
      //  updateSequenceScenes();
    //}
}

void NewProject_currentSequence_fetch(NewProject_currentSequence_t* output) {
    //*output = sequenceList[curSequenceId];
   // output->sequenceId = curSequenceId;
}

void NewProject_currentSequence_store(NewProject_currentSequence_t* input) {
   // if (curMode == NewProject_EDIT) {
       // sequenceList[curSequenceId] = *input;
       // updateSequenceScenes();
    //}
}

void NewProject_currentMode_fetch(NewProject_currentMode_t* output) {
   // *output = curMode;
}

void NewProject_currentMode_store(NewProject_currentMode_t* input) {
   // curMode = *input;
}

void NewProject_delay_fetch(NewProject_delay_t* output) {
    //*output = delayVal;
}

void NewProject_delay_store(NewProject_delay_t* input) {
    //delayVal = *input;
}

void NewProject_sceneState_fetch(NewProject_sceneState_t* output) {
    /* TODO: read resource 'sceneState' into 'output' */
}

void NewProject_SaveAndLoad_fetch(NewProject_SaveAndLoad_t* output) {
    //updateSceneLeds();
}

void NewProject_SaveAndLoad_store(NewProject_SaveAndLoad_t* input) {
    //updateSceneLeds();
}
