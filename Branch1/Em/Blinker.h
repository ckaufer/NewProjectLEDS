/**** DO NOT EDIT -- this file has been automatically generated from @emmoco.com.Blinker on 2013-11-29T12:36:52 ****/
/**** protocolLevel = 13, toolsVersion = 13.4.1.201311160446 ****/

#ifndef Blinker__H
#define Blinker__H

#include "Em_Types.h"
#include "Em_Message.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------- connection callback functions implemented by the application -------- */

void Blinker_connectHandler(void);
void Blinker_disconnectHandler(void);

/* -------- resource types defined in the schema -------- */

/* enum Cmd */
typedef uint8_t Blinker_Cmd;
#define Blinker_START_CMD 0
#define Blinker_STOP_CMD 1

/* enum LedState */
typedef uint8_t Blinker_LedState;
#define Blinker_LED_OFF 0
#define Blinker_LED_ON 1

/* -------- resource callback functions implemented by the application -------- */

/* resource cmd */
typedef Blinker_Cmd Blinker_cmd_t;
extern void Blinker_cmd_store(Blinker_cmd_t* input);

/* resource count */
typedef int16_t Blinker_count_t;
extern void Blinker_count_fetch(Blinker_count_t* output);
extern void Blinker_count_store(Blinker_count_t* input);

/* resource delay */
typedef uint16_t Blinker_delay_t;
#define Blinker_delay_min 500
#define Blinker_delay_max 2000
#define Blinker_delay_step 100
#define Blinker_delay_scale 1000
extern void Blinker_delay_fetch(Blinker_delay_t* output);
extern void Blinker_delay_store(Blinker_delay_t* input);

/* resource ledState */
typedef Blinker_LedState Blinker_ledState_t;
extern void Blinker_ledState_fetch(Blinker_ledState_t* output);
extern void Blinker_ledState_indicate(void);  /* called by the application */

/* -------- application functions implemented in Blinker.c -------- */

void Blinker_accept(bool enable);
void Blinker_activateParameters(uint8_t group);
void Blinker_broadcastOff(void);
void Blinker_disconnect(void);
void Blinker_pairingOn(uint8_t secs, void(*handler)(void));
void Blinker_pairingOff(void(*handler)(void));
void Blinker_reset(void);
void Blinker_setDeviceName(const char* name);
void Blinker_start(void);

#ifdef __cplusplus
}
#endif

#endif /* Blinker__H */
