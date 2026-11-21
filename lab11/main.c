/*
    Lab11 - ADC, slide potentiometer

    Complete main function, ConvertDistance, SysTickHandler
    NVIC_ST_RELOAD and initialization in adc.c. In the main function choose either
    UART or OLED.
*/

#include "verify.h"
#include "i2c_helper.h"
#include "ssd1306.h"
#include "adc.h"

#define NVIC_ST_CTRL_R          (*((volatile uint32_t *)0xE000E010))
#define NVIC_ST_RELOAD_R        (*((volatile uint32_t *)0xE000E014))
#define NVIC_ST_CURRENT_R       (*((volatile uint32_t *)0xE000E018))
#define NVIC_SYS_PRI3_R         (*((volatile uint32_t *)0xE000ED20))

// Calibration 
#define A 1184 //kalibreerimiskonstant A
#define B 302 //kalibreerimiskonstant B e. algne nihe

//kasutatud PLI ehk piecewise linear interpolation (tükiti lineaarse interpoleerimise meetodit) kuna tegu ei ole lineaarse vaid logaritmilise potekaga, mille keskpunktis on peak
#define DISTANCE_LEN 8 //kauguse stringi pikkus, siinkohal 8 tähemärki, kuna kaugus on kujul "X.XXX cm"
#define TABLE_SIZE 7 //kalibreerimispunktide arv PLI tabelis; Xtable ja Ytable sama suured


// Mõõdetud ADC väärtused kasvavas järjekorras (HW-223 logaritmilise potentsiomeetri jaoks)
const uint32_t Xtable[TABLE_SIZE] = {0, 78, 266, 641, 1550, 2780, 2793}; //mõõdetud ADC toorväärtused, ehk potekalt tulevad andmed (0-4095)
const uint32_t Ytable[TABLE_SIZE] = {0, 500, 1000, 1500, 2000, 2500, 3000}; // ja siin nendele vastavad tegelikud kaugused (0.001 cm ühikutes)

//fn'de prototüübid
void ConvertDistance(uint32_t num); //konverdib kauguse täisarvu (nt 1234) tekstiks kujul "1.234 cm"
uint32_t InterpolateDistance(uint32_t adc_sample); //teisendab ADC toorväärtuse PLI tabeli kauguse ühikuteks
void SysTickInit(void);
void SysTickHandler(void);
void ConfigureDisplay(void);
void OLEDprint(const char* unit, const char* str); //ekraani asjad

char str[DISTANCE_LEN + 1]; // + 1 for '\0'
uint32_t distance;  //mailbox, mille kaudu systickhandler saadab arvutatud kauguse ühikutes 0.001 cm main'i
uint32_t adc_data;  //toores 12 bitine ADC väärtus (pingetase vahemikus 0-4095), kus 4095 on 3.3v
volatile uint8_t flag = 0; /*sünkroniseerimislipp uue andmete olemasolu kohta; 1=andmed olemas, 0=andmed loetud; 
võib uuesti mõõtma hakata. volatile - ei optimeeri ehk loetav alati otse mälust*/

int main(void) {
    ADC0Init();
    SysTickInit();
    //ConfigureDisplay(); // Comment out if using UART
    BESGrader();
    //while (1);
    while (true) { //loop, mis kontrollib, kas on olemas uus kaugus, kui on, siis teisendab selle stringiks ja kuvab ekraanile või UARTi; iga 25 ms tagant toimub mõõtmine
        if (flag == 1) { //kui sünkroniseerimislipu väärtus on 1, siis mõõtmistulemus valmis
            ConvertDistance(distance); //teisendab kauguse stringiks
            UARTprintf("Distance ~ %s\n", str); // Comment out if using display - ei kasuta, ei kommenteeri välja
            flag = 0; //kui sünkroniseerimislipp on 0, siis saab uuesti mõõtma hakata
        }
        //SSD1306DisplayClear(); // Comment out if using UART
        //OLEDprint("Distance ~ ", str); // Comment out if using UART
        //SSD1306DisplayUpdate(); // Comment out if using UART
    }
}

/*
Teisendab 12-bitise ADC toornäidu kauguseks (0.001 cm) PLI (Piecewise Linear Interpolation) tabeli abil. 
*/
uint32_t InterpolateDistance(uint32_t adc_sample) {
    if (adc_sample <= Xtable[0]) { //kui näit on väiksem või võrdne esimese tabelipunktiga
        return Ytable[0]; //tagastab 0 ehk väiksem kaugus (0.001 cm ühikutes)
    }
    if (adc_sample >= Xtable[TABLE_SIZE - 1]) { //kui näit on suurem või võrdne viimase tabelipunktiga
        return Ytable[TABLE_SIZE - 1]; //tagastab maksimaalse kauguse (3000 ehk 3.000 cm)
    }

    for (uint32_t i = 0; i < TABLE_SIZE - 1; i++) { //käib läbi kõik tabelis olevad vahemikud
        if (adc_sample >= Xtable[i] && adc_sample <= Xtable[i + 1]) { //leiab kaks kõrvuti punkti, mille vahele ADC näit jääb
            uint32_t dx = Xtable[i + 1] - Xtable[i]; //arvutab ADC väärtuste vahe selles lõigus
            uint32_t dy = Ytable[i + 1] - Ytable[i]; //arvutab kauguste vahe selles lõigus
            return Ytable[i] + ((adc_sample - Xtable[i]) * dy) / dx; //arvutab täpse kauguse lineaarse interpolatsiooni sirge valemiga
        }
    }
    return 0; //tagastab 0, kui sobivat vahemikku ei leitud
}

/*
    \brief Converts the decimal number to a formatted distance string.
    4       to "0.004 cm", 
    31      to "0.031 cm",
    102     to "0.102 cm",
    2210    to "2.210 cm",
    10000   to "*.*** cm",
    >9999   to "*.*** cm".
    \param num Decimal numerical value to be converted to a string
    \return None
    \note Remember to set the last character as '\0'!
*/
void ConvertDistance(uint32_t num) {
    if (num > 9999) {
		str[0] = '*';
		str[1] = '.'; // punkt kujule "*.*** cm"
		str[2] = '*';
		str[3] = '*';
		str[4] = '*';
		str[5] = ' '; //tühik nr järel
		str[6] = 'c'; 
		str[7] = 'm'; // + cm
		str[8] = '\0';
		return;
	}
    // Nt arv 1234 tähendab kaugust 1.234, et saada kohad enne ja pärast koma
	uint32_t d3 = (num / 1000) % 10; // täisosa enne koma
    uint32_t d2 = (num / 100) % 10;  // kümnendikud ehk esimene koht pärast koma
    uint32_t d1 = (num / 10) % 10;   // sajandikud ehk teine koht pärast koma
    uint32_t d0 = num % 10;          // tuhandikud ehk kolmas koht pärast koma

	//Paigutab numbrite kohad massiivi formaati X.XXX cm
    str[0] = d3 + '0'; //täisosa (ASCII märgina kohale [0])
    str[1] = '.';      //koma kohale [1]
    str[2] = d2 + '0'; //künendikud ehk esimene koht pärast koma kohale [2]
    str[3] = d1 + '0'; //sajandikud ehk teine koht pärast koma kohale [3]
    str[4] = d0 + '0'; //kolmas koht pärast koma kohale [4]
    str[5] = ' ';      //numbri järel tühik kohale [5]
    str[6] = 'c';      
    str[7] = 'm';      // + cm järelliide kohale [6] ja [7]
    str[8] = '\0';     //lõpetab stringi kohal [8]
}
/* 
    \brief Subroutine initializes SysTick periodic interrupts.
    \param None
    \return None
    \note Initialize SysTick interrupts to trigger at 40 Hz, 25 ms.
*/
void SysTickInit(void) {
    NVIC_ST_CTRL_R = 0;                 // disable SysTick during setup
    NVIC_ST_RELOAD_R = 400000 - 1;    // reload 40 Hz kui 16 MHz taktsagedus ehk 16 000 000 / 40 = 400 000
    NVIC_ST_CURRENT_R = 0;              // any write to current clears it
    NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x1FFFFFFF) | 0x40000000; // priority 2        
    NVIC_ST_CTRL_R = 0x07;              // enable SysTick with core clock and interrupts
}

/*
    \brief Collects an 12-bit ADC sample, converts it to 32-bit unsigned
    fixed-point distance (resolution 0.001 cm) and stores in mailbox.
    Calibrate the A and B constants to achieve maximum linearity.
    \param None
    \return None
    \note Handler executes every 25 ms.
*/
void SysTickHandler(void) {
    if (flag == 0) { //kui flag on 0, siis saab lugeda uut väärtust
        adc_data = ADC0In(); //loeb ADC väärtuse 12 bitti = 0-4095
        distance = InterpolateDistance(adc_data); //teisendab ADC näidu kauguseks PLI tabeli abil
        flag = 1; //lipp näitab, et on olemas uus väärtus
    }
}

/*
    \brief Configures the OLED screen display for use.
    PD0 (PUR by PB6) is SCL (Clk pin on screen), and PD1 (PUR by PB7) is SDA (Data pin on screen).
    Connect +3.3V and GND to 3Vo and GND on screen.
    \param None
    \return None
*/
void ConfigureDisplay(void) {
    I2C3Config();
    SSD1306DisplayInit();
}

/*
    \brief Prints the unit and the string on the OLED screen
    \param unit Unit name, like "Distance ~ " 
    \param str_distance String to be displayed on the screen
    \return None
*/
void OLEDprint(const char* unit, const char* str_distance) {
    SSD1306SetCursor(0, 0);
    SSD1306DrawStr(unit);
    SSD1306DrawStr(str_distance);
}