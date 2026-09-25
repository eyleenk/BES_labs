#include "verify.h"
#include "sequence.h"

void PortFInit(void);
void Delay1ms(uint32_t msec);

int main(void) {
    PortFInit();        // Initialize Port F
    BESGrader();        // Käivitab graderi
    uint32_t out;       // output for PF3 and PF1

    while (true) {
        out = SetGreen(GPIO_PORTF_DATA_R);   // liumägi on vaba: loe register, sea roheline tuli (PF3 väärtus = 1)
        GPIO_PORTF_DATA_R = out;             // kirjuta tagasi - roheline põleb
        WaitForSWLow();                      //Oota, kuni inimene hakkab liugu laskma (SW1 vajutatud, PF4 e switch väärtus = 0)
        out = ClearGreen(GPIO_PORTF_DATA_R); // nulli bitt 3 (PF3 = 0) e roheline = kustu
        out = SetRed(out);                   // sea bitt 1 (PF1 = 1) e punane = põlema
        GPIO_PORTF_DATA_R = out;             // kirjuta tagasi - punane põleb
        Delay1ms(20);                        // oota 20 ms, et nupu põrkumine vaibuks
        WaitForSWHigh();                     // Oota, kuni inimene lahkub liumäelt (switch lahti, PF4 e switch väärtus = 1)
        Delay1ms(500);                       // Oota 500 ms (punane põleb edasi)
        out = SetGreen(GPIO_PORTF_DATA_R);   // sea bitt 3 e roheline, bitt 1 e punane jääb alles
        GPIO_PORTF_DATA_R = out;             // PF3 e roheline = 1, PF1 e punane  = 1 koos põlevad - põleb kollane
        Delay1ms(500);                       // Kollane põleb 500 ms
                                             //Punane kustu - jääb ainult roheline, tsükkel algab otsast
        out = ClearRed(GPIO_PORTF_DATA_R);   // nulli bitt 1 e punane (PF1 = 0)
        GPIO_PORTF_DATA_R = out;             // roheline põleb jne
        // mask 0x02= pF1 e punane LED, biti 1 mask
        // mask 0x08= pF3 e roheline LED, biti 3 mask
        // mask 0x10= pF4 e SW1 nupp, biti 4 mask
    }
}

void PortFInit(void) { 
    volatile uint32_t delay;
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;   // Turn on the clock for Port F
    delay = SYSCTL_RCGC2_R;                 // Delay to allow clock to stabilize     
    GPIO_PORTF_AMSEL_R &= 0x00;             // Disable analog function
    GPIO_PORTF_PCTL_R &= 0x00000000;        // GPIO clear bit PCTL  
    GPIO_PORTF_DIR_R &= ~0x10;              // PF4 input (bitt 4 -> 0)
    GPIO_PORTF_DIR_R |= 0x0A;               // PF3, PF1 output (bitid 3 ja 1 -> 1)
    GPIO_PORTF_AFSEL_R &= 0x00;             // No alternate function
    GPIO_PORTF_PUR_R |= 0x10;               // Enable pullup resistor on PF4
                                            // (nupp lahti -> PF4 = 1, vajutatud -> 0)
    GPIO_PORTF_DEN_R |= 0x1A;               // Enable digital pins PF4, PF3, PF1
}

/*
    Arvutus: 16 MHz / ~10 tsüklit iteratsiooni kohta = 1 600 000 iteratsiooni/s (protsessori tsüklid sekundis)
    -> 1 ms = 1600 iteratsiooni. Algväärtus, täpsusta loogikaanalüsaatoriga:
    uus = 1600 * 500 / mõõdetud_kollase_faasi_ms
*/
#define DELAY_1MS 1600 // Enter value that is close to ~0.9-1ms 

/*
    \brief Subroutine to delay in units of milliseconds
    \param msec Number of milliseconds to delay
    \return None
    \note Uses internal 16 MHz oscillator
*/
void Delay1ms(uint32_t msec) {
    for (; msec > 0; msec--) {                              // korda msec korda
        for (volatile uint32_t i = DELAY_1MS; i > 0; i--) {} // ~1 ms tühi tsükkel
    }                                                       // volatile: kompilaator
}                                                           // ei optimeeri tsüklit ära