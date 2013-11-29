/*
 * ============ Hardware Abstraction Layer for Arduino Uno ============
 */

#include "Hal.h"
#include "Em_Message.h"

#include "avr/interrupt.h"
#include "avr/io.h"

/* -------- INTERNAL FEATURES -------- */

#define LED_BIT                     (1 << 5)
#define LED_CONFIG()                (DDRB |= LED_BIT)
#define LED_ON()                    (PORTB |= LED_BIT)
#define LED_OFF()                   (PORTB &= ~LED_BIT)
#define LED_READ()                  (PORTB & LED_BIT)
#define LED_TOGGLE()                (PORTB ^= LED_BIT)

#define CONNECTED_BIT               (1 << 2)
#define CONNECTED_CONFIG()          (DDRB |= CONNECTED_BIT)
#define CONNECTED_ON()              (PORTB |= CONNECTED_BIT)
#define CONNECTED_OFF()             (PORTB &= ~CONNECTED_BIT)

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

#define DEBUG1_BIT                  (1 << 3)
#define DEBUG1_CONFIG()             (DDRB |= DEBUG1_BIT)
#define DEBUG1_ON()                 (PORTB |= DEBUG1_BIT)
#define DEBUG1_OFF()                (PORTB &= ~DEBUG1_BIT)

#define DEBUG2_BIT                  (1 << 4)
#define DEBUG2_CONFIG()             (DDRB |= DEBUG2_BIT)
#define DEBUG2_ON()                 (PORTB |= DEBUG2_BIT)
#define DEBUG2_OFF()                (PORTB &= ~DEBUG2_BIT)

#define DINT()                      cli()
#define EINT()                      sei()
#define SLEEP()                     sei()
#define WAKEUP()

#define NUM_HANDLERS 3

#define BUTTON_HANDLER_ID      0
#define TICK_HANDLER_ID        1
#define DISPATCH_HANDLER_ID    2

static void buttonHandler(void);
static void postEvent(uint8_t handlerId);

static Hal_Handler appButtonHandler;
static volatile uint16_t handlerEvents = 0;
static Hal_Handler handlerTab[NUM_HANDLERS];

/* -------- APP-HAL INTERFACE -------- */

void Hal_buttonEnable(Hal_Handler handler) {
}

void Hal_connected(void) {
    CONNECTED_ON();
}

void Hal_debugOn(uint8_t line) {
    switch (line) {
    case 1:
        DEBUG1_ON();
        break;
    case 2:
        DEBUG2_ON();
    }
}

void Hal_debugOff(uint8_t line) {
    switch (line) {
    case 1:
        DEBUG1_OFF();
        break;
    case 2:
        DEBUG2_OFF();
    }
}

void Hal_debugPulse(uint8_t line) {
    switch (line) {
    case 1:
        DEBUG1_ON();
        DEBUG1_OFF();
        break;
    case 2:
        DEBUG2_ON();
        DEBUG2_OFF();
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

void Hal_disconnected(void) {
    CONNECTED_OFF();
}

void Hal_init() {

    DEBUG1_CONFIG(); DEBUG1_OFF();
    DEBUG2_CONFIG(); DEBUG2_OFF();

    DEBUG1_ON(); DEBUG1_OFF();

    LED_CONFIG();
    LED_OFF();

    CONNECTED_CONFIG();
    CONNECTED_OFF();

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

void Hal_ledOn(void) {
    LED_ON();
}

void Hal_ledOff(void) {
    LED_OFF();
}

bool Hal_ledRead(void) {
    return LED_READ() ? true : false;
}

void Hal_ledToggle(void) {
    LED_TOGGLE();
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

