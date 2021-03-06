/*
 * File:   main_adc.c
 * Author: akirik
 *
 * Created on January 24, 2016, 12:12 PM
 * 
 * Analog to Digital Converter (ADC)
 * 
 *  Board connection (PICKit 44-Pin Demo Board; PIC16F887):
 *   PIN                	Module                         				  
 * -------------------------------------------                        
 *  RD0          			LED
 *  RD1          			LED
 *  RD2          			LED
 *  RD3          			LED
 *  RA0 (RP1)               POTENCIOMETER
 *
 */

/* The __delay_ms() function is provided by XC8. 
It requires you define _XTAL_FREQ as the frequency of your system clock. 
The compiler then uses that value to calculate how many cycles are required to give the requested delay. 
There is also __delay_us() for microseconds and _delay() to delay for a specific number of clock cycles. 
Note that __delay_ms() and __delay_us() begin with a double underscore whereas _delay() 
begins with a single underscore.
*/
#define _XTAL_FREQ 8000000

// PIC16F887 Configuration Bit Settings
// 'C' source line config statements
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown Out Reset Selection bits (BOR enabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#define PIN_A0                    0
#define ACQ_US_DELAY              5

void system_init()
{
    OSCCON=0x70;          // Select 8 Mhz internal clock
    
	// I/O
		// ANSELx registers
			ANSEL = 0x00;         // Set PORT ANS0 to ANS7 as Digital I/O
			ANSELH = 0x00;        // Set PORT ANS8 to ANS11 as Digital I/O
			ANSELbits.ANS0 = 1;   // Set RA0/AN0 to analog mode
	  
		// TRISx registers (This register specifies the data direction of each pin)
			TRISA = 0x00;         // Set All on PORTB as Output    
			TRISB = 0x00;         // Set All on PORTB as Output    
			TRISC = 0x00;         // Set All on PORTC as Output   
            TRISD = 0x00;         // Set All on PORTD as Output   
            TRISE = 0x00;         // Set All on PORTE as Output   
			TRISAbits.TRISA0 = 1; // Set RA0/AN0 as Input
		
		// PORT registers
			PORTA = 0x00;         // Set PORTA all 0
			PORTB = 0x00;         // Set PORTB all 0
			PORTC = 0x00;         // Set PORTC all 0
            PORTD = 0x00;         // Set PORTD all 0
            PORTE = 0x00;         // Set PORTE all 0
        
    // ADC setup
	// The PIC16F887 has a 10-bit ADC
	// It provides value 0 to 1023 as the input voltage rises from Vss to Vref
	// Vref is the analog reference voltage. 
	// It can either be Vdd or supplied externally to the RA1/AN1/Vref pin
	// The PIC16F887 has twelve analog input pins (AN0 through AN11). 
	// In order to enable the analog function for a pin, 
	// the corresponding bit must be set in the ANSEL or ANSELH register (see above).
    // Steps to configure:
    //  1. voltage reference in the ADCON1 register
    //  2. select ADC conversion clock in the ADCON0 register
    //  3. select one input channel CH0CH13 in the ADCON0 register
    //  4. select data format in the ADCON1 register (ADFM)
    //  5. enable AD converter in the ADCON0 register (ADON)
    //  optional configure interrupt:
    //  6. clear ADIF bit
    //  7. set ADIE, PEIE and GIE bits (see interrupts for more instructions)
    /* 
     * -------------------ADCON0------------------------------
     * Bit#:  ----7----6----5----4----3----2----1-------0-----
     * ANS:   --|ADCS1|ADCS0|CHS3|CHS2|CHS1|CHS0|GO/DONE|ADON|--
     * -------------------------------------------------------
      
        ADCS<1:0>: A/D Conversion Clock Select bits
            00 = FOSC/2
            01 = FOSC/8
            10 = FOSC/32
            11 = FRC (clock derived from a dedicated internal oscillator = 500 kHz max)     
      
        CHS<3:0> - channel select: This selects which voltage is fed into the ADC. 
            Setting this 0 to 11 selects AN0 to AN11 respectively. 
            Setting 12 feeds CVref (Comparator voltage reference). 
            Setting 13 connects the 0.6V fixed voltage reference from the
            Comparator module to the ADC.

        GO/DONE: 1 to start ADC conversion. 
            Be sure to wait at least 5us before doing this after changing 
            the input channel or performing a previous conversion.
            This bit is read to see if the current conversion is complete.
            The ADC will automatically reset this bit to 0 when conversion finishes.

        ADON - ADC enable bit: 0 - ADC disabled and consumes no current, 1 - ADC enabled.
     
     * -------------------ADCON1-------------------------
     * Bit#:  ----7----6-----5-----4----3---2---1---0----
     * ANS:   --|ADFM| 0  |VCFG1|VCFG0| 0 | 0 | 0 | 0 |--
     * --------------------------------------------------
     ADFM - result format: 0 - left justified (default), 1 - right justified:
            * -------------------------ADRESH------------------------|-------------------ADRESL-----------------------
            * Bit#:  ---7-----6-----5-----4-----3-----2-----1-----0--|--7-----6-----5-----4-----3-----2-----1-----0---
            * bit:   |  0  |  0  |  0  |  0  |  0  |  0  |bit-9|bit-8|bit-7|bit-6|bit-5|bit-4|bit-3|bit-2|bit-1|bit-0|
            * --------------------------------------------------------------------------------------------------------
            result = (ADRESH<<8)+ADRESL;

            * -------------------------ADRESH------------------------|-------------------ADRESL-----------------------
            * Bit#:  ---7-----6-----5-----4-----3-----2-----1-----0--|--7-----6-----5-----4-----3-----2-----1-----0---
            * bit:   |bit-9|bit-8|bit-7|bit-6|bit-5|bit-4|bit-3|bit-2|bit-1|bit-0|  0  |  0  |  0  |  0  |  0  |  0  |
            * --------------------------------------------------------------------------------------------------------
            This is useful if only the 8 most significant bits are required. 
            In this case, we just simply copy the contens of the ADRESH register.
 
     VCFG1: Voltage Reference bit
        1 = VREF - pin
        0 = VSS
     VCFG0: Voltage Reference bit
        1 = VREF + pin
        0 = VDD 
          
     ADC could be user with interrupts
        In order to use the ADC interrupts, the GIE (Global Interrupt Enable) 
        and PEIE (Peripheral Interrupt Enable) bits in the INTCON register must be set to 1.
     
        * -------------------PIE1-----------------------------------
        * Bit#:  ----7----6----5---4-----3-----2------1------0------
        * ANS:   --| 0 |ADIE|RCIE|TXIE|SSPIE|CCP1IE|TMR2IE|TMR1IE|--
        * ----------------------------------------------------------
           ADIE - ADC interrupt enable. 
               Set to 1 to generate interrupts when ADC is completed
        
        * -------------------PIR1-----------------------------------
        * Bit#:  ----7----6---5----4----3------2------1------0------
        * ANS:   --| 0 |ADIF|RCIF|TXIF|SSPIF|CCP1IF|TMR2IF|TMR1IF|--
        * ----------------------------------------------------------
           ADIF - ADC interrupt flag. 
               This will be set to 1 by the ADC when a conversion completes. 
               This must be cleared by software in the interrupt service routine 
               to prepare the interrupt flag for the next conversion.
    */
        ADCON1bits.ADFM = 1;   		// ADC result is right justified
        ADCON1bits.VCFG0 = 0;    	// Vref uses Vdd as reference
        ADCON0bits.ADCS0 = 0;       // Fosc/8 is the conversion clock
									//   This is selected because the conversion
									//   clock period (Tad) must be greater than 1.5us.
									//   With a Fosc of 4MHz, Fosc/8 results in a Tad
									//   of 2us.
        ADCON0bits.CHS = PIN_A0;	// Select analog input - AN0
        ADCON0bits.ADON = 1;    	// Turn on the ADC
        
}

uint16_t ADC_GetConversion()
{
    __delay_us(ACQ_US_DELAY);					// Acquisition time delay
    ADCON0bits.GO_nDONE = 1;					// Start the conversion
    while (ADCON0bits.GO_nDONE);				// Wait for the conversion to finish
    return ((uint16_t)((ADRESH << 8) + ADRESL));// Conversion finished, return the result
}

void main(void) 
{
    system_init();
    
    while(1)  
	{
        uint16_t adcResult = ADC_GetConversion();
                
        //PORTCbits.RC0 = adcResult > 512 ? 1 : 0;// Turn on the LED if the input voltage is above Vdd/2
		//__delay_ms(50);                         // sleep 50 milliseconds
		
		PORTDbits.RD0 = 0;
        PORTDbits.RD1 = 0;
        PORTDbits.RD2 = 0;
        PORTDbits.RD3 = 0;
		
        if(adcResult > 256) {
            PORTDbits.RD0 = 1;
        } 
        if(adcResult > 512)  {
            PORTDbits.RD1 = 1;
        } 
        if(adcResult > 768)  {
            PORTDbits.RD2 = 1;
        }
        if(adcResult > 1000)  {
            PORTDbits.RD3 = 1;      
        }
		__delay_ms(50);                         // sleep 50 milliseconds
    }
    
  return;
}
