#include "piano.h"

void PianoInit(void) { //INPUT - klaviatuur - viigud PE0 kuni PE3
    volatile uint32_t delay;
    SYSCTL_RCGC2_R |= 0x10; // activeerib port E
    delay = SYSCTL_RCGC2_R; // delay et stabiliseeruks
    GPIO_PORTE_AMSEL_R &= ~0x0F; // keelab analoogi
    GPIO_PORTE_PCTL_R &= ~0x0000FFFF; // lubaba inult GPIO funktsioonid
    GPIO_PORTE_AFSEL_R &= ~0x0F; // keelab alternatiivsed funktsioonid
    GPIO_PORTE_DIR_R &= ~0x0F; // määrab PE0, PE1, PE2, PE3 sisenditeks
    GPIO_PORTE_DEN_R |= 0x0F; // lubab digitaalset sisendit/väljundit PE0, PE1, PE2, PE3 portidel
    GPIO_PORTE_DATA_R &= ~0x0F; // algväärtab kõik PE0 kuni PE3 pordid 0-ks
}

uint32_t PianoInput(void) {
    return GPIO_PORTE_DATA_R & 0x0F;// loeb PE0 kuni PE3 portide olekud
}
