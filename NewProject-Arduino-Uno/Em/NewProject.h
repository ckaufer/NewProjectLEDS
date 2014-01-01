/**** DO NOT EDIT -- this file has been automatically generated from @emmoco.com.NewProject on 2014-01-01T15:07:37 ****/
/**** protocolLevel = 13, toolsVersion = 13.4.1.201311160446 ****/

#ifndef NewProject__H
#define NewProject__H

#include "Em_Types.h"
#include "Em_Message.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------- connection callback functions implemented by the application -------- */

void NewProject_connectHandler(void);
void NewProject_disconnectHandler(void);

/* -------- resource types defined in the schema -------- */

/* typedef SceneId */
typedef uint8_t NewProject_SceneId;
#define NewProject_SceneId_min 0
#define NewProject_SceneId_max 3
#define NewProject_SceneId_step 1
#define NewProject_SceneId_scale 1
static inline uint8_t NewProject_SceneId_pack(NewProject_SceneId v) { return (v - NewProject_SceneId_min) / NewProject_SceneId_step; }
static inline NewProject_SceneId NewProject_SceneId_unpack(uint8_t v) { return NewProject_SceneId_min + v / NewProject_SceneId_step; }

/* enum LedState */
typedef uint8_t NewProject_LedState;
#define NewProject_OFF 0
#define NewProject_ON 1

/* struct Scene */
typedef struct NewProject_Scene {
    NewProject_SceneId sceneId;
    NewProject_LedState led2;
    NewProject_LedState led3;
    NewProject_LedState led4;
    NewProject_LedState led5;
    NewProject_LedState led6;
    NewProject_LedState led7;
    NewProject_LedState led8;
    NewProject_LedState led9;
    NewProject_LedState led10;
    NewProject_LedState led11;
    NewProject_LedState led12;
    NewProject_LedState led13;
    NewProject_LedState led14;
    NewProject_LedState led15;
} NewProject_Scene;

/* typedef SequenceId */
typedef uint8_t NewProject_SequenceId;
#define NewProject_SequenceId_min 0
#define NewProject_SequenceId_max 2
#define NewProject_SequenceId_step 1
#define NewProject_SequenceId_scale 1
static inline uint8_t NewProject_SequenceId_pack(NewProject_SequenceId v) { return (v - NewProject_SequenceId_min) / NewProject_SequenceId_step; }
static inline NewProject_SequenceId NewProject_SequenceId_unpack(uint8_t v) { return NewProject_SequenceId_min + v / NewProject_SequenceId_step; }

/* typedef SequenceLength */
typedef uint8_t NewProject_SequenceLength;
#define NewProject_SequenceLength_min 0
#define NewProject_SequenceLength_max 5
#define NewProject_SequenceLength_step 1
#define NewProject_SequenceLength_scale 1
static inline uint8_t NewProject_SequenceLength_pack(NewProject_SequenceLength v) { return (v - NewProject_SequenceLength_min) / NewProject_SequenceLength_step; }
static inline NewProject_SequenceLength NewProject_SequenceLength_unpack(uint8_t v) { return NewProject_SequenceLength_min + v / NewProject_SequenceLength_step; }

/* struct Sequence */
typedef struct NewProject_Sequence {
    NewProject_SequenceId sequenceId;
    NewProject_SequenceLength seqLength;
    NewProject_SceneId sceneList[5];
} NewProject_Sequence;

/* enum Mode */
typedef uint8_t NewProject_Mode;
#define NewProject_PLAY 0
#define NewProject_EDIT 1

/* -------- resource callback functions implemented by the application -------- */

/* resource currentSceneId */
typedef NewProject_SceneId NewProject_currentSceneId_t;
#define NewProject_currentSceneId_min 0
#define NewProject_currentSceneId_max 3
#define NewProject_currentSceneId_step 1
#define NewProject_currentSceneId_scale 1
extern void NewProject_currentSceneId_store(NewProject_currentSceneId_t* input);

/* resource currentScene */
typedef NewProject_Scene NewProject_currentScene_t;
extern void NewProject_currentScene_fetch(NewProject_currentScene_t* output);
extern void NewProject_currentScene_store(NewProject_currentScene_t* input);

/* resource currentSequenceId */
typedef NewProject_SequenceId NewProject_currentSequenceId_t;
#define NewProject_currentSequenceId_min 0
#define NewProject_currentSequenceId_max 2
#define NewProject_currentSequenceId_step 1
#define NewProject_currentSequenceId_scale 1
extern void NewProject_currentSequenceId_store(NewProject_currentSequenceId_t* input);

/* resource currentSequence */
typedef NewProject_Sequence NewProject_currentSequence_t;
extern void NewProject_currentSequence_fetch(NewProject_currentSequence_t* output);
extern void NewProject_currentSequence_store(NewProject_currentSequence_t* input);

/* resource currentMode */
typedef NewProject_Mode NewProject_currentMode_t;
extern void NewProject_currentMode_fetch(NewProject_currentMode_t* output);
extern void NewProject_currentMode_store(NewProject_currentMode_t* input);

/* -------- application functions implemented in NewProject.c -------- */

void NewProject_accept(bool enable);
void NewProject_activateParameters(uint8_t group);
void NewProject_broadcastOff(void);
void NewProject_disconnect(void);
void NewProject_pairingOn(uint8_t secs, void(*handler)(void));
void NewProject_pairingOff(void(*handler)(void));
void NewProject_reset(void);
void NewProject_setDeviceName(const char* name);
void NewProject_start(void);

#ifdef __cplusplus
}
#endif

#endif /* NewProject__H */
