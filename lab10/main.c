/*
    Lab10 - DAC

    Navigate to the dac.c/piano.c/sound.c to finish functions.
    Finish the main function.
*/

#include "verify.h"
#include "sound.h"
#include "piano.h"
#include "dac.h"

#define DELAY_20MS 32000

void Delay20ms(void);

int main(void) {
    SoundInit();
    PianoInit();
    DACInit();
    __asm__("CPSIE I"); //lubaba SYstick inturruptid
    BESGrader();

    uint32_t input; // loeb PE0 kuni PE3 portide olekud
    uint32_t note; // valib perioodi vastavalt vajutatud klahvile

    while (true) {
        input = PianoInput();     // loeb PE0 kuni PE3 portide olekud
        note = ChooseNote(input); // valib noodi vastavalt vajutatud klahvile
        SoundPlay(note);          // mängib noodi
        Delay20ms();              // ootab 20 ms nupu võbisemise eemaldamiseks
    }
}

/*
    \brief Subroutine to delay 20 milliseconds
    \param None
    \return None
    \note Assumes 16 MHz clock
*/
void Delay20ms(void) {
    for (volatile uint32_t i = DELAY_20MS; i > 0; i--) {}
}