/*
    Lab5 - Breadboard circuit building

    Navigate to the led_interface.c to finish the SetOrToggleLED() function.
    Finish main function and PortE initialization.
*/

#include "led_interface.h"
#include "verify.h"

#define PE0_MASK                 0x01 //bitimask - vastab PE0 pordi E-bitile; sisend ehk nupp
#define PE1_MASK                 0x02 //bitimask - vastab PE1 pordi E-bitile; väljund ehk LED
#define GPIO_PORTE_DATA_R       (*((volatile uint32_t *)0x400243FC))
#define GPIO_PORTE_DIR_R        (*((volatile uint32_t *)0x40024400))
#define GPIO_PORTE_AFSEL_R      (*((volatile uint32_t *)0x40024420))
#define GPIO_PORTE_PUR_R        (*((volatile uint32_t *)0x40024510))
#define GPIO_PORTE_DEN_R        (*((volatile uint32_t *)0x4002451C))
#define GPIO_PORTE_AMSEL_R      (*((volatile uint32_t *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile uint32_t *)0x4002452C))
#define SYSCTL_RCGC2_R          (*((volatile uint32_t *)0x400FE108))
#define SYSCTL_RCGC2_GPIOE      0x00000010  // Port E Clock Gating Control

void PortEInit(void);
void Delay100ms(uint32_t times);

int main(void){
    PortEInit();
    BESGrader();
    uint32_t sw;   // input from PE0
    uint32_t out;  // output for PE1
    while (true) {
        sw  = GPIO_PORTE_DATA_R & PE0_MASK;  // PE0=1 = nupp on vajutatud
        out = GPIO_PORTE_DATA_R & PE1_MASK;  // loe LED-i praegune olek PE1-l
        Delay100ms(1);                       // vähenda lüliti võbisemise mõju delay abil
        out = SetOrToggleLED(sw, out);       // kui nupp vajutatud: vaheta olekut; muidu jäta LED sisse
        GPIO_PORTE_DATA_R = (GPIO_PORTE_DATA_R & ~PE1_MASK) | out; // muuda ainult PE1
    }
}

/* 
    \brief Subroutine to initialize port E pins for input and output.
    PE0 is switch input.
    PE1 is output to the LED.

    \param None
    \return None
    \note Set the LED to be initially ON at the end of the initialization. Bit setting
    doesn't affect other bits.
*/
void PortEInit(void) {
    volatile uint32_t delay;
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;   // Turn on the clock for Port E
    delay = SYSCTL_RCGC2_R;                 // Allow time for the clock to start
    (void)delay;
    GPIO_PORTE_AMSEL_R &= ~(PE0_MASK | PE1_MASK); // Disable analog on PE1 and PE0 (AMSEL)
    GPIO_PORTE_PCTL_R  &= ~0x000000FF;      // Clear PE1 and PE0 bit fields in PCTL to configure them as GPIO
    GPIO_PORTE_DIR_R   |=  PE1_MASK;        // Configure PE1 as output
    GPIO_PORTE_DIR_R   &= ~PE0_MASK;        // Configure PE0 as input
    GPIO_PORTE_AFSEL_R &= ~(PE0_MASK | PE1_MASK); // Clear PE1 and PE0 bits in AFSEL to disable alternate functions
    GPIO_PORTE_PUR_R   &= ~(PE0_MASK | PE1_MASK); // Clear PE1 and PE0 bits in PUR; the external pull-down is used
    GPIO_PORTE_DEN_R   |=  (PE0_MASK | PE1_MASK); // Set PE1 and PE0 bits in DEN to enable digital I/O
    GPIO_PORTE_DATA_R  |=  PE1_MASK;        // Set PE1 so the LED is initially ON
}

#define DELAY_100MS 160000 // ~100ms LED'i vilkumissagedus

/*
    \brief Subroutine to delay 100 milliseconds N times
    \param times Number of times to delay 100 ms
    \return None
    \note Assumes 16 MHz clock
*/
void Delay100ms(uint32_t times) {
    for (; times > 0; times--) {
        for (volatile uint32_t i = DELAY_100MS; i > 0; i--) {} 
    }
}
