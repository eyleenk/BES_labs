#include "convert.h"

void ConvertDec(uint32_t num, char* str) {
	if (num > 9999) { //kui üle 9999, siis kuva "**** "
		str[0] = '*'; //
		str[1] = '*';
		str[2] = '*';
		str[3] = '*';
		str[4] = ' ';
		str[5] = '\0'; //lõpetab stringi
		return;	
	}
	//Jagamine lükkab soovitud numbri kõige viimasele kohale ja % 10 võtab selle sealt välja.
	uint32_t d3 = (num / 1000) % 10; // tuhandelised; nt 1234 / 1000 = 1, 1 % 10 = 1
    uint32_t d2 = (num / 100) % 10;  // sajalised; nt 1234 / 100 = 12, 12 % 10 = 2
    uint32_t d1 = (num / 10) % 10;   // kümnelised; nt 1234 / 10 = 123, 123 % 10 = 3
    uint32_t d0 = num % 10;          // ühelised; nt 1234 % 10 = 4

    // Võtab numbri ja teisendab ASCII koodiks ja asendab tüveväärtused tühikutega
	//ASCII 0=48, 1=49, 2=50, 3=51, 4=52, 5=53, 6=54, 7=55, 8=56, 9=57
	/*kui nt 1234, siis 
	d3 + 0 on 1 + 48 = 49, mis on ASCII kood 1 jaoks
	d2 + 0 on 2 + 48 = 50, mis on ASCII kood 2 jaoks
	d1 + 0 on 3 + 48 = 51, mis on ASCII kood 3 jaoks
	d0 + 0 on 4 + 48 = 52, mis on ASCII kood 4 jaoks
	*/

    str[0] = (num >= 1000) ? (d3 + '0') : ' '; //KUI arv on >= 1000, siis võtab massiivis neljandal kohal oleva numbri ja paneb TUHANDELISTE kasti ehk [0]; muidu tühik
    str[1] = (num >= 100) ? (d2 + '0') : ' ';  //KUI arv on >= 100, siis võtab massiivis kolmandal kohal oleva numbri ja paneb SAJALISTE kasti ehk [1];; muidu tühik
    str[2] = (num >= 10) ? (d1 + '0') : ' ';   //KUI arv on >= 10, siis võtab massiivis teisel kohal oleva numbri ja paneb KÜMNELISTE kasti ehk [2]; muidu tühik
    str[3] = d0 + '0';                         //KUI arv on >= 0, siis võtab massiivis esimesel kohal oleva numbri ja paneb ÜHELISTE kasti ehk [3]; muidu tühik
    str[4] = ' '; //numbri järel alati tühik kohale [4]
    str[5] = '\0'; //lõpetab stringi kohal [5]
}

void ConvertDistance(uint32_t num, char* str) {
	if (num > 9999) {
		str[0] = '*';
		str[1] = '*';
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
