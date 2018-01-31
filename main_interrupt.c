/* 
 * File:   main_timer_interrupt.c
 * Author: akirik
 *
 * Created on January 24, 2016, 12:12 PM
 * 
 * Interrupt
 * Use External Interrupt.
 * On PIC16F887 external interrupt is pin RB0/AN12/INT
 * LED 7 blinks once a second
 * LED 3 changes its state on button press
 * 
 *  Board connection (PICKit 44-Pin Demo Board; PIC16F887):
 *   PIN                	Module                         				  
 * -------------------------------------------                        
 *  RD3          			LED
 *  RD7          			LED
 * 
 *  RB0/AN12/INT (SW1)      BUTTON    
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

void system_init()
{
    OSCCON=0x70;          // Select 8 Mhz internal clock
    
	// I/O	
		// ANSELx registers
			ANSEL = 0x00;         // Set PORT ANS0 to ANS7 as Digital I/O
			ANSELH = 0x00;        // Set PORT ANS8 to ANS11 as Digital I/O
	  
		// TRISx registers (This register specifies the data direction of each pin)
			TRISA = 0x00;         // Set All on PORTB as Output
			TRISB = 0xFF;         // Set All on PORTB as Input    
			TRISC = 0x00;         // Set All on PORTC as Output    
            TRISD = 0x00;         // Set All on PORTD as Output   
            TRISE = 0x00;         // Set All on PORTE as Output   
		
		// PORT registers (hold the current digital state of the digital I/O)
			PORTA = 0x00;         // Set PORTA all 0
			PORTB = 0x00;         // Set PORTB all 0
			PORTC = 0x00;         // Set PORTC all 0
            PORTD = 0x00;         // Set PORTD all 0
            PORTE = 0x00;         // Set PORTE all 0
        
	// Interrupt setup
    /* 
     * -------------------INTCON----------------------------
     * Bit#:  ----7----6----5----4----3----2----1----0------
     *        --|GIE|PEIE|T0IE|INTE|RBIE|T0IF|INTF|RBIF|--
     * -----------------------------------------------------
     * GIE      - global interrupt enable bit
     * PEIE     - peripheral interrupt enable bit
     * T0IE     - Timer 0 overflow interrupt enable bit
     * INTE     - external interrupt enable bit
     * RBIE     - port B change interrupt enable bit
     * T0IF     - Timer 0 overflow interrupt flag bit
     * INTF     - external interrupt flag bit
     * RBIF     - port B change interrupt flag bit
     * 
     * -------------------OPTION_REG----------------------
     * Bit#:  ----7------6-----5----4----3---2---1---0----
     *        --|RABPU|INTEDG|T0CS|T0SE|PSA|PS2|PS1|PS0|--
     * ---------------------------------------------------
     * INTEDG   - interrupt edge select bit
     * 
     * In order to initialize the RB0/AN12/INT interrupt, the following operations must take place:
     *   1. Port B, pin 0 (RB0), must be initialized as Input.
     *   2. The interrupt source must be set to take place either on the falling or the rising edge of
     *      the signal using INTEDG bit of OPTION_REG.
     *   3. The External Interrupt flag (INTF in the INTCON Register) must be cleared.
     *   4. The External Interrupt on RB0/AN12/INT must be enabled by setting the INTE bit in the INTCON
     *      Register.
     *   5. Global interrupts must be enabled by setting the GIE bit in the INTCON Register.
    */
        OPTION_REGbits.INTEDG = 1;  // Interrupt on the rising edge
        INTCONbits.INTF = 0;        // Reset the External Interrupt flag
		INTCONbits.INTE = 1;        // Set the External Interrupt Enabled
		INTCONbits.GIE = 1;         // Set the Global Interrupt Enabled
}

/* 
 * The PIC16F887 can only have one Interrupt Service Routine. 
 * Compiler should know which function is the interrupt handler. 
 * This is done by declaring the function with 'interrupt' prefix:
 */
void interrupt isr()
{
    if(INTCONbits.INTF == 1)
    {
        PORTDbits.RD3 = ~PORTDbits.RD3; // Toggle the LED
        __delay_ms(200);                // Key debounce time
        
        INTCONbits.INTF = 0;            // Clear the External Interrupt flag
    }
    //else if(...)
}

void main(void) 
{
    system_init();
    
    while(1)
    {        
        PORTDbits.RD7 = ~PORTDbits.RD7;	// Toggle the LED
        __delay_ms(1000);   			// sleep 1 seconds

    }
    
  return;
}
