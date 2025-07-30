//EE 482: ECE Senior Capstone Design
//Last Updated: 4/12/2024
//Team Name: MAD
//Team Members: Elizabeth Lee, Sheridan Blasey, Daniel Kling, Lisa Jacklin

/* 
Musculoskeletal Skeletal Device 

Overview:The goal of this project is to develop a device that will allow people to accurately assess their muscle 
 strength with and without the help of a healthcare provider. The importance of this project is to provide people who
 need muscle rehabilitation with an easier way to track progress. This will be accomplished via an electromyography (EMG) 
 sensor. An EMG sensor senses the electrical signal sent to the muscles from the brain and quantifies it. The user will attach 
 the EMG sensor and contract their bicep while holding a resistance band. If the person has greater muscle strength the brain
 will have to “recruit” less muscle fibers and will thus result in a smaller signal on the EMG which will give us our muscle 
 rating score. The expected outcomes of the project are to create a mobile device that can be deployed in the field or in 
 patient homes that can measure muscle and joint strength for use in tracking growth, aging, and disease progression.

Objectives:

Notes:

*/

////////LIBRARIES////////////////
#include <msp430.h>

unsigned int ADC_Result[512];
unsigned int i = 0;
unsigned int mVal = 0;
unsigned int buttonFlag = 0;
volatile unsigned char dataVal = 0;
void displayData(char dataVar);
void showChar(char c, int position);
void showCharB(char c, int position);
#define pos1 4 // Digit A1 - L4
#define pos2 6 // Digit A2 - L6
#define pos3 8 // Digit A3 - L8
#define pos4 10 // Digit A4 - L10
#define pos5 2 // Digit A5 - L2
#define pos6 18 // Digit A6 - L18
// Define word access definitions to LCD memories
#define LCDMEMW ((int*)LCDMEM)
#define LCDBMEMW ((int*)LCDBMEM)
// LCD memory map for numeric digits
const char digit[10][2] =
{
{0xFC, 0x28}, /* "0" LCD segments a+b+c+d+e+f+k+q */
{0x60, 0x20}, /* "1" */
{0xDB, 0x00}, /* "2" */
{0xF3, 0x00}, /* "3" */
{0x67, 0x00}, /* "4" */
{0xB7, 0x00}, /* "5" */
{0xBF, 0x00}, /* "6" */
{0xE4, 0x00}, /* "7" */
{0xFF, 0x00}, /* "8" */
{0xF7, 0x00} /* "9" */
};
// LCD memory map for uppercase letters
const char alphabetBig[26][2] =

29

{
{0xEF, 0x00}, /* "A" LCD segments a+b+c+e+f+g+m */
{0xF1, 0x50}, /* "B" */
{0x9C, 0x00}, /* "C" */
{0xF0, 0x50}, /* "D" */
{0x9F, 0x00}, /* "E" */
{0x8F, 0x00}, /* "F" */
{0xBD, 0x00}, /* "G" */
{0x6F, 0x00}, /* "H" */
{0x90, 0x50}, /* "I" */
{0x78, 0x00}, /* "J" */
{0x0E, 0x22}, /* "K" */
{0x1C, 0x00}, /* "L" */
{0x6C, 0xA0}, /* "M" */
{0x6C, 0x82}, /* "N" */
{0xFC, 0x00}, /* "O" */
{0xCF, 0x00}, /* "P" */
{0xFC, 0x02}, /* "Q" */
{0xCF, 0x02}, /* "R" */
{0xB7, 0x00}, /* "S" */
{0x80, 0x50}, /* "T" */
{0x7C, 0x00}, /* "U" */
{0x0C, 0x28}, /* "V" */
{0x6C, 0x0A}, /* "W" */
{0x00, 0xAA}, /* "X" */
{0x00, 0xB0}, /* "Y" */
{0x90, 0x28} /* "Z" */
};

void displayData(char dataVar){
LCDMEMW[pos3/2] = digit[dataVar/10][0] | (digit[dataVar/10][1] << 8);
LCDMEMW[pos4/2] = digit[dataVar%10][0] | (digit[dataVar%10][1] << 8);
LCDBMEMW[pos3/2] = digit[dataVar/10][0] | (digit[dataVar/10][1] << 8);
LCDBMEMW[pos4/2] = digit[dataVar%10][0] | (digit[dataVar%10][1] << 8);
}

//Writes to screen 1
void showChar(char c, int position)
{
if (c == ' ')
{
// Display space
LCDMEMW[position/2] = 0;

30

}
else if (c >= '0' && c <= '9')
{
// Display digit
LCDMEMW[position/2] = digit[c-48][0] | (digit[c-48][1] << 8);
}
else if (c >= 'A' && c <= 'Z')
{
// Display alphabet
LCDMEMW[position/2] = alphabetBig[c-65][0] | (alphabetBig[c-65][1] << 8);
}
else
{
// Turn all segments on if character is not a space, digit, or uppercase letter
LCDMEMW[position/2] = 0xFFFF;
}
}
//Writes to screen 2
void showCharB(char c, int position)
{
if (c == ' ')
{
// Display space
LCDBMEMW[position/2] = 0;
}
else if (c >= '0' && c <= '9')
{
// Display digit
LCDBMEMW[position/2] = digit[c-48][0] | (digit[c-48][1] << 8);
}
else if (c >= 'A' && c <= 'Z')
{
// Display alphabet
LCDBMEMW[position/2] = alphabetBig[c-65][0] | (alphabetBig[c-65][1] << 8);
}
else
{
// Turn all segments on if character is not a space, digit, or uppercase letter
LCDBMEMW[position/2] = 0xFFFF;
}
}

31

int main(void)
{
WDTCTL = WDTPW | WDTHOLD; // Stop WDT
// Configure GPIO
P1DIR |= BIT0; // Set P1.0 to output direction
P1OUT &= ~BIT0; // Clear P1.0
// Configure ADC A1 pin
SYSCFG2 |= ADCPCTL8;
// Configure XT1 oscillator
P4SEL0 |= BIT1 | BIT2; // P4.2~P4.1: crystal pins
// Disable the GPIO power-on default high-impedance mode to activate
// previously configured port settings
PM5CTL0 &= ~LOCKLPM5;
FRCTL0 = FRCTLPW | NWAITS0; // Set number of FRAM waitstates to 0
CSCTL4 = SELMS__DCOCLKDIV | SELA__XT1CLK; // Set ACLK = XT1; MCLK = SMCLK
= DCO
do
{
CSCTL7 &= ~(XT1OFFG | DCOFFG); // Clear XT1 and DCO fault flag
SFRIFG1 &= ~OFIFG;
}while (SFRIFG1 & OFIFG); // Test oscillator fault flag
//CONFIGURE BUTTON
P1DIR &= ~BIT2; //input P1.2
P1REN |= BIT2; //enable pull-up resistor for P1.2
P1OUT |= BIT2; //enable pull-up resistor for P1.2
P1IES &= ~BIT2; //interrupt on falling edge "&="
P1IE |= BIT2; //P1 interrupt enable for bit2
P1IFG &= ~BIT2; //P1 interrupt flag clear

// Configure ADC - Pulse sample mode; ADCSC trigger
ADCCTL0 |= ADCSHT_8 | ADCON + ADCMSC; // ADC ON,temperature
sample period>30us
ADCCTL1 |= ADCSHP + ADCCONSEQ_2; // s/w trig, single ch/conv,

32

MODOSC
ADCCTL2 |= ADCRES; // 10-bit conversion results
ADCMCTL0 |= ADCSREF_0 | ADCINCH_8; // ADC input ch A12 => temp sense
ADCIE |=ADCIE0; // Enable the Interrupt request for a completed
ADC_B conversion
// Configure reference
PMMCTL0_H = PMMPW_H; // Unlock the PMM registers
PMMCTL0_L |= PMMREGOFF_L; // and set PMMREGOFF
PMMCTL2 |= INTREFEN; // Enable internal reference and temperature
sensor
__delay_cycles(400); // Delay for reference settling
// LCD
// Configure LCD pins
SYSCFG2 |= LCDPCTL; // R13/R23/R33/LCDCAP0/LCDCAP1 pins
selected
LCDPCTL0 = 0xFFFF;
LCDPCTL1 = 0x07FF;
LCDPCTL2 = 0x00F0; // L0~L26 & L36~L39 pins selected
LCDCTL0 = LCDSSEL_0 | LCDDIV_7; // flcd ref freq is xtclk
//LCD Operation - Mode 2
LCDVCTL = LCDCPEN | //Enable charge pump
LCDSELVDD | //Internally connect to Vcc
VLCD_8 | //internal 3.08v
(LCDCPFSEL0 | LCDCPFSEL1 | LCDCPFSEL2 | LCDCPFSEL3);//Charge pump frequency
selection 256Hz
LCDMEMCTL |= LCDCLRM | LCDCLRBM; // Clear LCD memory
LCDCSSEL0 = 0x000F; // Configure COMs and SEGs
LCDCSSEL1 = 0x0000; // L0, L1, L2, L3: COM pins
LCDCSSEL2 = 0x0000;

LCDBLKCTL = LCDBLKPRE__512 | //Divide xtclk by 512
LCDBLKMOD_3; //Switch between memory contents of LCDM and
LCDB

33

LCDCTL0 |= LCD4MUX | LCDON;

ADCCTL0 |= ADCENC;
_EINT();
//LPM0; // Turn on LCD, 4-mux selected
while(1){

LCDM0 = 0x21; // L0 = COM0, L1 = COM1
LCDM1 = 0x84; // L2 = COM2, L3 = COM3
showChar('P', pos1);
showChar('R', pos2);
showChar('E', pos3);
showChar('S', pos4);
showChar('S', pos5);
LCDBM0 = 0x21;
LCDBM1 = 0x84;
showCharB('B', pos1);
showCharB('U', pos2);
showCharB('T', pos3);
showCharB('T', pos4);
showCharB('O', pos5);
showCharB('N', pos6);
LPM0;
buttonFlag = 1;
LCDMEMCTL |= LCDCLRM | LCDCLRBM;
// Clear LCD memory

LCDM0 = 0x21; // L0 = COM0, L1 = COM1
LCDM1 = 0x84;
LCDBM0 = 0x21;
LCDBM1 = 0x84;

dataVal = 100 - ((mVal*100)/1024);
displayData(dataVal);

34

LPM0;
}
}
// ADC interrupt service routine
void ADC_ISR(void) __interrupt[ADC_VECTOR]{
if(i < 400){
ADC_Result[i] = ADCMEM0;
i++;
__delay_cycles(10000);
}
else{//input code here for when we're done taking our samples
ADCCTL0 &= ~ ADCENC;
for(i = 0; i < 400; i++)
{
if(ADC_Result[i] > mVal){
mVal = ADC_Result[i];
}
}
i=0;
LPM0_EXIT;
}
P1IV = 0;
//while(1);
}
void Button(void) __interrupt[PORT1_VECTOR]{ //start sampling when button pressed
//Check if button press happened
if (P1IV == P1IV_P1IFG2) //Button press interrupt present for P1.2
{
if(buttonFlag == 0){
ADCCTL0 |= ADCSC + ADCENC; //start conversion
LCDMEMCTL |= LCDCLRM | LCDCLRBM;
LCDM0 = 0x21; // L0 = COM0, L1 = COM1
LCDM1 = 0x84;
showChar('M', pos1);
showChar('E', pos2);

35

showChar('A', pos3);
showChar('S', pos4);
LCDBM0 = 0x21;
LCDBM1 = 0x84;
showCharB('M', pos3);
showCharB('E', pos4);
showCharB('A', pos5);
showCharB('S', pos6);
}
else{
buttonFlag = 0;
}
}
LPM0_EXIT;
P1IV = 0;

__no_operation();
}
/* PROGRAM NOTES
program uses multiple sample single channel activated via button to take x samples then stop.
samples for about 6.5 seconds right now.
current issues:
phantom break point issue when placing break points sometimes?
*/
