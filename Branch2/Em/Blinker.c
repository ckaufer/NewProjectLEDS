/**** DO NOT EDIT -- this file has been automatically generated from @emmoco.com.Blinker on 2013-11-26T21:36:02 ****/
/**** protocolLevel = 13, toolsVersion = 13.4.1.201311160446 ****/

#include "Em_Message.h"
#include "Blinker.h"

#ifdef __cplusplus
extern "C" {
#endif

#define Em_Message_protocolLevel 13

typedef struct Em_App_Message {
    uint8_t dummy[3];
    uint8_t sot;
    Em_Message_Header hdr;
    uint8_t data[20];
} Em_App_Message;

const uint8_t Em_App_hash[] = {100, 133, 204, 118, 171, 105, 196, 176, 165, 61, 177, 160, 190, 191, 194, 241, 13, 0, ((sizeof(struct{uint8_t f1; uint16_t f2;}) - sizeof(uint16_t)) << 4) | (sizeof(struct{uint8_t f1; uint32_t f2;}) - sizeof(uint32_t))};

const uint8_t Em_App_build[] = {116, 150, 14, 152, 66, 1, 0, 0};

#define Em_App_APP_RESOURCE_COUNT 4
#define Em_App_SYS_RESOURCE_COUNT 9

#define Em_App_ACCEPT Blinker_accept
#define Em_App_ACTIVATEPARAMETERS Blinker_activateParameters
#define Em_App_BROADCASTOFF Blinker_broadcastOff
#define Em_App_DISCONNECT Blinker_disconnect
#define Em_App_PAIRINGON Blinker_pairingOn
#define Em_App_PAIRINGOFF Blinker_pairingOff
#define Em_App_RESET Blinker_reset
#define Em_App_SETDEVICENAME Blinker_setDeviceName
#define Em_App_START Blinker_start

#define Em_App_CONNECTHANDLER Blinker_connectHandler
#define Em_App_DISCONNECTHANDLER Blinker_disconnectHandler

#define Em_App_MAX_INDICATOR 2

/* BEGIN common code */

enum {Em_App_IDLE, Em_App_STARTING, Em_App_DISCONNECTED, Em_App_CONNECTED};

typedef struct Em_App_Indicator {
    uint8_t dummy[3];
    uint8_t sot;
    Em_Message_Header hdr;
    uint8_t data[Em_Message_INDSIZE];
} Em_App_Indicator;

union { uint32_t align; Em_App_Message msg; } Em_App_msg_u;
union { uint32_t align; Em_App_Indicator ind; } Em_App_ind_u;
#define Em_App_msg Em_App_msg_u.msg
#define Em_App_ind Em_App_ind_u.ind

void (*Em_App_pdHdlr)(void);

const uint16_t Em_App_endian = 0x0100;

Em_Message_Size Em_App_recvIdx;
Em_Message_Size Em_App_recvSize;
Em_Message_Size Em_App_xmitIdx;
Em_Message_Size Em_App_xmitSize;

uint8_t Em_App_state = Em_App_IDLE;
int32_t Em_App_fileIndex = 0;
uint32_t Em_App_xmitMask = 0;

uint8_t* Em_App_valp;
uint8_t* Em_App_bufp;
const char* Em_App_desc;

uint8_t* Em_App_inBuf = (uint8_t*)&Em_App_msg.hdr;
uint8_t* Em_App_outBuf = 0;

uint8_t* _Em_Message_rxBuf = 0;
uint8_t _Em_Message_rxCnt = 0;

uint8_t* _Em_Message_txBuf = 0;
uint8_t _Em_Message_txCnt = 0;

#define Em_App_DEVNAME_LEN 9
const char* Em_App_devName = 0;

void Em_App_fetchDispatch(void);
void Em_Message_marshallToBuf(uint8_t* valp, uint8_t* bufp, const char* desc);
void Em_Message_marshallToVal(uint8_t* valp, uint8_t* bufp, const char* desc);
void Em_App_storeDispatch(void);
void Em_App_sendIndicator(Em_Message_ResId indId);
void Em_App_sendResponse(Em_Message_Kind kind, Em_Message_Size size);
void Em_App_startIndSend(void);
void Em_App_startResSend(void);
void Em_App_sysFetchDispatch(void);
void Em_App_sysStoreDispatch(void);
bool Em_App_xmitReady(Em_Message_ResId indId);

void Em_Message_nextXmit(void) {
    uint8_t key = Em_Hal_lock();
    if (Em_App_xmitMask != 0) {
        uint8_t i;
        uint32_t m;
        for (i = 0, m = 0x1; i < Em_App_MAX_INDICATOR; i++, m <<= 1) {
            if (Em_App_xmitMask & m) {
                Em_App_xmitMask &= ~m;
                if (i == 0) {
                    Em_App_startResSend();
                }
                else {
                    Em_App_sendIndicator(i - 1);
                }
                break;
            }
        }
    }
    Em_Hal_unlock(key);
}

void Em_Message_restart(void) {
    Em_App_START();
}

void Em_App_ACCEPT(bool enable) {
    if (Em_App_state == Em_App_CONNECTED) {
        return;
    }
    Em_App_ind.sot = 0;
    Em_App_ind.hdr.kind = Em_Message_ACCEPT;
    Em_App_ind.hdr.size = sizeof (Em_Message_Header);
    Em_App_ind.hdr.resId = enable;
    Em_App_startIndSend();
}

void Em_App_ACTIVATEPARAMETERS(uint8_t group) {
    if (Em_App_state == Em_App_IDLE || Em_App_state == Em_App_STARTING) {
        return;
    }
    Em_App_ind.sot = 0;
    Em_App_ind.hdr.kind = Em_Message_ACTIVE_PARAMS;
    Em_App_ind.hdr.size = sizeof (Em_Message_Header);
    Em_App_ind.hdr.resId = group;
    Em_App_startIndSend();
}

void Em_App_BROADCASTOFF(void) {
    Em_App_ind.sot = 0;
    Em_App_ind.hdr.kind = Em_Message_INDICATOR;
    Em_App_ind.hdr.size = sizeof (Em_Message_Header);
    Em_App_ind.hdr.resId = 0;
    Em_App_startIndSend();
}

void Em_App_DISCONNECT(void) {
    if (Em_App_state != Em_App_CONNECTED) {
        return;
    }
    Em_App_state = Em_App_DISCONNECTED;
    Em_App_ind.sot = 0;
    Em_App_ind.hdr.kind = Em_Message_DISCONNECT;
    Em_App_ind.hdr.size = sizeof (Em_Message_Header);
    Em_App_ind.hdr.resId = 0;
    Em_App_startIndSend();
}

void Em_Message_dispatch(void) {
    if (Em_App_state == Em_App_IDLE) {
        return;
    }
    switch (Em_App_msg.hdr.kind) {
        case Em_Message_CONNECT:
            Em_App_state = Em_App_CONNECTED;
            Em_App_CONNECTHANDLER();
            break;
        case Em_Message_DISCONNECT:
            Em_App_state = Em_App_DISCONNECTED;
            Em_App_DISCONNECTHANDLER();
            break;
        case Em_Message_PAIRING_DONE:
            if (Em_App_pdHdlr) {
                (*Em_App_pdHdlr)();
            }
            break;
        case Em_Message_FETCH:
            if (Em_App_msg.hdr.resId < 0x80) {
                Em_App_fetchDispatch();
            }
            else {
                Em_App_sysFetchDispatch();
            }
            break;
        case Em_Message_STORE:
            if (Em_App_msg.hdr.resId < 0x80) {
                Em_App_storeDispatch();
            }
            else {
                Em_App_sysStoreDispatch();
            }
            break;
    }
}

void Em_App_marshallToBuf() {
    char ch;
    while ((ch = *Em_App_desc++)) {
        switch (ch) {
            case '0' : {
                *Em_App_bufp++ = 0;
                break;
            }
            case '1' : {
                *Em_App_bufp++ = *Em_App_valp & 0xFF;
                break;
            }
            case '2' : {
                uint16_t v16 = *(uint16_t*)Em_App_valp;
                *Em_App_bufp++ = v16 & 0xFF;
                *Em_App_bufp++ = (v16 >> 8) & 0xFF;
                break;
            }
            case '4' : {
                if (((uint32_t)Em_App_valp & 0x1)) Em_App_valp++;
                uint32_t v32 = *(uint32_t*)Em_App_valp++;
                *Em_App_bufp++ = v32 & 0xFF;
                *Em_App_bufp++ = (v32 >> 8) & 0xFF;
                *Em_App_bufp++ = (v32 >> 16) & 0xFF;
                *Em_App_bufp++ = (v32 >> 24) & 0xFF;
                break;
            }
        }
        Em_App_valp += 1;
    }
}

void Em_App_marshallToVal() {
    char ch;
    while ((ch = *Em_App_desc++)) {
        switch (ch) {
            case '0' : {
                *Em_App_valp = 0;
                Em_App_bufp += 1;
                break;
            }
            case '1' : {
                *Em_App_valp = *Em_App_bufp++ & 0xFF;
                break;
            }
            case '2' : {
                uint16_t v16 = *Em_App_bufp++ & 0xFF;
                v16 |= (*Em_App_bufp++ << 8);
                *(uint16_t*)Em_App_valp = v16;
                break;
            }
            case '4' : {
                if (((uint32_t)Em_App_valp & 0x1)) Em_App_valp++;
                uint32_t v32 = (uint32_t)*Em_App_bufp++ & 0xFF;
                v32 |= ((uint32_t)*Em_App_bufp++ << 8);
                v32 |= ((uint32_t)*Em_App_bufp++ << 16);
                v32 |= ((uint32_t)*Em_App_bufp++ << 24);
                *(uint32_t*)Em_App_valp++ = v32;
                break;
            }
        }
        Em_App_valp += 1;
    }
}

void Em_App_PAIRINGOFF(void(*handler)(void)) {
    Em_App_PAIRINGON(0, handler);
}

void Em_App_PAIRINGON(uint8_t secs, void(*handler)(void)) {
    if (Em_App_state != Em_App_DISCONNECTED) {
        return;
    }
    Em_App_pdHdlr = handler;
    Em_App_ind.sot = 0;
    Em_App_ind.hdr.kind = Em_Message_PAIRING;
    Em_App_ind.hdr.size = sizeof (Em_Message_Header);
    Em_App_ind.hdr.resId = secs;
    Em_App_startIndSend();
}

void Em_App_RESET(void) {
    Em_Hal_reset();
    _Em_Message_rxBuf = _Em_Message_txBuf = 0;
    _Em_Message_rxCnt = _Em_Message_txCnt = 0;
    Em_App_recvIdx = Em_App_recvSize = Em_App_xmitIdx = Em_App_xmitSize = 0;
    Em_App_state = Em_App_IDLE;
    Em_App_fileIndex = 0;
    Em_App_xmitMask = 0;
}

void Em_App_SETDEVICENAME(const char* name) {
    Em_App_devName = name;
}

void Em_App_START(void) {
    Em_App_RESET();
    Em_App_state = Em_App_STARTING;
}

void Em_App_sendResponse(Em_Message_Kind kind, Em_Message_Size size) {
    if (Em_App_state != Em_App_IDLE) {
        Em_App_msg.sot = 0;
        Em_App_msg.hdr.kind = kind;
        Em_App_msg.hdr.size = size + sizeof (Em_Message_Header);
        if (Em_App_xmitReady(0)) {
            Em_App_startResSend();
        }
    }
}

void Em_App_startIndSend(void) {
    Em_App_outBuf = (uint8_t*)&Em_App_ind.sot;
    Em_App_xmitSize = Em_App_ind.hdr.size + 1;
    Em_App_xmitIdx = 0;
    Em_Hal_startSend();
}

void Em_App_startResSend(void) {
    Em_App_outBuf = (uint8_t*)&Em_App_msg.sot;
    Em_App_xmitSize = Em_App_msg.hdr.size + 1;
    Em_App_xmitIdx = 0;
    Em_Hal_startSend();
}

void Em_App_sysFetchDispatch(void) {
    uint8_t size = 0;
    int i, j;
    switch (Em_App_msg.hdr.resId) {
        case Em_Message_SYS_SCHEMA_HASH:
            for (i = 0; i < sizeof (Em_App_hash); i++) {
                Em_App_msg.data[i] = Em_App_hash[i];
            }
            Em_App_msg.data[sizeof (Em_App_hash)] = *((uint8_t*)&Em_App_endian);
            size = sizeof (Em_App_hash) + 1;
            break;
        case Em_Message_SYS_MCM_NAME:
            if (Em_App_devName) {
                for (i = 0; i < Em_App_DEVNAME_LEN; i++) {
                    if ((Em_App_msg.data[i] = Em_App_devName[i]) == 0) {
                        break;
                    }
                }
                for (j = i; j < Em_App_DEVNAME_LEN; j++) {
                    Em_App_msg.data[j] = 0;
                }
                size = Em_App_DEVNAME_LEN;
            }
            break;
        case Em_Message_SYS_EAP_PROTOCOL_LEVEL:
            *((Em_Message_protocolLevel_t*)Em_App_msg.data) = Em_Message_protocolLevel;
            size = sizeof (Em_Message_protocolLevel_t);
            break;
        case Em_Message_SYS_EAP_BUILD_DATE:
            for (i = 0; i < sizeof (Em_App_build); i++) {
                Em_App_msg.data[i] = Em_App_build[i];
            }
            size = sizeof (Em_App_build);
            break;
        case Em_Message_SYS_RESOURCE_COUNT:
            Em_App_msg.data[0] = Em_App_APP_RESOURCE_COUNT;
            Em_App_msg.data[1] = Em_App_SYS_RESOURCE_COUNT;
            size = 2;
            break;
    }
    Em_App_sendResponse(Em_Message_FETCH_DONE, size);
}

void Em_App_sysStoreDispatch(void) {
    switch (Em_App_msg.hdr.resId) {
        case Em_Message_SYS_FILE_INDEX_RESET:
            Em_App_fileIndex = 0;
            break;
    }
    Em_App_sendResponse(Em_Message_STORE_DONE, 0);
}

bool Em_App_xmitReady(Em_Message_ResId indId) {
    uint8_t key = Em_Hal_lock();
    bool res = _Em_Message_txBuf == 0 && Em_App_xmitMask == 0;
    if (!res) {
        Em_App_xmitMask |= (1 << indId);
    }
    Em_Hal_unlock(key);
    return res;    
}

/* END common code */

void Em_App_fetchDispatch(void) {
    uint8_t size = 0;
    switch (Em_App_msg.hdr.resId) {
        case 0: {
            break;
        }
        case 2: {
#ifdef Em_16BIT
            Blinker_count_t val;
            Em_App_valp = (uint8_t*)&val;
            Em_App_bufp = Em_App_msg.data;
            Em_App_desc = "2";
            Blinker_count_fetch(&val);
            Em_App_marshallToBuf();
#else
            Blinker_count_fetch((void*)Em_App_msg.data);
#endif
            size = 2;
            break;
        }
        case 3: {
#ifdef Em_16BIT
            Blinker_delay_t val;
            Em_App_valp = (uint8_t*)&val;
            Em_App_bufp = Em_App_msg.data;
            Em_App_desc = "2";
            Blinker_delay_fetch(&val);
            Em_App_marshallToBuf();
#else
            Blinker_delay_fetch((void*)Em_App_msg.data);
#endif
            size = 2;
            break;
        }
        case 4: {
#ifdef Em_16BIT
            Blinker_ledState_t val;
            Em_App_valp = (uint8_t*)&val;
            Em_App_bufp = Em_App_msg.data;
            Em_App_desc = "1";
            Blinker_ledState_fetch(&val);
            Em_App_marshallToBuf();
#else
            Blinker_ledState_fetch((void*)Em_App_msg.data);
#endif
            size = 1;
            break;
        }
    }
    Em_App_sendResponse(Em_Message_FETCH_DONE, size);
}

void Em_App_storeDispatch(void) {
    switch (Em_App_msg.hdr.resId) {
        case 0: {
            break;
        }
        case 1: {
#ifdef Em_16BIT
            Blinker_cmd_t val;
            Em_App_valp = (uint8_t*)&val;
            Em_App_bufp = Em_App_msg.data;
            Em_App_desc = "1";
            Em_App_marshallToVal();
            Blinker_cmd_store(&val);
#else
            Blinker_cmd_store((void*)Em_App_msg.data);
#endif
            break;
        }
        case 2: {
#ifdef Em_16BIT
            Blinker_count_t val;
            Em_App_valp = (uint8_t*)&val;
            Em_App_bufp = Em_App_msg.data;
            Em_App_desc = "2";
            Em_App_marshallToVal();
            Blinker_count_store(&val);
#else
            Blinker_count_store((void*)Em_App_msg.data);
#endif
            break;
        }
        case 3: {
#ifdef Em_16BIT
            Blinker_delay_t val;
            Em_App_valp = (uint8_t*)&val;
            Em_App_bufp = Em_App_msg.data;
            Em_App_desc = "2";
            Em_App_marshallToVal();
            Blinker_delay_store(&val);
#else
            Blinker_delay_store((void*)Em_App_msg.data);
#endif
            break;
        }
    }
    Em_App_sendResponse(Em_Message_STORE_DONE, 0);
}

void Em_App_sendIndicator(Em_Message_ResId indId) {
    Em_Message_Size resId = 0;
    Em_Message_Size size = 0;
    switch (indId) {
        case 1: {
#ifdef Em_16BIT
            Blinker_ledState_t val;
            Em_App_valp = (uint8_t*)&val;
            Em_App_bufp = Em_App_ind.data;
            Em_App_desc = "1";
            Blinker_ledState_fetch(&val);
            Em_App_marshallToBuf();
#else
            Blinker_ledState_fetch((Blinker_ledState_t*)&Em_App_ind.data);
#endif
            resId = 4;
            size = 1;
            break;
        }
    }
    Em_App_ind.sot = 0;
    Em_App_ind.hdr.kind = Em_Message_INDICATOR;
    Em_App_ind.hdr.size = sizeof (Em_Message_Header) + size;
    Em_App_ind.hdr.resId = resId;
    Em_App_startIndSend();
}

void Blinker_ledState_indicate(void) {
    if (Em_App_state == Em_App_CONNECTED && Em_App_xmitReady(1 + 1)) Em_App_sendIndicator(1);
}

#ifdef __cplusplus
}
#endif

