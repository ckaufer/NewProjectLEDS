#include "NewProject.h"
#include "Hal.h"
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
//The Pragmatic Programmer, Dry principal

#define SCENEADDRESS 0
#define offsetof_elem(type, memb, index) \ 
                 ((size_t)(offsetof(type, memb[0]) + \ 
                           (size_t)(index) * sizeof(((type *)0)->memb[0]) ))
#define SCENE_OFFSET() offsetof_elem(Database, sceneTable, curSceneId)
#define SEQUENCE_OFFSET() offsetof_elem(Database, sequenceTable, curSequenceId)
#define LEDSTATE(mask, n) (mask & (1 << n) ? NewProject_ON : NewProject_OFF)
//declare helper functions above
//identities, | and &
//cleanup the code, test everything, readup. How to compute offset of an element of a structure of arrays?
//top of a file, only decleration (header file)
//constants, types, functions get declared -> first (with semicolon). global variables -> 2nd, functions -> 3rd alphabetise
typedef uint16_t SceneMask;
typedef struct {
    NewProject_Sequence sequenceTable[NewProject_SequenceId_max];
    SceneMask sceneTable[NewProject_SceneId_max];
} Database;

SceneMask sceneList[NewProject_SceneId_max];
NewProject_Sequence sequenceList[NewProject_SequenceId_max];
NewProject_currentSequence_t currentSequence;
NewProject_currentSceneId_t curSceneId;
NewProject_currentSequenceId_t curSequenceId;
NewProject_currentMode_t curMode;
NewProject_SaveAndLoad_t saveAndLoad;
NewProject_delay_t delayVal = 0.8 * NewProject_delay_scale;
NewProject_test_t test;
NewProject_delay_t curTime = 0;
uint16_t curSceneIdx = 0;



SceneMask encodeScene(NewProject_currentScene_t*);
static void decodeScene(SceneMask, NewProject_Scene*);
static void updateLed(uint8_t, SceneMask);
static void updateSceneLeds(void);
static void updateSequenceScenes(void);
static void tickHandler(void);
static void loadAll(void);

void main() {
    Hal_init();
    loadAll();
	Hal_tickStart(NewProject_delay_step, tickHandler);
	NewProject_start();    
	Hal_idleLoop();
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
    updateSceneLeds();
}

loadScene(SceneMask sceneNum) {
    SceneMask* temp;
    Hal_User_eepromRead(SCENE_OFFSET(), temp, sizeof(SceneMask));
    if (temp == 65535 || sceneList[sceneNum] == 65535) {
        sceneList[sceneNum] = 0;
    }
    else {
        sceneList[sceneNum] = *temp;
    }

    updateSceneLeds();
}

saveScene(SceneMask sceneNum) {
    Hal_User_eepromWrite(SCENE_OFFSET(), &sceneList[sceneNum], sizeof(SceneMask));
}

loadSequence(NewProject_currentSequenceId_t sequenceNum) {
    NewProject_Sequence* temp;
    Hal_User_eepromRead(SEQUENCE_OFFSET() + SCENE_OFFSET(), temp, sizeof(NewProject_Sequence));
    sequenceList[sequenceNum] = *temp;
    updateSequenceScenes();
}

saveSequence(NewProject_currentSequenceId_t sequnceNum) {
    Hal_User_eepromWrite(SEQUENCE_OFFSET(), sequenceList[sequnceNum], sizeof(NewProject_Sequence));
}

void loadAll() {
    for(SceneMask i = 0; i <= NewProject_currentSceneId_max; i++) {
        loadScene(i);
    }
    for(SceneMask i = 0; i <= NewProject_currentSequenceId_max; i++) {
        loadSequence(i);
    }
}

void tickHandler() {
    
    if (curTime < delayVal) {
        curTime += NewProject_delay_step;
        return;
    }
    
    if (curMode == NewProject_PLAY) { 
		if(sequenceList[curSequenceId].maxRange != sequenceList[curSequenceId].minRange) {
	        if (curSceneIdx <= sequenceList[curSequenceId].maxRange) {
				sequenceList[curSequenceId].seqLength = curSceneIdx++;
			}
			else {
	            curSceneIdx = sequenceList[curSequenceId].minRange;
			}
			updateSequenceScenes();
	    }
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
        loadScene(curSceneId);
    }
}

void NewProject_currentScene_fetch(NewProject_currentScene_t* output) {
    //Hal_User_eepromRead(SCENEADDRESS, sceneList[curSceneId], sizeof(SceneMask));
    loadScene(curSceneId);
    decodeScene(sceneList[curSceneId], output);
    output->sceneId = curSceneId;
    updateSceneLeds();
}

void NewProject_currentScene_store(NewProject_currentScene_t* input) {
    if (curMode == NewProject_EDIT) {
        sceneList[curSceneId] = encodeScene(input);
        //Hal_User_eepromWrite(SCENEADDRESS, encodeScene(input), sizeof(SceneMask));
        saveScene(curSceneId);
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
    //loadSequence(curSequenceId);
    *output = sequenceList[curSequenceId];
    output->sequenceId = curSequenceId;
}

void NewProject_currentSequence_store(NewProject_currentSequence_t* input) {
   // if (curMode == NewProject_EDIT) {
        sequenceList[curSequenceId] = *input;
        //saveSequence(curSequenceId);
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
    *output = saveAndLoad;
}

void NewProject_SaveAndLoad_store(NewProject_SaveAndLoad_t* input) {
    saveAndLoad = *input;
}


void NewProject_test_fetch(NewProject_test_t* output) {
    ///*output = sceneList[curSceneId];
    //Hal_User_eepromRead(SCENEADDRESS, output, sizeof(NewProject_test_t));
    //loadScene(curSceneId);
    //sceneList[curSceneId] = *output;
    *output = sizeof(sceneList);
}

void NewProject_test_store(NewProject_test_t* input) {
    //Hal_User_eepromWrite(SCENEADDRESS, input, sizeof(NewProject_test_t));
    SceneMask sceneInt = 6;
    saveScene(curSceneId);
    //Hal_User_eepromWrite(SCENEADDRESS, &sceneInt, sizeof(SceneMask));
    //test = *input;
    //sceneList[curSceneId] = *input;
    updateSceneLeds();
}
