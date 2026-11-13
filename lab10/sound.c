#include "sound.h"
#include "dac.h"

const uint32_t sinewave[] = {
    7, 8, 9, 10, 11, 12, 13, 14,
    15, 15, 15, 14, 13, 12, 11, 10,
    9, 8, 7, 6, 5, 4, 3, 2,
    1, 1, 1, 2, 3, 4, 5, 6 //sämplid (0-15 = 16)
};
uint32_t i = 0;

void SoundInit(void) { // kindlas järjekorras, et vältida enneaegseid katkestusi ja tagada, et loendur samal ajal ei tiksuks
    NVIC_ST_CTRL_R = 0; // lülitab SysTick välja (seadistamise ajaks)
    NVIC_ST_RELOAD_R = 0; // määrab taimeri perioodi väärtuseks 0
    NVIC_ST_CURRENT_R = 0; // nullib loenduri
    NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x1FFFFFFF) | 0x20000000; // prioriteedi seadistamien
    NVIC_ST_CTRL_R = 0x07; // lülitab SysTick sisse, lubab taimeri ja inturruptid, kasutab core clocki
}

void SoundPlay(uint32_t period) {
    if (period <= 1) { // kui periood on liiga lühike (väiksem, kui 1), siis heli ei mängi (dac out 0)
        NVIC_ST_RELOAD_R = 0;
        DACOut(0); 
    } else {           //muul juhul mängib heli (dac out 0-15)
        if (NVIC_ST_RELOAD_R != (period - 1)) { //kui periood erinev eelmisest perioodist, siis seadistab uue perioodi (nupp on vajutatud)
            NVIC_ST_RELOAD_R = period - 1; 

            // periood = 16 000 000 / sagedus (Hz) * 32 (sämplite arv) - tulemus takti/sammu kohta
            //N. C noodi puhul = 16 000 000 / 523.251 * 32 = 978 000 (periood)
        }
    }
}

void SysTickHandler(void) {
    if (NVIC_ST_RELOAD_R > 0) { // kui periood on suurem kui 0, siis heli aktiivne
        DACOut(sinewave[i]);    
        i = (i + 1) & 0x1F;     // käib läbi sinewave massiivi (0-31, kuna igas reas 8 elementi) ja saadab väärtused DACile
    }
}

uint32_t ChooseNote(uint32_t input) {
    switch (input) {
        case 0x01: return 956; // kui PE0 on vajutatud, siis mängib C (sagedus 523.251 Hz)
        case 0x02: return 851; // kui PE1 on vajutatud, siis mängib D (sagedus 587.33 Hz)
        case 0x04: return 758; // kui PE2 on vajutatud, siis mängib E (sagedus 659.255 Hz)
        case 0x08: return 638; // kui PE3 on vajutatud, siis mängib G (sagedus 783.991 Hz)
        default:   return 1;   // kui ühtegi klahvi ei vajutata või vajutatakse mitut korraga, siis heli ei mängi (tagastab 1)
    }
}