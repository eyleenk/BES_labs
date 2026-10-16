#include "bits.h"

uint32_t ToggledBits(uint32_t data_reg, uint32_t before_toggle) { //fn peab tagastama bitid, mis muutusid kahe lugemise vahel
    return ((data_reg ^ before_toggle) & 0x13) != 0; //XOR leiab erinevused ainult muutunud bittide jaoks (ehk 0, 1 ja 4)
    //bitimask 0x13 = BIN 0001 0011  ehk PF0, PF1 ja PF4
    //bit 0 = PF0 ehk SW2 nupp
    //bit 1 = PF1 ehk punane LED
    //bit 4 = PF4 ehk SW1 nupp
}