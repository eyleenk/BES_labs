#include "led_control.h"

uint32_t SetOrToggleLED(uint32_t sw, uint32_t led_val) {

    // Kui PF4 = 0, siis nupp on vajutatud
    if((sw & 0x10) == 0){
        led_val ^= 0x04;   // Muuda LED olekut (ON <-> OFF)
    }
    else{
        led_val |= 0x04;   // Hoia LED põlemas
    }
    return led_val;
}

// PF4 = 0x10 (SW1 nupp)
// PF2 = 0x04 (sinine LED)

// Nupu loogika:
// PF4 = 1 -> nupp vabastatud
// PF4 = 0 -> nupp vajutatud

// LED loogika:
// PF2 = 1 -> LED põleb
// PF2 = 0 -> LED kustunud
// Kui nupp on vajutatud, LED vahetab olekut.
// Kui nupp ei ole vajutatud, LED jääb põlema.