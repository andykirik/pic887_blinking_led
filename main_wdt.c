/*
 * File:   main.c
 * Author: akirik
 *
 * Created on January 24, 2016, 12:12 PM
 * 
 * Blinking LED
 * Digital I/O
 * 
 *  Board connection (PICKit 44-Pin Demo Board; PIC16F887):
 *   PIN                	Module                         				  
 * -------------------------------------------                        
 *  RD0          			LED
 *  RD1          			LED
 *  RD2          			LED
 *  RD3          			LED
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
#pragma config WDTE = ON        // Watchdog Timer Enable bit (WDT enabled)
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
    OSCCON=0x70;                // Select 8 Mhz internal clock    
    OPTION_REG = 0b00001111;    // Assign 1:128 prescaler to WDT (~2 sec)
	
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
}

void main(void) 
{
    system_init();
   
    PORTDbits.RD0 = 1;
    PORTDbits.RD1 = 0;
    PORTDbits.RD2 = 1;
    PORTDbits.RD3 = 0;

    //Enable Watchdog Timer
    WDTCONbits.SWDTEN = 1;

    SLEEP();

    PORTDbits.RD0 = 0;
    PORTDbits.RD1 = 1;
    PORTDbits.RD2 = 0;
    PORTDbits.RD3 = 1;

    WDTCONbits.SWDTEN = 0;
        
    while(1)
    {}
    
	return;
}
