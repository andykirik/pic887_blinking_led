/*
 * File:   main_timer.c
 * Author: akirik
 *
 * Created on January 24, 2016, 12:12 PM
 * 
 * Using Timer 1
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
        
	// Timer Setup - Timer 1
    /* The Timer1 module is a 16-bit timer/counter.
     * -------------------T1CON---------------------------------------------
     * Bit#:  ----7------6-------5-------4-------3-------2------1-------0---
     * ANS:   -|T1GINV|TMR1GE|T1CKPS1|T1CKPS0|T1OSCEN|T1SYNC|TMR1CS|TMR1ON|-
     * ---------------------------------------------------------------------
     * T1GINV: Timer1 Gate Invert bit
     * TMR1GE: Timer1 Gate Enable bit
     * T1CKPS<1:0>: Timer1 Input Clock Prescale Select bits
     * T1OSCEN: LP Oscillator Enable Control bit
     * T1SYNC: Timer1 External Clock Input Synchronization Control bit
     * TMR1CS: Timer1 Clock Source Select bit
     * TMR1ON: Timer1 On bit
    
     * A prescaler is a circuit that reduces the frequency of a clock using integer division. 
     *  The prescaler can be set anywhere from 1:1 to 1:8 for Timer 1.
     *  The clock we are slowing down is NOT the system clock Fosc (250 kHz as in here). 
     *  It's the system's instruction clock Fcy, which is always Fosc/4.
     *  The timer expires when the TMR2 register rolls over. 
     *  The TMR1 register is an 16 bit register, therefore it will roll over after 65535 counts.
     *  16-bit timer/counter register pair (TMR1H:TMR1L)
     *  Rollover Frequency = Fosc / (4 * prescaler * 65535)
     *  In following case it would be 0.95 Hz or 1.04 seconds per rollover.
    */
		TMR1 = 0;                   // Start with zero Counter
        T1CON = 0b00001001;         // Internal Clock (Fosc/4), Prescaler: 1:1, Timer1=On, 
}

void main(void) 
{
    system_init();
    
    while(1)
    {        
        while(PIR1bits.TMR1IF == 0){} // Wait until Timer 1 overflows(TMR1IF bit of the PIR1 register is set).

        PORTDbits.RD0 = ~PORTDbits.RD0; // Toggle the LED
		
        PIR1bits.TMR1IF = 0;        // Clear the Timer 1 interrupt flag
        TMR1 = 0;                   // Load a value of 1 into the timer

    }
    
  return;
}
