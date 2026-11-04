/*
    Lab8 - FSM

    Complete port initializations and main function.
    Complete the enum states in the fsm.h.
    Fill in the 'fsm' array variable.
    Choose the correct argument for the SysTickWait.
*/

#include "verify.h"
#include "fsm.h"

#define SYSCTL_RCGC2_R          (*((volatile uint32_t *)0x400FE108))

#define GPIO_PORTF_DATA_R       (*((volatile uint32_t *)0x400253FC))
#define GPIO_PORTF_DIR_R        (*((volatile uint32_t *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile uint32_t *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile uint32_t *)0x40025510))
#define GPIO_PORTF_DEN_R        (*((volatile uint32_t *)0x4002551C))
#define GPIO_PORTF_AMSEL_R      (*((volatile uint32_t *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile uint32_t *)0x4002552C))
#define SYSCTL_RCGC2_GPIOF      0x00000020  // Port F Clock Gating Control
	
#define GPIO_PORTB_DATA_R       (*((volatile uint32_t *)0x400053FC))
#define GPIO_PORTB_DIR_R        (*((volatile uint32_t *)0x40005400))
#define GPIO_PORTB_AFSEL_R      (*((volatile uint32_t *)0x40005420))
#define GPIO_PORTB_DEN_R        (*((volatile uint32_t *)0x4000551C))
#define GPIO_PORTB_AMSEL_R      (*((volatile uint32_t *)0x40005528))
#define GPIO_PORTB_PCTL_R       (*((volatile uint32_t *)0x4000552C))
#define SYSCTL_RCGC2_GPIOB      0x00000002  // Port B Clock Gating Control
	
#define GPIO_PORTE_DATA_R       (*((volatile uint32_t *)0x400243FC))
#define GPIO_PORTE_DIR_R        (*((volatile uint32_t *)0x40024400))
#define GPIO_PORTE_AFSEL_R      (*((volatile uint32_t *)0x40024420))
#define GPIO_PORTE_DEN_R        (*((volatile uint32_t *)0x4002451C))
#define GPIO_PORTE_AMSEL_R      (*((volatile uint32_t *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile uint32_t *)0x4002452C))
#define SYSCTL_RCGC2_GPIOE      0x00000010  // Port E Clock Gating Control
	
#define NVIC_ST_CTRL_R          (*((volatile uint32_t *)0xE000E010))
#define NVIC_ST_RELOAD_R        (*((volatile uint32_t *)0xE000E014))
#define NVIC_ST_CURRENT_R       (*((volatile uint32_t *)0xE000E018))
#define NVIC_ST_CTRL_COUNT      0x00010000  // Count Flag

void SysTickWait100ms(uint32_t times);
void SysTickInit(void);
void PortBInit(void);
void PortEInit(void);
void PortFInit(void);

const FSMStateData_t fsm[Count] = { //kujul autode olek, jalakäijate olek, aeg ning millisesse olekusse edasi minna
    {0x04, 0x02, 30, {goCar, goCar, waitCar, waitCar}},       // goCar: autodel roheline (0x04), jalakäijatel punane (0x02), kestab 3 s. Kui jalakäija vajutab (sisend 2 või 3), läheb waitCar, muidu jääb goCar
    {0x02, 0x02, 10, {stopCar, stopCar, stopCar, stopCar}},   // waitCar: autodel kollane (0x02), jalakäijatel punane (0x02), kestab 1 s. Liigub alati stopCar olekusse
    {0x01, 0x02, 10, {goPed, goPed, goPed, goPed}},           // stopCar: kõigile punane (autod 0x01, jalakäijad 0x02), paus kestab 1 s. Liigub alati goPed olekusse
    {0x01, 0x08, 30, {offPed1, offPed1, goPed, offPed1}},     // goPed: autodel punane (0x01), jalakäijatel roheline (0x08), kestab 3 s. Kui ainult jalakäija ootab (sisend 2), jääb siia muidu alustab vilgutamist offPed1

    {0x01, 0x00, 5, {onPed1, onPed1, onPed1, onPed1}},        // offPed1: vilgutamine 1. kord väljas (PF=0x00), autodel punane (0x01), kestab 0.5 s. Liigub faasi onPed1
    {0x01, 0x08, 5, {offPed2, offPed2, offPed2, offPed2}},    // onPed1: vilgutamine 1. kord sees (PF=0x08), autodel punane (0x01), kestab 0.5 s. Liigub faasi offPed2
    {0x01, 0x00, 5, {onPed2, onPed2, onPed2, onPed2}},        // offPed2: vilgutamine 2. kord väljas (PF=0x00), autodel punane (0x01), kestab 0.5 s. Liigub faasi onPed2
    {0x01, 0x08, 5, {offPed3, offPed3, offPed3, offPed3}},    // onPed2: vilgutamine 2. kord sees (PF=0x08), autodel punane (0x01), kestab 0.5 s. Liigub faasi offPed3
    {0x01, 0x00, 5, {stopPed, stopPed, stopPed, stopPed}},    // offPed3: vilgutamine 3. kord väljas (PF=0x00), autodel punane (0x01), kestab 0.5 s. Liigub faasi stopPed

    {0x01, 0x02, 10, {goCar, goCar, goCar, goCar}}            // stopPed: kõigile punane (autod 0x01, jalakäijad 0x02), kestab 1 s paus. Liigub tagasi autode roheliseks goCar
};

/*
goCar:   Autodel roheline (PB2=1 ehk PB=0x04), jalakäijatel punane (PF1=1 ehk PF=0x02); kui jalakäija vajutab nuppu, läheb waitCar, muidu jääb goCar.
waitCar: Autodel kollane (PB1=1 ehk PB=0x02), jalakäijatel punane (PF1=1 ehk PF=0x02); 1 s hoiatus autodele, liigub alati stopCar olekusse.
stopCar: Autodel punane (PB0=1 ehk PB=0x01), jalakäijatel punane (PF1=1 ehk PF=0x02); 1 s ohutuspaus kõigile punane, liigub olekusse goPed.
goPed:   Autodel punane (PB0=1 ehk PB=0x01), jalakäijatel roheline (PF3=1 ehk PF=0x08); kui ainult jalakäija hoiab nuppu, püsib siin, muidu läheb offPed1.
offPed:  Vilgutamine (3x välja PF=0x00, 2x sisse PF=0x08); hoiatab ületusaja lõpust autode punase taustal, viib olekusse stopPed.
stopPed: Autodel punane (PB0=1 ehk PB=0x01), jalakäijatel punane (PF1=1 ehk PF=0x02); 1 s ohutuspaus kõigile punane, liigub tagasi goCar.
*/

int main(void) {
    PortBInit();
    PortEInit();
    PortFInit();
    SysTickInit();
    BESGrader();
    uint32_t input;           // input combination from sensors on PE0-1
    FSMState_t state = goCar; // algolekuks seatakse goCar (autodel roheline)
    while (true) {
        GPIO_PORTB_DATA_R = fsm[state].portb_out; // näitab autode fooritulesid (PB0, PB1, PB 2)
        GPIO_PORTF_DATA_R = fsm[state].portf_out; // näitab jalakäijate fooritulesid (PF1, PF3)
        SysTickWait100ms(fsm[state].times);       // ootab määratud aega
        input = GPIO_PORTE_DATA_R & 0x03;         // loeb PE0 ja PE1 sisendi väärtuse
        state = fsm[state].next[input];           // läheb sõltuvalt sisendist järgmisesse olekusse
    }
}

/* 
    \brief Subroutine to delay for a specifiad amount of time.
    \param delay  
    \return The delay parameter is in units of the 16 MHz core clock (62.5 ns).
*/
void SysTickWait(uint32_t delay) {
    NVIC_ST_RELOAD_R = delay - 1;                           // number of counts to wait
    NVIC_ST_CURRENT_R = 0;                                  // any value written to CURRENT clears it
    while ((NVIC_ST_CTRL_R & NVIC_ST_CTRL_COUNT) == 0) {}   // wait for count flag
}

/* 
    \brief Subroutine to delay exactly 100ms X times
    \param times How many times to delay 100ms
    \return None
	\note Use this function for delays in the main program.
*/
void SysTickWait100ms(uint32_t times) {
    for (uint32_t i = 0; i < times; i++) {
        SysTickWait(1600000); // 1 sek = 16 000 000 tsüklit, 100 ms (0.1 s) = 1 600 000 tsüklit
    }
}

/*
    \brief Subroutine to initialize SysTick. 
    \note Runs at 16 MHz. 
*/
void SysTickInit(void) {
    NVIC_ST_CTRL_R = 0;               // disable SysTick during setup
    NVIC_ST_CTRL_R = 0x00000005;      // enable SysTick with core clock
}

/* 
    PB0 is car red external LED.
    PB1 is car yellow external LED.
	PB2 is car green external LED.
*/

//Määrab PB0, PB1 ja PB2 väljunditeks 
void PortBInit(void) {
    volatile uint32_t delay;
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; //lülitab taktsignaali sisse
    delay = SYSCTL_RCGC2_R;           // annab aega, et kell stailiseeruks
    GPIO_PORTB_AMSEL_R &= ~0x07;      // PB0, PB1 ja PB2 analoog välja (0)
    GPIO_PORTB_PCTL_R &= ~0x00000FFF;  // määrab tavaliseks GPIO'ks
    GPIO_PORTB_DIR_R |= 0x07;         // määrab väljundid - PB2 = 1, PB1 = 1, PB0 = 1
    GPIO_PORTB_AFSEL_R &= ~0x07;      // tavareiim (ainult GPIO)
    GPIO_PORTB_DEN_R |= 0x07;         // lubab PB0, PB1 ja PB2 digitaalse väljundid
}

//Määrab PE0 ja PE1 sisenditeks
void PortEInit(void) {
    volatile uint32_t delay;
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;
    delay = SYSCTL_RCGC2_R;           // annab aega, et kell stailiseeruks
    GPIO_PORTE_AMSEL_R &= ~0x03;      // PE0 ja PE1analoog välja (0)
    GPIO_PORTE_PCTL_R &= ~0x000000FF;  // määrab tavaliseks GPIO'ks
    GPIO_PORTE_DIR_R &= ~0x03;        // määrab sisendid - PE1 = 0, PE0 = 0
    GPIO_PORTE_AFSEL_R &= ~0x03;      // tavareziim (ainult GPIO)
    GPIO_PORTE_DEN_R |= 0x03;         // lubab PE0 ja PE1digitaalse väljundi
}

//Määrab PF1 ja PF3 väljunditeks
void PortFInit(void) { 
    volatile uint32_t delay;
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;
    delay = SYSCTL_RCGC2_R;           // annab aega, et kell stailiseeruks
    GPIO_PORTF_AMSEL_R &= ~0x0A;      //  PF1 ja PF3 analoog välja (0)
    GPIO_PORTF_PCTL_R &= ~0x0000F0F0;  // määrab tavaliseks GPIO'ks
    GPIO_PORTF_DIR_R |= 0x0A;         // määrab väljundid - PF3 = 1, PF1 = 1
    GPIO_PORTF_AFSEL_R &= ~0x0A;      // tavareziim (ainult GPIO)
    GPIO_PORTF_DEN_R |= 0x0A;         // lubab PF3 ja PF1 digitaalse väljundi
}
