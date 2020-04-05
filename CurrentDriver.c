/*
 * File:   CurrentDriver.c
 * Author: dell
 *
 * Created on March 26, 2020, 10:50 PM
 */


#include <xc.h>
#include <stdint.h>

#define _XTAL_FREQ 4000000 //for __delay_ms it need to know the cpu speed

//FOSC = 0x4 setup as internal oscillator
//WDTE = 0x01 watch dog timer enable
//PWRTE = 0x1 Power up timer, will wait 72 ms before executing the code (waiting to stabilise the power)) 1 mean disable
//MCLRE = 0x1  Disable master clear
//BOREN = 0x1 Brown out enable bit detect power decreasing and will reboot the sotware
// CP CPD is code protection, if set to 1 code protection is disable.

//AN4 (RC0) will be the AtoD Pin
//RC5 will be the mosfet driver

#pragma config FOSC = 0x5, WDTE = 0x0, PWRTE = 0x1, MCLRE = 0x0, BOREN = 0x0, CP = 0x1, CPD = 0x1

uint8_t wVoltageRead = 0;
uint8_t wVref = 0x0F;

void main(void) 
{
    uint8_t ErrorCode=0;
    
    int16_t NB_Wait=0;
    INTCONbits.GIE = 0; //Disable interrupt
    

    
    PORTC = 0x00;
    TRISC = 0xFF; //All pin from TRISC configured as input
    TRISCbits.TRISC5 = 0; //Driver Outpout
    TRISCbits.TRISC4 = 0; //Error Led
    ADCON0bits.CHS0 = 0;
    ADCON0bits.CHS1 = 0;
    ADCON0bits.CHS2 = 1;   //Selection of AN4
    ADCON0bits.ADFM = 0;   //Using only msb in ADRESH
    ADCON0bits.VCFG = 0;   //Using only VDD as reference in ADRESH
    ADCON1bits.ADCS = 1;   //Conversion in 2 TOSC
    ADCON0bits.ADON = 1;  //Enable the AToD Module
    
    ANSEL = 0x00;         //Set all do digital
    ANSELbits.ANS4 = 1;   //An4 is analog
    
    PIE1bits.ADIE = 0;    //Disable AtoD interrupt
    INTCONbits.PEIE = 0;  //Disable periferic interrupt
    INTCONbits.GIE = 0; // Disable Interrupt
    
    for(uint16_t i=0; i< 65500;i++)
    {
       __asm__ __volatile__ ("nop"); //Let all capacitor to charge correctly
    }
    
    int16_t i=0;
    
    unit8_t wState=0;
    switch(wState)
    {
        case 0://Check the maximum noise 
            
        
    }
 while(1)
 {
    PORTCbits.RC5 = 1;
    
    for(i=0; i< NB_Wait;i++)
    {
       __asm__ __volatile__ ("nop");
    }
    
    ADCON0bits.GO_nDONE = 1; //Starting the conversion
    __asm__ __volatile__ ("nop"); //cannot work need 11 us for the chold
    PORTCbits.RC5 = 0;
    
    for(i=0; i< NB_Wait;i++)
    {
       __asm__ __volatile__ ("nop");
    }    
    while(ADCON0bits.GO_nDONE == 1)
    {}
    if(ADRESH > wVref)
    {
        NB_Wait--;
        if(NB_Wait == -1)
        {
            ErrorCode= 1;
            break;   
        }
    }
    else
    {
        NB_Wait++;
        if( NB_Wait == 255) //check if we don't roll over
        {
           ErrorCode=2;
           break;   
        }
    }
 }
    
 PORTCbits.RC5 = 0;

while(ErrorCode != 0)
{
    switch(ErrorCode) 
    {
        case 1:
            for(uint16_t i=0; i< 65000;i++)
            {
               __asm__ __volatile__ ("nop"); //Let all capacitor to charge correctly
            }
            PORTCbits.RC4 = 1;
            for(uint16_t i=0; i< 65000;i++)
            {
               __asm__ __volatile__ ("nop"); //Let all capacitor to charge correctly
            }
            PORTCbits.RC4 = 0;
            break;
        case 2:
            PORTCbits.RC4 = 1;
            break;
        default:
            break;
        
    }
    //To add error loop to flash the led to provide error code.
}
    
    return;
}

void __interrupt() myint(void) 
{

}
