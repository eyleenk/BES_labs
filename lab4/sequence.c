#include "sequence.h"

void WaitForSWLow(void) {
    while ((GPIO_PORTF_DATA_R & 0x10) != 0) { //portF data r näitab pinnide hetkeolekut
    } 
}
//kui pin 4 on 1, siis on nupp alla vajutamata, kui 0, siis on nupp alla vajutatud, tsüklit korratakse kuni väärtus 0 ehk nupp on alla vajutatud
//iga bitt vastab ühele pin'ile
// 0x10 = 0001 0000, see jätab alles ainult 4nda biti (mis 1) ja nullib kõik teised, seal on SW1 nupp

void WaitForSWHigh(void) {
    while ((GPIO_PORTF_DATA_R & 0x10) == 0) { //vastupidine; kui nupp lastakse lahti siis PF4=1 ja tsükkel lõpeb
    }
}

uint32_t SetRed(uint32_t portf_data_reg) {
    return portf_data_reg | 0x02; // Set bit 1 to turn on red LED
}

uint32_t ClearRed(uint32_t portf_data_reg) {
    return portf_data_reg & ~0x02; // Clear bit 1 to turn off red LED; bitt 1 to 0 
}

uint32_t SetGreen(uint32_t portf_data_reg) {
    return portf_data_reg | 0x08; // Set bit 3 on (to 1) to turn on green LED
}

uint32_t ClearGreen(uint32_t portf_data_reg) {
    return portf_data_reg & ~0x08; // Clear bit 3 (to 0) to turn off green LED
}
/*bitt:  7  6  5  4    3    2    1    0
         -  -  - PF4  PF3  PF2  PF1  PF0
                 SW1  ROH       PUN   
                    
 ehk mask 0x02= pF1 e punane LED, biti 1 mask
 ehk mask 0x08= pF3 e roheline LED, biti 3 mask
 ehk mask 0x10= pF4 e SW1 nupp, biti 4 mask

reg | mask seab maski bitid väärtuseks 1, teised jäävad samaks.
reg & ~mask seab maski bitid väärtuseks 0, teised jäävad samaks.
 */