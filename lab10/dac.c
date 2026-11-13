#include "dac.h"
#include "tm4c123gh6pm.h"

void DACInit(void) { //OUTPUT - D/A muundur - viigud PB0 kuni PB3
    volatile uint32_t delay;
    SYSCTL_RCGC2_R |= 0x02; // activeerib portB 
    delay = SYSCTL_RCGC2_R; // delay et stabiliseeruks
    (void)delay;
    GPIO_PORTB_AMSEL_R &= ~0x0F; // keelab analoogi
    GPIO_PORTB_PCTL_R &= ~0x0000FFFF; // ainult GPIO funktsioonid aktiivsed
    GPIO_PORTB_DIR_R |= 0x0F; // määrab PB0, PB1, PB2, PB3 väljunditeks
    GPIO_PORTB_AFSEL_R &= ~0x0F; // keelab alternatiivsed funktsioonid
    GPIO_PORTB_DR8R_R |= 0x0F; // lubab 8 mA voolu PB0 kuni PB3 portidel
    GPIO_PORTB_DEN_R |= 0x0F; // lubab digitaalset sisendit/väljundit PB0 kuni PB3 portidel
    GPIO_PORTB_DATA_R &= ~0x0F; // algväärtustab kõik PB0 kuni PB3 pordid 0-ks
}

void DACOut(uint32_t data) {
	GPIO_PORTB_DATA_R = (GPIO_PORTB_DATA_R & ~0x0F) | (data & 0x0F); //kirjutab väärtused PB0 kuni PB3 portidesse
}