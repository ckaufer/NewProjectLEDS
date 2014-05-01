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
    result |= (input->led2) << 2;
    result |= (input->led3) << 3;
    result |= (input->led4) << 4;
    result |= (input->led5) << 5;
    result |= (input->led6) << 6;
    result |= (input->led7) << 7;
    result |= (input->led8) << 8;
    result |= (input->led9) << 9;
    result |= (input->led10) << 10;
    result |= (input->led11) << 11;
    result |= (input->led12) << 12;
    result |= (input->led13) << 13;
    result |= (input->led14) << 14;
    result |= (input->led15) << 15;
    return result;
}

void decodeScene(SceneMask mask, NewProject_Scene* output) {
    output->led2 = LEDSTATE(mask, 2);
    output->led3 = LEDSTATE(mask, 3);
    output->led4 = LEDSTATE(mask, 4);
    output->led5 = LEDSTATE(mask, 5);
    output->led6 = LEDSTATE(mask, 6);
    output->led7 = LEDSTATE(mask, 7);
    output->led8 = LEDSTATE(mask, 8);
    output->led9 = LEDSTATE(mask, 9);
    output->led10 = LEDSTATE(mask, 10);
    output->led11 = LEDSTATE(mask, 11);
    output->led12 = LEDSTATE(mask, 12);
    output->led13 = LEDSTATE(mask, 13);
    output->led14 = LEDSTATE(mask, 14);
    output->led15 = LEDSTATE(mask, 15);
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
    updateLed(2, LEDSTATE(mask, 2));
    updateLed(3, LEDSTATE(mask, 3));
    updateLed(4, LEDSTATE(mask, 4));
    updateLed(5, LEDSTATE(mask, 5));
    updateLed(6, LEDSTATE(mask, 6));
    updateLed(7, LEDSTATE(mask, 7));
    updateLed(8, LEDSTATE(mask, 8));
    updateLed(9, LEDSTATE(mask, 9));
    updateLed(10, LEDSTATE(mask, 10));
    updateLed(11, LEDSTATE(mask, 11));
    updateLed(12, LEDSTATE(mask, 12));
    updateLed(13, LEDSTATE(mask, 13));
    updateLed(14, LEDSTATE(mask, 14));
    updateLed(15, LEDSTATE(mask, 15));
}

void updateSequenceScenes() {
    curSceneId = sequenceList[curSequenceId].sceneList[sequenceList[curSequenceId].seqLength];
    //sceneList[curSceneId]->sceneId = curSceneId;
    updateSceneLeds();
}

void tickHandler() {
    
    if (curTime < delayVal) {
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
    curTime = 0;
    NewProject_sceneState_indicate;
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
    if (curMode == NewProject_EDIT) {
        curSequenceId = *input;
        updateSequenceScenes();
    }
}

void NewProject_currentSequence_fetch(NewProject_currentSequence_t* output) {
    *output = sequenceList[curSequenceId];
    output->sequenceId = curSequenceId;
}

void NewProject_currentSequence_store(NewProject_currentSequence_t* input) {
   // if (curMode == NewProject_EDIT) {
        sequenceList[curSequenceId] = *input;
        updateSequenceScenes();
    //}
}

void NewProject_currentMode_fetch(NewProject_currentMode_t* output) {
    *output = curMode;
}

void NewProject_currentMode_store(NewProject_currentMode_t* input) {
    curMode = *input;
}

void NewProject_delay_fetch(NewProject_delay_t* output) {
    *output = delayVal;
}

void NewProject_delay_store(NewProject_delay_t* input) {
    delayVal = *input;
}

void NewProject_sceneState_fetch(NewProject_sceneState_t* output) {
    /* TODO: read resource 'sceneState' into 'output' */
}

void NewProject_SaveAndLoad_fetch(NewProject_SaveAndLoad_t* output) {
    updateSceneLeds();
}

void NewProject_SaveAndLoad_store(NewProject_SaveAndLoad_t* input) {
    updateSceneLeds();
}
