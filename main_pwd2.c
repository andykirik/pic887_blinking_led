/*
 * File:   main_pwm.c
 * Author: akirik
 *
 * Created on January 24, 2016, 12:12 PM
 * 
 * PWM
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
We are using the internal oscillator at its default 4MHz, so _XTAL_FREQ is defined as 4000000. 
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
	  
		// TRISx registers (This register specifies the data direction of each pin)
			TRISA = 0x00;         // Set All on PORTB as Output    
			TRISB = 0x00;         // Set All on PORTB as Output    
			TRISC = 0x00;         // Set All on PORTC as Output    
            TRISD = 0x00;         // Set All on PORTD as Output   
            TRISE = 0x00;         // Set All on PORTE as Output   
		
		// PORT registers
			PORTA = 0x00;         // Set PORTA all 0
			PORTB = 0x00;         // Set PORTB all 0
			PORTC = 0x00;         // Set PORTC all 0
            PORTD = 0x00;         // Set PORTD all 0
            PORTE = 0x00;         // Set PORTE all 0
            
    // ADC setup
        ADCON1bits.ADFM = 0;   		// ADC result is left justified
        ADCON1bits.VCFG0 = 0;    	// Vref uses Vdd as reference
        ADCON1bits.VCFG1 = 0;       // use VCC as reference gnd
        ADCON0bits.ADCS0 = 0;       // Fosc/8 is the conversion clock
									//   This is selected because the conversion
									//   clock period (Tad) must be greater than 1.5us.
									//   With a Fosc of 4MHz, Fosc/8 results in a Tad
									//   of 2us.
        ADCON0bits.CHS = PIN_A0;	// Select analog input - AN0
        ADCON0bits.ADON = 1;    	// Turn on the ADC
        
    // configure RA0 as analog AN0
    TRISA = 1;
    ANSELbits.ANS0 = 1; 
    ADCON0bits.ADCS = 0b10; // clock / 32
        
    // PWM setup
    // The PWM mode generates a Pulse-Width Modulated signal on the CCP1 pin. 
    // The duty cycle, period and resolution are determined by the following registers:
    // PR2, T2CON, CCPR1L, CCP1CON
    // In Pulse-Width Modulation (PWM) mode, the CCP module produces up to a 10-bit resolution 
    // PWM output on the CCP1 pin. Since the CCP1 pin is multiplexed with the PORT data latch, 
    // the TRIS for that pin must be cleared to enable the CCP1 pin output driver.
    // The PWM period is specified by the PR2 register of Timer2.
    /* 
	 * -------------------CCP1CON----------------------------------
     * Bit#:  ---7----6----5------4-----3------2------1------0-----
     *        -|P1M1|P1M0|DC1B1|DC1B0|CCP1M3|CCP1M2|CCP1M1|CCP1M0|-
     * ------------------------------------------------------------
     * P1M<1:0>: PWM Output Configuration bits
     * DC1B<1:0>: PWM Duty Cycle Least Significant bits
     * CCP1M<3:0>: ECCP Mode Select bits
     * 
     * -------------------PSTRCON----------------------------------
     * Bit#:  ---7----6---5----4------3----2----1----0-------------
     *        -|---|---|----|STRSYNC|STRD|STRC|STRB|STRA|----------
     * ------------------------------------------------------------
     * STRSYNC  - Steering Sync bit
     * STRD     - Steering Enable bit D
     * STRC     - Steering Enable bit C
     * STRB     - Steering Enable bit B
     * STRA     - Steering Enable bit A
    */
        // Set the PWM period by loading the PR2 register.
        PR2 = 0x40;                     // this combined with a TMR2 prescale of 16 gives us a PWM frequency of just under 2kHz
        //PR2 = 0x65;                   // Frequency: 4.90 kHz
        //PSTRCON = 0b00000100;         // Enable Pulse Steering on P1C (RC3)
        PSTRCON = 0b00011110;           // set up pulse steering mode on port D pins
        
    // Configure the CCP module for the PWM mode by loading the CCPxCON register with the appropriate values.
        //CCP1CON = 0b00001100;         // Single PWM mode; P1A, P1C active-high; P1B, P1D active-high
        CCP1CONbits.P1M = 0b00;         // Single output mode
        CCP1CONbits.CCP1M = 0b1100;     // ECCP Mode PWM P1A-D active high
        CCP1CONbits.DC1B = 0x80 & 0b11; // Set the PWM duty cycle by loading the CCPRxL register and DCxB bits of the CCPxCON register.
        CCPR1L = 0x80 >> 2;             // 0x80 is approx 50% duty cycle... I think!
        //CCPR1L = 0;                   // Start with zero Duty Cycle
 
    // Configure and start Timer2:
        //T2CON = 0b00000101;         // Postscale: 1:1, Timer2=On, Prescale = 1:4
        //TMR2 = 0;                   // Start with zero Counter
        PIR1bits.TMR2IF = 0;            // Clear the TMR2IF interrupt flag bit of the PIR1 register.
        T2CONbits.T2CKPS = 0b10;/*16*/  //=16  Set the Timer2 prescale value by loading the T2CKPS bits of the T2CON register.
        T2CONbits.TMR2ON = 1;           // Enable Timer2 by setting the TMR2ON bit of the T2CON register.
 
    // Enable PWM output after a new PWM cycle has started:
        while(PIR1bits.TMR2IF == 0){} // Wait until Timer2 overflows(TMR2IF bit of the PIR1 register is set).
    
    PORTD = 0b00010000;// switch on one of the adjacent LEDs for brightness comparison
}

uint16_t ADC_GetConversion()
{
    __delay_us(ACQ_US_DELAY);					// Acquisition time delay
    ADCON0bits.GO_nDONE = 1;					// Start the conversion
    while (ADCON0bits.GO_nDONE);				// Wait for the conversion to finish
    //return ((uint16_t)((ADRESH << 8) + ADRESL));// Conversion finished, return the result
    return ADRESH;                              // read an 8-bit value
}


void main()
{
    OSCCON=0x70;          // Select 8 Mhz internal clock
    
    system_init(); 
  
    while(1)
    {
        uint16_t duty = ADC_GetConversion();
 
        // set the new duty cycle
        CCP1CONbits.DC1B = duty & 0b11; // LSB
        CCPR1L = duty >> 2;             // MSB
    }
}