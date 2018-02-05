/*
 * File:   main_timer.c
 * Author: akirik
 *
 * Created on January 24, 2016, 12:12 PM
 * 
 * Using Timer 2
 * 
 *  Board connection (PICKit 44-Pin Demo Board; PIC16F887):
 *   PIN                	Module                         				  
 * -------------------------------------------                        
 *  RD0          			LED
 * 
 */

// PIC16F887 Configuration Bit Settings
// 'C' source line config statements
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       //!!! Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
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

void system_init()
{
    OSCCON=0b0100000;          // Select 250 kHz internal clock
    
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
		
		// PORT registers (hold the current digital state of the digital I/O)
			PORTA = 0x00;         // Set PORTA all 0
			PORTB = 0x00;         // Set PORTB all 0
			PORTC = 0x00;         // Set PORTC all 0
            PORTD = 0x00;         // Set PORTD all 0
            PORTE = 0x00;         // Set PORTE all 0
        
	// Timer Setup - Timer 0
    /* 
	 * -------------------T2CON----------------------------------------------
     * Bit#:  ---7----6-------5-------4-------3-------2------1-------0-------
     * :      -|---|TOUTPS3|TOUTPS2|TOUTPS1|TOUTPS0|TMR2ON|T2CKPS1|T2CKPS0|--
     * ----------------------------------------------------------------------
        TOUTPS<3:0>: Timer2 Output Postscaler Select bits
            0000  1:1 
            0001  1:2 
            0010  1:3 
            0011  1:4 
            0100  1:5 
            0101  1:6 
            0110  1:7 
            0111  1:8 
            1000  1:9 
            1001  1:10 
            1010  1:11 
            1011  1:12 
            1100  1:13 
            1101  1:14 
            1110  1:15 
            1111  1:16 
        TMR2ON: Timer2 On bit
        T2CKPS<1:0>: Timer2 Clock Prescale Select bits
            00  1:1
            01  1:4
            1x  1:16
    
     * A prescaler is a circuit that reduces the frequency of a clock using integer division. 
     *  The prescaler can be set anywhere from 1:1 to 1:16 for Timer 2.
     *  The clock we are slowing down is NOT the system clock Fosc (250 kHz as in here). 
     *  It's the system's instruction clock Fcy, which is always Fosc/4.
     *  The postscaler has postscale options of 1:1 to 1:16 for Timer 2. 
     *  The output of the Timer2 postscaler is used to set the TMR2IF interrupt flag bit in the PIR1 register.
     *  The timer expires when the TMR2 register rolls over. 
     *  The TMR2 register is an 8 bit register, therefore it will roll over after 256 counts.
     *  Rollover Frequency = Fosc / (4 * prescaler * 256 * postscaler)
     *  In following case it would be 1.90 Hz or 0.52 seconds per rollover.
    */
		TMR2 = 0;                   // Start with zero Counter
        T2CON = 0b01110111;         // Postscaler: 1:8, Timer2=On, Prescaler: 1:16
}

void main(void) 
{
    system_init();
    
    while(1)
    {        
        while(PIR1bits.TMR2IF == 0){} // Wait until Timer 2 overflows(TMR2IF bit of the PIR1 register is set).

        PORTDbits.RD0 = ~PORTDbits.RD0; // Toggle the LED
		
        PIR1bits.TMR2IF = 0;        // Clear the Timer 2 interrupt flag
        TMR2 = 0;                   // Load a value of 2 into the timer

    }
    
  return;
}
