/*
    Lab7 - Interrupts and buzzer

    Finish the SysTickHandler() function, PortA initialization and NVIC_ST_RELOAD value.
*/

#include "verify.h"

#define SYSCTL_RCGC2_R          (*((volatile uint32_t *)0x400FE108))

#define GPIO_PORTA_AMSEL_R      (*((volatile uint32_t *)0x40004528))
#define GPIO_PORTA_PCTL_R       (*((volatile uint32_t *)0x4000452C))
#define GPIO_PORTA_DIR_R        (*((volatile uint32_t *)0x40004400))
#define GPIO_PORTA_DR8R_R       (*((volatile uint32_t *)0x40004508))
#define GPIO_PORTA_AFSEL_R      (*((volatile uint32_t *)0x40004420))
#define GPIO_PORTA_DEN_R        (*((volatile uint32_t *)0x4000451C))
#define GPIO_PORTA_DATA_R       (*((volatile uint32_t *)0x400043FC))
#define SYSCTL_RCGC2_GPIOA      0x00000001  // Port A Clock Gating Control

#define NVIC_ST_CTRL_R          (*((volatile uint32_t *)0xE000E010))
#define NVIC_ST_RELOAD_R        (*((volatile uint32_t *)0xE000E014))
#define NVIC_ST_CURRENT_R       (*((volatile uint32_t *)0xE000E018))
#define NVIC_SYS_PRI3_R         (*((volatile uint32_t *)0xE000ED20))

void PortAInit(void);
void SysTickInit(void);
void SysTickHandler(void);

volatile uint32_t LastSwitchState = 0; // Peab meeles PA3 ehk nupu viimast olekut
volatile uint32_t BuzzerState = 0;     // Heli olek; 0 - välja, 1 - sisse

// Should remain untouched!
int main(void) {
    PortAInit();
    SysTickInit();
    BESGrader();
    while (true) {}
}

/* 
    \brief Subroutine to initialize port A pins for input and output.
    PA3 is external switch input.
    PA2 is output to the buzzer.

    \param None
    \return None
    \note No analog, regular function, no alt function, digital enabled, 8-mA drive select for output
*/
void PortAInit(void) {
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA; // enable clock for port A
    volatile uint32_t delay = SYSCTL_RCGC2_R; // allow time for clock to start
    GPIO_PORTA_AMSEL_R &= ~0x0C; // puhastab bitid 2 ja 3, et need oleksid digitaalsed
    GPIO_PORTA_PCTL_R &= ~0x0000FF00; // lülitab välja UART ja PWM funktsioonid PA2 ja PA3
    GPIO_PORTA_AFSEL_R &= ~0x0C; // -||-
    GPIO_PORTA_DIR_R &= ~0x08; // PA3 = sisend (0) ehk bit 3 on 0, et lugeda nuppu
    GPIO_PORTA_DIR_R |= 0x04;  // PA2 = väljund (1) ehk bit 2 on 1, et juhtida buzzerit
    GPIO_PORTA_DR8R_R |= 0x04; // määrab PA2 väljundile 8 mA voolu
    GPIO_PORTA_DEN_R |= 0x0C; // lülitab PA2 ja PA3 digitaalseks
}

/*
    \brief Subroutine to initialize SysTick. 
    \param None
    \return None
    \note Runs at 16 MHz. 
*/
void SysTickInit(void) {
    NVIC_ST_CTRL_R = 0;           // disable SysTick during setup
    NVIC_ST_RELOAD_R = 18181 - 1;     // muudetud reload value, kuna vaja 880Hz ehk 16 000 000 Hz / 880 Hz = 18181
    NVIC_ST_CURRENT_R = 0;        // any write to current clears it
    NVIC_SYS_PRI3_R = NVIC_SYS_PRI3_R & 0x1FFFFFFF; // priority 0               
    NVIC_ST_CTRL_R = 0x00000007;  // enable with core clock and interrupts
}

/*
    \brief Subroutine to handle the SysTick interrupt. When switch is pressed, the buzzer
    is emmiting a tone at 440 Hz. When the switch is pressed again, the buzzer turns off.
    \param None
    \return None
    \note Interrupt at 1/880Hz = 1.13636ms. 
    When the switch is held down, the buzzer tone is toggled on/off once!
*/
void SysTickHandler(void) {
    uint32_t switchState = GPIO_PORTA_DATA_R & 0x08; // loeb PA3 ehk nupu oleku
    if (switchState && !LastSwitchState) { // võrdleb praegust ja eelmist nupu olekut, kui vajutatud, aga enne polnud siis olek muutub
        BuzzerState ^= 1; // lülitab sisse välja XOR'ga (kui 1 siis 0 ja kui 0 siis 1'ks)
    }
    LastSwitchState = switchState; // salvestab viimase oleku, kui pole vajutatud, siis 0, kui on, siis PA3 biti väärtus 1
    if (BuzzerState) { //KUI BuzzerState on 1, siis 
        GPIO_PORTA_DATA_R ^= 0x04;  // vahetab 880Hz sagedusel HIGH/LOW signaalide vahel, ehk tekitab kokku 440Hz heli
    } else {
        GPIO_PORTA_DATA_R &= ~0x04; // muidu vaikus: tagab 0V väljundil
    }
}
