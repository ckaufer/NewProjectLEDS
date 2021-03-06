#include "Blinker.h"
#include "Hal.h"
#include "avr/interrupt.h"
#include "avr/io.h"
static void tickHandler(void);
#define TICK_RATE  (500)
static Blinker_cmd_t cmdVal = Blinker_START_CMD;
static Blinker_count_t countVal = 0;
static Blinker_delay_t delayVal = (TICK_RATE); //=1000, delayVal = 1200


#define FOREVER -1
static Blinker_delay_t curTime = 0;

#define MIN_LED_NUM 2
#define MAX_LED_NUM 15
#define LED_NONE 0
#define LED_ALL 0xFFFF
#define LED2 (1 << 2)
#define LED3 (1 << 3)
#define LED4 (1 << 4)
#define LED5 (1 << 5)
#define LED6 (1 << 6)
#define LED7 (1 << 7)
#define LED8 (1 << 8)
#define LED9 (1 << 9)
#define LED10 (1 << 10)
#define LED11 (1 << 11)
#define LED12 (1 << 12)
#define LED13 (1 << 13)
#define LED14 (1 << 14)
#define LED15 (1 << 15)
#define CONNECTTIME (3000)
#define NOTCONNECTED -1
#define NUM_PATTERNS 4

static uint16_t arraySizes[4] = {0, 15, 7, 15, 15};
//static uint16_t ledBitArray[12]={3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 0, 1};
const uint16_t ledArray1[15]={LED2, LED3, LED4, LED5, LED6, LED7, LED8, LED15, LED14, LED13, LED12, LED11, LED10, LED9};
static uint16_t ledArray2[15]={LED2+LED9, LED3+LED10, LED4+LED11, LED5+LED12, LED6+LED13, LED7+LED14, LED8+LED15, LED7+LED14, LED6+LED13, LED5+LED12, LED4+LED11, LED3+LED10, LED2+LED9};
static uint16_t ledArray3[15]={LED_ALL, LED_NONE, LED_ALL, LED_NONE, LED_ALL, LED_NONE, LED_ALL, LED_NONE, LED_ALL, LED_NONE, LED_ALL, LED_NONE, LED_ALL, LED_NONE, LED_ALL};
static uint16_t ledArray4[15]={LED2+LED15, LED3+LED14, LED4+LED13, LED5+LED12, LED6+LED11, LED7+LED10, LED8+LED9, LED2+LED15, LED3+LED14, LED4+LED13, LED5+LED12, LED6+LED11, LED7+LED10, LED8+LED9, LED2+LED15};
static uint16_t curLedIdx;
static int16_t connectTime = NOTCONNECTED;
typedef uint16_t* Pattern;
Pattern patterns[] = {ledArray1, ledArray2, ledArray3, ledArray4};
static Pattern curPattern = 0;

void main() {
    Hal_init();
    Hal_tickStart(TICK_RATE/5, tickHandler);
    Blinker_start();
    Hal_idleLoop();
    
}

void ledsOff() {
    int i;
    for(i = MAX_LED_NUM; i >= MIN_LED_NUM; i--) {
        Hal_User_ledOff(i);
    }
}

static void tickHandler(void) {

    if (connectTime == 0) {
        Blinker_disconnect();
        connectTime = NOTCONNECTED;
        return;
    }
    
    if (connectTime > 0) {
        connectTime -= (1 * TICK_RATE / 100);
    }
    
    if (cmdVal == Blinker_STOP_CMD) {
        return;
    }
    
    if (curTime < delayVal) {
        curTime += TICK_RATE;
        return;
    }

    if (curPattern == 0) {
       // ledsOff();
        Hal_ledToggle();
    }
    else {
        uint16_t mask = curPattern[curLedIdx++];
        if (curLedIdx == 15) {
            curLedIdx = 0;
        }
        uint16_t i;
        for (i = MIN_LED_NUM; i <= MAX_LED_NUM; i++) {
            if (mask & (1 << i)) {
                Hal_User_ledOn(i);
            }
            else {
                Hal_User_ledOff(i);
            }
        }
    }   
    curTime = 0;
    Blinker_ledState_indicate();   
}

/* -------- SCHEMA CALLBACKS -------- */

void Blinker_connectHandler(void) {
    Hal_connected();
    connectTime = CONNECTTIME;
}

void Blinker_disconnectHandler(void) {
    Hal_disconnected();
    connectTime = NOTCONNECTED;
}

void Blinker_cmd_store(Blinker_cmd_t* input) {
    cmdVal = *input;
    uint8_t i;
    switch (cmdVal) {
        case Blinker_START_CMD:
            curTime = 0;
            countVal = 0;
            curLedIdx = 0;
            break;
        case Blinker_STOP_CMD:
        return;
            break;
    }
}

void Blinker_count_fetch(Blinker_count_t* output) {
    *output = countVal;
	if (countVal > NUM_PATTERNS) {
	   countVal = NUM_PATTERNS;
	}
}

void Blinker_count_store(Blinker_count_t* input) {
    countVal = *input;
    curPattern = countVal == 0 ? 0 : patterns[(countVal - 1) % NUM_PATTERNS];
    if (countVal > NUM_PATTERNS) {
        countVal = NUM_PATTERNS;
    }
}

void Blinker_delay_fetch(Blinker_delay_t* output) {
    *output = delayVal;
}

void Blinker_delay_store(Blinker_delay_t* input) {
    delayVal = *input;
}

void Blinker_ledState_fetch(Blinker_ledState_t* output) {
    *output = Hal_ledRead() ? Blinker_LED_ON : Blinker_LED_OFF;
}