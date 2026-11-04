#ifndef FSM_H
#define FSM_H

#include <stdint.h>

#define NEXT_SIZE 4 // The amount of possible next states

typedef struct FSMStateData {
    uint32_t portb_out;         // PB0-2 car lights
    uint32_t portf_out;         // PF3 and PF1 pedestrian lights
    uint32_t times;             // X times 100ms delay
    uint32_t next[NEXT_SIZE];   // Array of next possible states
} FSMStateData_t;

// State must be 1:1 with the same state index in fsm array

//kuna juhises, et 3x peab roheline käima kustu ja 2 korda sisse, siis kokku 10 olekut:
typedef enum FSMState {
    goCar = 0,      // kui audel roheline, siis jalakäijatel punane
    waitCar,        // kui audel kollane, siis jalakäijatel punane
    stopCar,        // kui audel punane, siis jalakäijatel punane
    goPed,          // kui audel punane, siis jalakäijatel roheline
    offPed1,         // vilkumine; roheline kustu 1
    onPed1,          // vilkumine; roheline käima 1
    offPed2,        // vilkumine; roheline kustu 2
    onPed2,         // vilkumine; roheline käima 2
    offPed3,        // vilkumine; roheline kustu 3
    stopPed,        // kui audel punane, siis jalakäijatel punane
    Count   // all enums count
} FSMState_t;

/*Sisendid (PE1, PE0):
 0 (00) - keegi ei oota
 1 (01) - auto ootab
 2 (10) - jalakäija ootab
 3 (11) - mõlemad ootavad
 */

#endif /* FSM_H */
