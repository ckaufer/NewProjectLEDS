#include "NewProject.h"
#include "Hal.h"
#include <avr/pgmspace.h>

NewProject_Scene sceneList[NewProject_SceneId_max];
NewProject_Sequence sequenceList[NewProject_SequenceId_max];
NewProject_currentSequence_t currentSequence;
NewProject_currentSceneId_t curSceneId;
NewProject_currentSequenceId_t curSequenceId;
NewProject_currentMode_t curMode;
NewProject_delay_t delayVal = 0.8 * NewProject_delay_scale;
NewProject_occupiedScenes_t occupiedScenes = 2;
uint16_t curSceneIdx = 0;
static void loop(void);

void main() {
	Hal_init();
	Hal_tickStart(1000, loop);
	NewProject_start();    
	Hal_idleLoop();
	//PROGMEM  prog_uint16_t charSet[]  = {3, 3, 4};
}

void updateLed(uint8_t id, NewProject_LedState ledState) {
    if (ledState) {
        Hal_User_ledOn(id);
    }
    else {
        Hal_User_ledOff(id);
    }
}

void updateSceneLeds() {
    NewProject_currentScene_t* scenePtr = &sceneList[curSceneId];
    updateLed(2, scenePtr->led2);
    updateLed(3, scenePtr->led3);
    updateLed(4, scenePtr->led4);
    updateLed(5, scenePtr->led5);
    updateLed(6, scenePtr->led6);
    updateLed(7, scenePtr->led7);
    updateLed(8, scenePtr->led8);
    updateLed(9, scenePtr->led9);
    updateLed(10, scenePtr->led10);
    updateLed(11, scenePtr->led11);
    updateLed(12, scenePtr->led12);
    updateLed(13, scenePtr->led13);
    updateLed(14, scenePtr->led14);
    updateLed(15, scenePtr->led15);
}

void updateSequenceScenes() {
    curSceneId = sequenceList[curSequenceId].sceneList[sequenceList[curSequenceId].seqLength];
    sceneList[curSceneId].sceneId = curSceneId;
    updateSceneLeds();
}

void loop() {
    if (curMode == NewProject_PLAY) { 
		if (curSceneIdx <= NewProject_SequenceLength_max) {
			sequenceList[curSequenceId].seqLength = curSceneIdx++;
		}
		else {
            curSceneIdx = 0;
		}
		updateSequenceScenes();
    }
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
    *output = sceneList[curSceneId];
    output->sceneId = curSceneId;
}

void NewProject_currentScene_store(NewProject_currentScene_t* input) {
    if (curMode == NewProject_EDIT) {
        sceneList[curSceneId] = *input;
        updateSceneLeds();
    }
}

void NewProject_currentSequenceId_store(NewProject_currentSequenceId_t* input) {
    if (curMode == NewProject_EDIT) {
        curSequenceId = *input;
    }
}

void NewProject_currentSequence_fetch(NewProject_currentSequence_t* output) {
    *output = sequenceList[curSequenceId];
    output->sequenceId = curSequenceId;
}

void NewProject_currentSequence_store(NewProject_currentSequence_t* input) {
    if (curMode == NewProject_EDIT) {
        sequenceList[curSequenceId] = *input;
        updateSequenceScenes();
    }
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

void NewProject_occupiedScenes_fetch(NewProject_occupiedScenes_t* output) {
    *output = occupiedScenes;
}

void NewProject_occupiedScenes_store(NewProject_occupiedScenes_t* input) {
    occupiedScenes = *input;
}
