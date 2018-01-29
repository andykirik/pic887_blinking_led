/*
 * File:   main_timer.c
 * Author: akirik
 *
 * Created on January 24, 2016, 12:12 PM
 * 
 * Using Timer 0
 * (Note, Watch Dog Timer should be disabled)
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
    //OSCCON=0b1110000;          // Select 8 MHz internal clock
    OSCCON=0b0100000;          // Select 250 kHz internal clock
    //OSCCON=0b0000000;          // Select 31 kHz internal clock
    
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
	 * -------------------OPTION_REG-------------------------
     * Bit#:  ---7------6------5------4-----3---2---1---0----
     * :      -|WPUEN|INTEDG|TMR0CS|TMR0SE|PSA|PS2|PS1|PS0|--
     * ------------------------------------------------------
        WPUEN: Weak Pull-Up Enable bit
            1 = All weak pull-ups are disabled (except MCLR, if it is enabled)
            0 = Weak pull-ups are enabled by individual WPUx latch values
        INTEDG: Interrupt Edge Select bit
            1 = Interrupt on rising edge of INT pin
            0 = Interrupt on falling edge of INT pin
        TMR0CS: Timer0 Clock Source Select bit
            1 = Transition on T0CKI pin
            0 = Internal instruction cycle clock (FOSC/4)
        TMR0SE: Timer0 Source Edge Select bit
            1 = Increment on high-to-low transition on T0CKI pin
            0 = Increment on low-to-high transition on T0CKI pin
        PSA: Prescaler Assignment bit
            1 = Prescaler is not assigned to the Timer0 module
            0 = Prescaler is assigned to the Timer0 module
        PS<2:0>: Prescaler Rate Select bits
            000     1 : 2
            001     1 : 4
            010     1 : 8
            011     1 : 16
            100     1 : 32
            101     1 : 64
            110     1 : 128
            111     1 : 256
    
     * A prescaler is a circuit that reduces the frequency of a clock using integer division. 
     *  The prescaler can be set anywhere from 1:2 to 1:256 for Timer 0 (8 bit prescaler, shared with WDT).
     *  The clock which is slowing is NOT the system clock Fosc (250 kHz as in here). 
     *  It's the system's instruction clock Fcy, which is always Fosc/4.
     *  The timer expires when the TMR0 register rolls over. 
     *  The TMR0 register is an 8 bit register, therefore it will roll over after 256 counts.
     *  Rollover Frequency = Fosc / (4 * prescaler * 256)
     *  In following case it would be 0.95 Hz or 1.04 seconds per rollover.
    */
		OPTION_REGbits.PSA = 0; 	// Prescaler assigned to Timer 0
		OPTION_REGbits.PS = 0b111;  // Set the prescaler to 1:256
		OPTION_REGbits.T0CS = 0;    // Use the instruction clock (Fcy/4) as the timer clock. 
}

void main(void) 
{
    system_init();
    
    while(1)
    {        
        while(INTCONbits.T0IF == 0); // Wait for the interrupt to occur. This happens when the TMR0 register rolls over.

        PORTDbits.RD0 = ~PORTDbits.RD0; // Toggle the LED
		
        INTCONbits.T0IF = 0;        // Clear the Timer 0 interrupt flag
        TMR0 = 0;                   // Load a value of 0 into the timer

    }
    
  return;
}
