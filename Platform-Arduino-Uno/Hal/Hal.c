#include "Hal.h"
#include "Em_Message.h"
#include "avr/interrupt.h"
#include "avr/io.h"
#include "arduino/Arduino.h"
#include <avr/eeprom.h>

/* -------- INTERNAL FEATURES -------- */

#define LED13_BIT                     (1 << 5)
#define LED13_CONFIG()                (DDRB |= LED13_BIT)
#define LED13_ON()                    (PORTB |= LED13_BIT)
#define LED13_OFF()                   (PORTB &= ~LED13_BIT)
#define LED13_READ()                  (PORTB & LED13_BIT)
#define LED13_TOGGLE()                (PORTB ^= LED13_BIT)

#define LED14_BIT                     (1 << 0)
#define LED14_CONFIG()                (DDRC |= LED14_BIT)
#define LED14_ON()                    (PORTC |= LED14_BIT)
#define LED14_OFF()                   (PORTC &= ~LED14_BIT)
#define LED14_READ()                  (PORTC & LED14_BIT)
#define LED14_TOGGLE()                (PORTC ^= LED14_BIT)

#define LED15_BIT                     (1 << 1)
#define LED15_CONFIG()                (DDRC |= LED15_BIT)
#define LED15_ON()                    (PORTC |= LED15_BIT)
#define LED15_OFF()                   (PORTC &= ~LED15_BIT)
#define LED15_READ()                  (PORTC & LED15_BIT)
#define LED15_TOGGLE()                (PORTC ^= LED15_BIT)


#define LED11_BIT                     (1 << 3)
#define LED11_CONFIG()                (DDRB |= LED11_BIT)
#define LED11_ON()                    (PORTB |= LED11_BIT)
#define LED11_OFF()                   (PORTB &= ~LED11_BIT)
#define LED11_READ()                  (PORTB & LED11_BIT)
#define LED11_TOGGLE()                (PORTB ^= LED11_BIT)

#define LED4_BIT                     (1 << 4)
#define LED4_CONFIG()                (DDRD |= LED4_BIT)
#define LED4_ON()                    (PORTD |= LED4_BIT)
#define LED4_OFF()                   (PORTD &= ~LED4_BIT)
#define LED4_READ()                  (PORTD & LED4_BIT)
#define LED4_TOGGLE()                (PORTD ^= LED4_BIT)

#define LED12_BIT                     (1 << 4)
#define LED12_CONFIG()                (DDRB |= LED12_BIT)
#define LED12_ON()                    (PORTB |= LED12_BIT)
#define LED12_OFF()                   (PORTB &= ~LED12_BIT)
#define LED12_READ()                  (PORTB & LED12_BIT)
#define LED12_TOGGLE()                (PORTB ^= LED12_BIT)

#define LED9_BIT                     (1 << 1)
#define LED9_CONFIG()                (DDRB |= LED9_BIT)
#define LED9_ON()                    (PORTB |= LED9_BIT)
#define LED9_OFF()                   (PORTB &= ~LED9_BIT)
#define LED9_READ()                  (PORTB & LED9_BIT)
#define LED9_TOGGLE()                (PORTB ^= LED9_BIT)

#define LED8_BIT                     (1 << 0)
#define LED8_CONFIG()                (DDRB |= LED8_BIT)
#define LED8_ON()                    (PORTB |= LED8_BIT)
#define LED8_OFF()                   (PORTB &= ~LED8_BIT)
#define LED8_READ()                  (PORTB & LED8_BIT)
#define LED8_TOGGLE()                (PORTB ^= LED8_BIT)

#define LED7_BIT                     (1 << 7)
#define LED7_CONFIG()                (DDRD |= LED7_BIT)
#define LED7_ON()                    (PORTD |= LED7_BIT)
#define LED7_OFF()                   (PORTD &= ~LED7_BIT)
#define LED7_READ()                  (PORTD & LED7_BIT)
#define LED7_TOGGLE()                (PORTD ^= LED7_BIT)

#define LED6_BIT                     (1 << 6)
#define LED6_CONFIG()                (DDRD |= LED6_BIT)
#define LED6_ON()                    (PORTD |= LED6_BIT)
#define LED6_OFF()                   (PORTD &= ~LED6_BIT)
#define LED6_READ()                  (PORTD & LED6_BIT)
#define LED6_TOGGLE()                (PORTD ^= LED6_BIT)

#define LED5_BIT                     (1 << 5)
#define LED5_CONFIG()                (DDRD |= LED5_BIT)
#define LED5_ON()                    (PORTD |= LED5_BIT)
#define LED5_OFF()                   (PORTD &= ~LED5_BIT)
#define LED5_READ()                  (PORTD & LED5_BIT)
#define LED5_TOGGLE()                (PORTD ^= LED5_BIT)

#define LED3_BIT                     (1 << 3)
#define LED3_CONFIG()                (DDRD |= LED3_BIT)
#define LED3_ON()                    (PORTD |= LED3_BIT)
#define LED3_OFF()                   (PORTD &= ~LED3_BIT)
#define LED3_READ()                  (PORTD & LED3_BIT)
#define LED3_TOGGLE()                (PORTD ^= LED3_BIT)

#define LED2_BIT                     (1 << 2)
#define LED2_CONFIG()                (DDRC |= LED2_BIT)
#define LED2_ON()                    (PORTC |= LED2_BIT)
#define LED2_OFF()                   (PORTC &= ~LED2_BIT)
#define LED2_READ()                  (PORTC & LED2_BIT)
#define LED2_TOGGLE()                (PORTC ^= LED2_BIT)

#define LED10_BIT                     (1 << 2)
#define LED10_CONFIG()                (DDRB |= LED10_BIT)
#define LED10_ON()                    (PORTB |= LED10_BIT)
#define LED10_OFF()                   (PORTB &= ~LED10_BIT)
#define LED10_READ()                  (PORTB & LED10_BIT)
#define LED10_TOGGLE()                (PORTB ^= LED10_BIT)

#define EAP_RX_ACK_BIT              (1 << 3)
#define EAP_RX_ACK_CONFIG()         (DDRC |= EAP_RX_ACK_BIT)
#define EAP_RX_ACK_SET()            (PORTC |= EAP_RX_ACK_BIT)
#define EAP_RX_ACK_CLR()            (PORTC &= ~EAP_RX_ACK_BIT)

#define EAP_TX_ACK_BIT              (1 << 2)
#define EAP_TX_ACK_CONFIG()         (DDRD &= ~EAP_TX_ACK_BIT, PORTD |= EAP_TX_ACK_BIT, EICRA |= 0x3) 

#define EAP_RX_BUF                  (UDR0)
#define EAP_TX_BUF                  (UDR0)

#define EAP_RX_INT_CLR()            (UCSR0A |= 1 << RXC0)
#define EAP_RX_INT_ENABLE()         (UCSR0B |= 1 << RXCIE0)
#define EAP_TX_INT_CLR()            (EIFR |= 0x1)
#define EAP_TX_INT_ENABLE()         (EIMSK |= 0x1)

#define TIMER_CONFIG()              TCCR1A = 0x00, TCCR1B = 0x00
#define TIMER_DIV1024               0x05
#define TIMER_MODE_CTC              0x08  

#define DEBUG12_BIT                  (1 << 3)
#define DEBUG12_CONFIG()             (DDRB |= DEBUG12_BIT)
#define DEBUG12_ON()                 (PORTB |= DEBUG12_BIT)
#define DEBUG12_OFF()                (PORTB &= ~DEBUG12_BIT)

#define DEBUG11_BIT                  (1 << 4)
#define DEBUG11_CONFIG()             (DDRB |= DEBUG11_BIT)
#define DEBUG11_ON()                 (PORTB |= DEBUG11_BIT)
#define DEBUG11_OFF()                (PORTB &= ~DEBUG11_BIT)

#define DEBUG9_BIT                  (1 << 1)
#define DEBUG9_CONFIG()             (DDRB |= DEBUG9_BIT)
#define DEBUG9_ON()                 (PORTB |= DEBUG9_BIT)
#define DEBUG9_OFF()                (PORTB &= ~DEBUG9_BIT)

#define DINT()                      cli()
#define EINT()                      sei()
#define SLEEP()                     sei()
#define WAKEUP()

#define NUM_HANDLERS 3

#define BUTTON_HANDLER_ID      0
#define TICK_HANDLER_ID        1
#define DISPATCH_HANDLER_ID    2

#define LEDSTATE(mask, n) (mask & (1 << n) ? NewProject_ON : NewProject_OFF)

typedef uint16_t SceneMask;

static void buttonHandler(void);
static void postEvent(uint8_t handlerId);

static Hal_Handler appButtonHandler;
static volatile uint16_t handlerEvents = 0;
static Hal_Handler handlerTab[NUM_HANDLERS];

/* -------- APP-HAL INTERFACE -------- */

void Hal_buttonEnable(Hal_Handler handler) {
}

void Hal_connected(void) {
    LED10_ON();
}

void Hal_disconnected(void) {
    LED10_OFF();
}

void Hal_debugOn(uint8_t line) {
    switch (line) {
    case 1:
        DEBUG12_ON();
        break;
    case 2:
        DEBUG11_ON();
        break;
    case 3:
    DEBUG9_ON();
    }
}

void Hal_debugOff(uint8_t line) {
    switch (line) {
    case 1:
        DEBUG12_OFF();
        break;
    case 2:
        DEBUG11_OFF();
        case 3:
    DEBUG9_OFF();
    }
}

void Hal_debugPulse(uint8_t line) {
    switch (line) {
    case 1:
        DEBUG12_ON();
        DEBUG12_OFF();
        break;
    case 2:
        DEBUG11_ON();
        DEBUG11_OFF();
      case 3:
        DEBUG9_ON();
        DEBUG9_OFF();
    }
}

void Hal_delay(uint16_t msecs) {
    while (msecs-- > 0) {
        uint16_t i;
            for (i = 0; i < 1000; i++) {
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
                asm("nop");
            }
        }
}



void Hal_init() {

        LED3_CONFIG();
        LED5_CONFIG();
        LED6_CONFIG();
        LED7_CONFIG();
        LED8_CONFIG();
        DEBUG9_CONFIG(); 
        LED10_CONFIG();
        DEBUG11_CONFIG(); 
        DEBUG12_CONFIG();
        LED13_CONFIG();
        LED14_CONFIG();
        LED15_CONFIG();
        LED2_CONFIG();
        LED4_CONFIG();
        EAP_TX_ACK_CONFIG();
        EAP_RX_ACK_CONFIG();
        EAP_RX_ACK_SET();
        
        TIMER_CONFIG();
        
        UBRR0H = (8 >> 8);
        UBRR0L = 8;
        UCSR0B |= (1 << RXEN0) | (1 << TXEN0);    // enable receiver and transmitter
        UCSR0C = ((1 << UCSZ00) | (1 << UCSZ01)); // Set 8 data bits, 1 stop bit, no parity
        
        handlerTab[DISPATCH_HANDLER_ID] = Em_Message_dispatch;
}

void Hal_idleLoop(void) {

    EINT();
    for (;;) {
        
        // atomically read/clear all handlerEvents
        DINT();
        uint16_t events = handlerEvents;
        handlerEvents = 0;

        if (events) {   // dispatch all current events
            EINT();
            uint16_t mask;
            uint8_t id;
            for (id = 0, mask = 0x1; id < NUM_HANDLERS; id++, mask <<= 1) {
                if ((events & mask) && handlerTab[id]) {
                    handlerTab[id]();
                }
            }
        }
        else {          // await more events
            SLEEP();
        }
    }
}

void Hal_User_ledOn(uint8_t ledNum) {
    
        switch(ledNum) {
        case 2:
            LED2_ON();
            break;
        case 3:
            LED3_ON();
            break;
        case 4:
            LED4_ON();
            break;
        case 5:
            LED5_ON();
            break;
        case 6:
            LED6_ON();
            break;
        case 7:
            LED7_ON();
            break;
        case 8:
            LED8_ON();
            break;
        case 9:
            LED9_ON();
            break;
        case 10:
            LED10_ON();
            break;
        case 11:
            LED11_ON();
            break;
        case 12:
            LED12_ON();
            break;
        case 13:
            LED13_ON();
            break;
        case 14:
            LED14_ON();
            break;
        case 15:
            LED15_ON();
            break;
    
    }
}
void Hal_User_ledOff(uint8_t ledNum)
{
    switch(ledNum)
    {
        
        case 2:
                LED2_OFF();
                break;
        case 3:
                LED3_OFF();
                break;
        case 4:
                LED4_OFF();
                break;
        case 5:
                LED5_OFF();
                break;
        case 6:
                LED6_OFF();
                break;
        case 7:
                LED7_OFF();
                break;
        case 8:
                LED8_OFF();
                break;
        case 9:
                LED9_OFF();
                break;
        case 10:
                LED10_OFF();
                break;
        case 11:
                LED11_OFF();
                break;
        case 12:
                LED12_OFF();
                break;
        case 13:
                LED13_OFF();
                break;
        case 14:
                LED14_OFF();
                break;
        case 15:
                LED15_OFF();
                break;
    }
}


void Hal_User_ledToggle(uint8_t ledNum)
{
    switch(ledNum)
    {
        case 2:
            LED2_TOGGLE();
            break;
        case 3:
            LED3_TOGGLE();
            break;
        case 4:
            LED4_TOGGLE();
            break;
        case 5:
            LED5_TOGGLE();
            break;
        case 6:
            LED6_TOGGLE();
            break;
        case 7:
            LED7_TOGGLE();
            break;
        case 8:
            LED8_TOGGLE();
            break;
        case 9:
            LED9_TOGGLE();
            break;
        case 10:
            LED10_TOGGLE();
            break;
        case 11:
            LED11_TOGGLE();
            break;
        case 12:
            LED12_TOGGLE();
            break;
        case 13:
            LED13_TOGGLE();
            break;
        case 14:
            LED14_TOGGLE();
            break;
        case 15:
            LED15_TOGGLE();
            break;
    }
}
void Hal_ledOff(void) {
    LED13_OFF();
}

bool Hal_User_ledRead(uint16_t ledNum)
{
    switch(ledNum)
    {
   case 10:
    return LED10_READ() ? true : false;
    case 9:
     return LED9_READ() ? true : false;
    break;
   case 8:
      return LED8_READ() ? true : false;
    break;
    case 7:
     return LED7_READ() ? true : false;
    break;
    case 5:
     return LED5_READ() ? true : false;
    break;
    case 6:
     return LED6_READ() ? true : false;;
    break;
     case 13:
      return LED13_READ() ? true : false;
     break;
     case 11:
      return LED11_READ() ? true : false;
     break;
     case 12:
      return LED12_READ() ? true : false;
     break;
     case 14:
      return LED14_READ() ? true : false;
     break;
     case 15:
      return LED15_READ() ? true : false;
     break;
    }
}


bool Hal_ledRead(void) {
    return LED13_READ() ? true : false;
}

void Hal_ledToggle(void) {
    LED13_TOGGLE();
}

void Hal_tickStart(uint16_t msecs, void (*handler)(void)) {
    handlerTab[TICK_HANDLER_ID] = handler;
    OCR1AH = (msecs >> 4);                      // HI(msecs * 16)
    OCR1AL = (msecs << 4);                      // LO(msecs * 16)
    TCNT1 = 0;                                  // clear counter
    TIFR1 &= ~(1 << OCF1A);                     // clear interrupt
    TIMSK1 |= (1 << OCIE1A);                    // enable interrupt
    TCCR1B = TIMER_MODE_CTC | TIMER_DIV1024;    // start timer
}

void Hal_User_eepromRead(uint16_t addr, void* output, uint16_t length) {
    uint8_t* src; 
    uint8_t* dst;
    src = (uint8_t*)addr;
    dst = (uint8_t*)output;
    for (uint16_t i = 0; i < length; i++) {
        *dst++ = eeprom_read_byte(src++);
    }
}

void Hal_User_eepromWrite(uint16_t addr, void* input, uint16_t length) {
    uint8_t* src; 
    uint8_t* dst;
    src = (uint8_t*)input;
    dst = (uint8_t*)addr;
    for (uint16_t i = 0; i < length; i++) {
        eeprom_write_byte(dst++, *src++);
    }
}
/* -------- SRT-HAL INTERFACE -------- */

uint8_t Em_Hal_lock(void) {
    uint8_t key = SREG & (1 << 7);
    cli();
    return key;
}

void Em_Hal_reset(void) {
    uint8_t key = Em_Hal_lock();
    EAP_RX_ACK_CLR();    // suspend the MCM
    Hal_delay(100);
    EAP_RX_ACK_SET();    // reset the MCM
    Hal_delay(500);
    EAP_RX_INT_CLR();
    EAP_TX_INT_CLR();
    EAP_RX_INT_ENABLE();
    EAP_TX_INT_ENABLE();
    Em_Hal_unlock(key);
}

void Em_Hal_startSend(void) {
    EAP_TX_BUF = Em_Message_startTx();
}

void Em_Hal_unlock(uint8_t key) {
    if (key) {
        sei();
    }
    else { 
        cli();
    }
}

void Em_Hal_watchOn(void) {
}

void Em_Hal_watchOff(void) {
}

/* -------- INTERNAL FUNCTIONS -------- */

static void buttonHandler(void) {
    if (appButtonHandler) {
        appButtonHandler();
    }
}

static void postEvent(uint8_t handlerId) {
    uint8_t key = Em_Hal_lock();
    handlerEvents |= 1 << handlerId;
    Em_Hal_unlock(key);
}

/* -------- INTERRUPT SERVICE ROUTINES -------- */

/* Hal_rxIsr */
ISR(USART_RX_vect) {
    Em_Message_startRx();
    uint8_t b = EAP_RX_BUF;
    if (Em_Message_addByte(b)) {
        postEvent(DISPATCH_HANDLER_ID);
    }
    EAP_RX_ACK_CLR();
    EAP_RX_ACK_SET();
}

/* Hal_txAckIsr */
ISR(INT0_vect) {
    uint8_t b;
    if (Em_Message_getByte(&b)) {
        EAP_TX_BUF = b;
    }
    EAP_TX_INT_CLR();
}

/* Hal_timerIsr */
ISR(TIMER1_COMPA_vect) {
    postEvent(TICK_HANDLER_ID);
}
