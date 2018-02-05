/*
 * File:   main_timer_interrupt.c
 * Author: akirik
 *
 * Created on January 24, 2016, 12:12 PM
 * 
 * Using Timer 0 with interrupt
 * (Note, Watch Dog Timer should be disabled)
 * 
 *  Board connection (PICKit 44-Pin Demo Board; PIC16F887):
 *   PIN                	Module                         				  
 * -------------------------------------------                        
 *  RD0          			LED
 * 
 */

/* The __delay_ms() function is provided by XC8. 
It requires you define _XTAL_FREQ as the frequency of your system clock. 
The compiler then uses that value to calculate how many cycles are required to give the requested delay. 
There is also __delay_us() for microseconds and _delay() to delay for a specific number of clock cycles. 
Note that __delay_ms() and __delay_us() begin with a double underscore whereas _delay() 
begins with a single underscore.
*/
#define _XTAL_FREQ 250000

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

#define TIMER_RESET_VALUE 240 // To set up the timer for a period of 1 ms (timerPeriod)
                            // Calculated by the formula:
                            // TMR0 = 256 - ((timerPeriod * Fosc) / (4 * prescaler)) + x
                            // TMR1 = 65536 - ((timerPeriod * Fosc) / (4 * prescaler)) + x

                            // In following case,   timerPeriod = 0.001s
                            //                      Fosc = 250,000
                            //                      prescaler = 4
                            //                      x = 0 because prescaler is > 2

                            // TMR0 = 256 - (0.001 * 250000) / (4 * 4) = 240

int delayTime = 0;          // store the time that has elapsed

void system_init()
{
    OSCCONbits.IRCF = 0b010;       // Select 250 kHz internal clock
    
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
     *  The prescaler can be set anywhere from 1:2 to 1:256 for Timer 0.
     *  The clock we are slowing down is NOT the system clock Fosc (250 kHz as in here). 
     *  It's the system's instruction clock Fcy, which is always Fosc/4.
     *  The timer expires when the TMR0 register rolls over. 
     *  The TMR0 register is an 8bit register, therefore it will roll over after 256 counts.
     *  Rollover Frequency = Fosc / (4 * prescaler * 256)
     *  In following case it would be 0.95 Hz or 1.04 seconds per rollover.
    */
		OPTION_REGbits.PSA = 0; 	// Prescaler assigned to Timer 0 
        OPTION_REGbits.PS = 0b001;  // Set the prescaler to 1:4
		OPTION_REGbits.T0CS = 0;    // Use the instruction clock (Fcy/4) as the timer clock. 
									//   Other option is an external oscillator or clock on the T0CKI pin.
        INTCONbits.T0IE = 1;        // Enable the Timer 0 interrupt
		INTCONbits.T0IF = 0;        // Clear the Timer 0 interrupt flag
		TMR0 = TIMER_RESET_VALUE;   // Load the starting value back into the timer

	// Interrupt setup
		INTCONbits.T0IE = 1;        // Enable the Timer 0 interrupt
		INTCONbits.GIE = 1;         // Set the Global Interrupt Enable
}

/* 
 * The PIC16F887 can only have one Interrupt Service Routine. 
 * Compiler should know which function is the interrupt handler. 
 * This is done by declaring the function with 'interrupt' prefix:
 */
void interrupt isr()
{
    INTCONbits.T0IF = 0;    // Clear the Timer 0 interrupt flag
    TMR0 = TIMER_RESET_VALUE;   // Load the starting value back into the timer
    
    if(++delayTime >= 5000) // 5 seconds has elapsed
    {
        delayTime = 0;
        PORTDbits.RD3 = ~PORTDbits.RD3; // Toggle the LED
    }
}

void main(void) 
{
    system_init();
    
    while(1)
    {        
        PORTDbits.RD0 = ~PORTDbits.RD0;	// Toggle the LED
        __delay_ms(1000);   			// sleep 1 seconds

    }
    
  return;
}
