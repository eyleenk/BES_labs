#include "led_interface.h" //fn, mis otsustab milline väärtus kirjutada PE1'le

uint32_t SetOrToggleLED(uint32_t sw, uint32_t led_val) { //tagastab 32-bitise pos. täisarvu, võtab sisendiks
    //sw - Pe0 sisendi ehk nupu olek
    //led_val - Pe1 sisendi ehk LED-i olek
    if (sw) {
        if ((led_val & 0x02) != 0) { //kui Led on sees ehk 0x02 bitimask ehk Pe1 väärtus 
            //POLE 0, siis LED=põleb; lülitab välja
            return 0;
        } else {
            return 0x02; //kui PE1 bitt = 0, siis LED = väljas; lülitab sisse
        }
    }
    return 0x02; //0x02 on Pe1 ehk kui nuppu POLE ÜLDSE VAJUTATUD peab led põlema
}