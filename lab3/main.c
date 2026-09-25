#include "led_control.h"
#include "verify.h"

#define GPIO_PORTF_DATA_R       (*((volatile uint32_t *)0x400253FC))
#define GPIO_PORTF_DIR_R        (*((volatile uint32_t *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile uint32_t *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile uint32_t *)0x40025510))
#define GPIO_PORTF_DEN_R        (*((volatile uint32_t *)0x4002551C))
#define GPIO_PORTF_AMSEL_R      (*((volatile uint32_t *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile uint32_t *)0x4002552C))
#define SYSCTL_RCGC2_R          (*((volatile uint32_t *)0x400FE108))
#define SYSCTL_RCGC2_GPIOF      0x00000020  // Port F Clock Gating Control

void PortFInit(void);
void Delay100ms(uint32_t times);

int main(void){
    PortFInit(); // Student submitted subroutine
    BESGrader();
    uint32_t sw1;  // input from PF4
    uint32_t out;  // output for PF2
    while (true) {
        sw1 = GPIO_PORTF_DATA_R; //loe PF4 nupu olek
        Delay100ms(1); // oota 100ms
        out= SetOrToggleLED(sw1, GPIO_PORTF_DATA_R & 0x04); //kontrollib nupu ja LED olekut
        GPIO_PORTF_DATA_R = out; //kirjuta tulemus tagasi andmeregistrisse
    }
}

void PortFInit(void) {
    volatile uint32_t delay;

    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF; // Lülita sisse Port F kell
    delay = SYSCTL_RCGC2_R;               // Oota kella käivitumist
    GPIO_PORTF_AMSEL_R &= ~0x14;          // Keela analoog PF4 ja PF2
    GPIO_PORTF_PCTL_R &= ~0x000F0F00;     // Määra PF4 ja PF2 GPIO-ks
    GPIO_PORTF_DIR_R &= ~0x10;            // PF4 sisend
    GPIO_PORTF_DIR_R |= 0x04;             // PF2 väljund
    GPIO_PORTF_AFSEL_R &= ~0x14;          // Keela alternatiivfunktsioonid
    GPIO_PORTF_PUR_R |= 0x10;             // Pull-up takisti PF4-le
    GPIO_PORTF_DEN_R |= 0x14;             // Luba digitaalne I/O
    GPIO_PORTF_DATA_R |= 0x04;            // LED alguses sisse
}
//PF4= input = 0x10 (nupp)
//PF2= output = 0x04 (sinine LED)

#define DELAY_100MS 160000 // ~100ms

void Delay100ms(uint32_t times) {
    for (; times > 0; times--) {
        for (volatile uint32_t i = DELAY_100MS; i > 0; i--) {}
    }
}