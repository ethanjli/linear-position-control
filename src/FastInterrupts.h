#ifndef Interrupts_h
#define Interrupts_h

#if defined(ALL_SENSOR_PINS) || defined (SENSOR_PIN0)
#define INTERRUPT_FLAG_PIN0 fastInterruptCounter0
#define NEEDFORSPEED
#endif
#if defined(ALL_SENSOR_PINS) || defined (SENSOR_PIN1)
#define INTERRUPT_FLAG_PIN1 fastInterruptCounter1
#define NEEDFORSPEED
#endif
#if defined(ALL_SENSOR_PINS) || defined (SENSOR_PIN2)
#define INTERRUPT_FLAG_PIN2 fastInterruptCounter2
#define NEEDFORSPEED
#endif
#if defined(ALL_SENSOR_PINS) || defined (SENSOR_PIN3)
#define INTERRUPT_FLAG_PIN3 fastInterruptCounter3
#define NEEDFORSPEED
#endif
#if defined(ALL_SENSOR_PINS) || defined (SENSOR_PIN4)
#define INTERRUPT_FLAG_PIN4 fastInterruptCounter4
#define NEEDFORSPEED
#endif
#if defined(ALL_SENSOR_PINS) || defined (SENSOR_PIN5)
#define INTERRUPT_FLAG_PIN5 fastInterruptCounter5
#define NEEDFORSPEED
#endif
#if defined(ALL_SENSOR_PINS) || defined (SENSOR_PIN6)
#define INTERRUPT_FLAG_PIN6 fastInterruptCounter6
#define NEEDFORSPEED
#endif
#if defined(ALL_SENSOR_PINS) || defined (SENSOR_PIN7)
#define INTERRUPT_FLAG_PIN7 fastInterruptCounter7
#define NEEDFORSPEED
#endif
#if defined(ALL_SENSOR_PINS) || defined (SENSOR_PIN8)
#define INTERRUPT_FLAG_PIN8 fastInterruptCounter8
#define NEEDFORSPEED
#endif
#if defined(ALL_SENSOR_PINS) || defined (SENSOR_PIN9)
#define INTERRUPT_FLAG_PIN9 fastInterruptCounter9
#define NEEDFORSPEED
#endif
#if defined(ALL_SENSOR_PINS) || defined (SENSOR_PIN10)
#define INTERRUPT_FLAG_PIN10 fastInterruptCounter10
#define NEEDFORSPEED
#endif
#if defined(ALL_SENSOR_PINS) || defined (SENSOR_PIN11)
#define INTERRUPT_FLAG_PIN11 fastInterruptCounter11
#define NEEDFORSPEED
#endif
#if defined(ALL_SENSOR_PINS) || defined (SENSOR_PIN12)
#define INTERRUPT_FLAG_PIN12 fastInterruptCounter12
#define NEEDFORSPEED
#endif

#include <EnableInterrupt.h>

namespace LinearPositionControl {

volatile uint8_t &getInterruptCounter(uint8_t pin) {
    // This is a very ugly implementation, but it's a consequence of EnableInterrupt
    // choosing to declare the counters instead of letting us choose references
    // (e.g. array members) for the counters.
    switch (pin) {
#ifdef INTERRUPT_FLAG_PIN0
        case 0:
            return INTERRUPT_FLAG_PIN0;
#endif
#ifdef INTERRUPT_FLAG_PIN1
        case 1:
            return INTERRUPT_FLAG_PIN1;
#endif
#ifdef INTERRUPT_FLAG_PIN2
        case 2:
            return INTERRUPT_FLAG_PIN2;
#endif
#ifdef INTERRUPT_FLAG_PIN3
        case 3:
            return INTERRUPT_FLAG_PIN3;
#endif
#ifdef INTERRUPT_FLAG_PIN4
        case 4:
            return INTERRUPT_FLAG_PIN4;
#endif
#ifdef INTERRUPT_FLAG_PIN5
        case 5:
            return INTERRUPT_FLAG_PIN5;
#endif
#ifdef INTERRUPT_FLAG_PIN6
        case 6:
            return INTERRUPT_FLAG_PIN6;
#endif
#ifdef INTERRUPT_FLAG_PIN7
        case 7:
            return INTERRUPT_FLAG_PIN7;
#endif
#ifdef INTERRUPT_FLAG_PIN8
        case 8:
            return INTERRUPT_FLAG_PIN8;
#endif
#ifdef INTERRUPT_FLAG_PIN9
        case 9:
            return INTERRUPT_FLAG_PIN9;
#endif
#ifdef INTERRUPT_FLAG_PIN10
        case 10:
            return INTERRUPT_FLAG_PIN10;
#endif
#ifdef INTERRUPT_FLAG_PIN11
        case 11:
            return INTERRUPT_FLAG_PIN11;
#endif
#ifdef INTERRUPT_FLAG_PIN12
        case 12:
            return INTERRUPT_FLAG_PIN12;
#endif
    }
}

}

#endif

