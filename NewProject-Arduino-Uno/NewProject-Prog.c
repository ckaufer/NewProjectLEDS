#include "NewProject.h"
#include "Hal.h"

NewProject_Scene sceneList[NewProject_SceneId_max];
NewProject_Sequence sequenceList[NewProject_SequenceId_max];
NewProject_currentSceneId_t curSceneId;
NewProject_currentMode_t curMode;

void main() {
	Hal_init();
	NewProject_start();    
	Hal_idleLoop();
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
    /* TODO: write resource 'currentSequenceId' from 'input' */
}

void NewProject_currentSequence_fetch(NewProject_currentSequence_t* output) {
    /* TODO: read resource 'currentSequence' into 'output' */
}

void NewProject_currentSequence_store(NewProject_currentSequence_t* input) {
    /* TODO: write resource 'currentSequence' from 'input' */
}

void NewProject_currentMode_fetch(NewProject_currentMode_t* output) {
    *output = curMode;
}

void NewProject_currentMode_store(NewProject_currentMode_t* input) {
    curMode = *input;
}
