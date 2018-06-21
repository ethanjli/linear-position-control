#ifndef LPC_Components_FastInterrupts_h
#define LPC_Components_FastInterrupts_h

#if defined(LPC_Components_FastInterrupts_All) || defined (LPC_Components_FastInterrupts_Pin0)
#define INTERRUPT_FLAG_PIN0 LinearPositionControl::Components::fastInterruptCounter0
#define NEEDFORSPEED
#endif
#if defined(LPC_Components_FastInterrupts_All) || defined (LPC_Components_FastInterrupts_Pin1)
#define INTERRUPT_FLAG_PIN1 LinearPositionControl::Components::fastInterruptCounter1
#define NEEDFORSPEED
#endif
#if defined(LPC_Components_FastInterrupts_All) || defined (LPC_Components_FastInterrupts_Pin2)
#define INTERRUPT_FLAG_PIN2 LinearPositionControl::Components::fastInterruptCounter2
#define NEEDFORSPEED
#endif
#if defined(LPC_Components_FastInterrupts_All) || defined (LPC_Components_FastInterrupts_Pin3)
#define INTERRUPT_FLAG_PIN3 LinearPositionControl::Components::fastInterruptCounter3
#define NEEDFORSPEED
#endif
#if defined(LPC_Components_FastInterrupts_All) || defined (LPC_Components_FastInterrupts_Pin4)
#define INTERRUPT_FLAG_PIN4 LinearPositionControl::Components::fastInterruptCounter4
#define NEEDFORSPEED
#endif
#if defined(LPC_Components_FastInterrupts_All) || defined (LPC_Components_FastInterrupts_Pin5)
#define INTERRUPT_FLAG_PIN5 LinearPositionControl::Components::fastInterruptCounter5
#define NEEDFORSPEED
#endif
#if defined(LPC_Components_FastInterrupts_All) || defined (LPC_Components_FastInterrupts_Pin6)
#define INTERRUPT_FLAG_PIN6 LinearPositionControl::Components::fastInterruptCounter6
#define NEEDFORSPEED
#endif
#if defined(LPC_Components_FastInterrupts_All) || defined (LPC_Components_FastInterrupts_Pin7)
#define INTERRUPT_FLAG_PIN7 LinearPositionControl::Components::fastInterruptCounter7
#define NEEDFORSPEED
#endif
#if defined(LPC_Components_FastInterrupts_All) || defined (LPC_Components_FastInterrupts_Pin8)
#define INTERRUPT_FLAG_PIN8 LinearPositionControl::Components::fastInterruptCounter8
#define NEEDFORSPEED
#endif
#if defined(LPC_Components_FastInterrupts_All) || defined (LPC_Components_FastInterrupts_Pin9)
#define INTERRUPT_FLAG_PIN9 LinearPositionControl::Components::fastInterruptCounter9
#define NEEDFORSPEED
#endif
#if defined(LPC_Components_FastInterrupts_All) || defined (LPC_Components_FastInterrupts_Pin10)
#define INTERRUPT_FLAG_PIN10 LinearPositionControl::Components::fastInterruptCounter10
#define NEEDFORSPEED
#endif
#if defined(LPC_Components_FastInterrupts_All) || defined (LPC_Components_FastInterrupts_Pin11)
#define INTERRUPT_FLAG_PIN11 LinearPositionControl::Components::fastInterruptCounter11
#define NEEDFORSPEED
#endif
#if defined(LPC_Components_FastInterrupts_All) || defined (LPC_Components_FastInterrupts_Pin12)
#define INTERRUPT_FLAG_PIN12 LinearPositionControl::Components::fastInterruptCounter12
#define NEEDFORSPEED
#endif

#include <EnableInterrupt.h>

namespace LinearPositionControl { namespace Components {

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

} }

#endif

