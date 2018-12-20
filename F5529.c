#include <intrinsics.h>
#include <stdint.h>
#include <msp430.h> 

#define TRIGGER_PIN BIT1   // P6.1
#define ECHO_PIN BIT3  // P1.3
#define LED_PIN BIT0   // P1.0
#define DISTANCE_THRESHOLD 60  // cm
#define MEASURE_INTERVAL 2048  // ~250 ms
volatile float distance; //current distance
volatile _Bool ON; //bool to check if user is present or not


void triggerMeasurement() {
    static volatile int trigWait;

    // Start trigger
    P6OUT |= TRIGGER_PIN;

    // Wait a small amount of time with trigger high, > 10us required (~10 clock cycles at 1MHz MCLK)
    for (trigWait = 0; trigWait < 12; trigWait++) {}

    // End trigger
    P6OUT &= ~TRIGGER_PIN;
}


int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	P4SEL |= BIT4 + BIT5;                       //UART A0
    P3SEL |= BIT3 + BIT4;                       // P3.3,4 = USCI_A1 TXD/RXD

    UCA1CTL1 |= UCSWRST;                      // **Put state machine in reset**
    UCA1CTL1 |= UCSSEL_2;                     // SMCLK
    UCA1BR0 = 6;                            // 1MHz 9600 (see User's Guide)
    UCA1BR1 = 0;                              // 1MHz 9600
    UCA1MCTL |= UCBRS_0 + UCBRF_13 + UCOS16;   // Modulation UCBRSx=1, UCBRFx=0
    UCA1CTL1 &= ~UCSWRST;                   // **Initialize USCI state machine**
    UCA1IE |= UCRXIE;                         // Enable USCI_A1 RX interrupt

    UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 6;                            // 1MHz 9600 (see User's Guide)
    UCA0BR1 = 0;                              // 1MHz 9600
    UCA0MCTL |= UCBRS_0 + UCBRF_13 + UCOS16;   // Modulation UCBRSx=1, UCBRFx=0
    UCA0CTL1 &= ~UCSWRST;                   // **Initialize USCI state machine**
    UCA0IE |= UCRXIE;                         // Enable USCI_A1 RX interrupt

    __bis_SR_register(GIE);       // Enter LPM0, interrupts enabled

    //sensor
    // Configure trigger pin, low to start
    P6DIR |= TRIGGER_PIN;
    P6OUT &= ~TRIGGER_PIN;

    // Configure LED, off to start
    P1DIR |= LED_PIN;
    P1OUT &= ~LED_PIN;

    // Configure echo pin as capture input to TA0CCR2
    P1DIR &= ~ECHO_PIN;
    P1SEL |= ECHO_PIN;

    // Set up TA0 to capture in CCR2 on both edges from P1.3 (echo pin)
    TA0CCTL2 = CM_3 | CCIS_0 | SCS | CAP | CCIE;

    // Set up TA0 to compare CCR0 (measure interval)
    TA0CCR0 = MEASURE_INTERVAL;
    TA0CCTL0 = CCIE;

    // Set up TA0 with ACLK / 4 = 8192 Hz
    TA0CTL = TASSEL__ACLK | ID__4 | MC__CONTINUOUS | TACLR;

    uint16_t lastCount = 0;
    uint32_t distance = 0;

    while(1)
    {
        triggerMeasurement();

        // Wait for echo start
        __low_power_mode_1();

        lastCount = TA0CCR2;

        // Wait for echo end
        __low_power_mode_1();

        distance = TA0CCR2 - lastCount;
        distance *= 34000; //unit conversion
        distance >>= 14;  // division by 16384 (2 ^ 14)


        if (distance <= DISTANCE_THRESHOLD)
        {
            // Turn on LED
            P1OUT |= LED_PIN;
            ON = 1; //tell MSP user is present
        }
        else
        {
            // Turn off LED
            P1OUT &= ~LED_PIN;
            ON = 0; //tell MSP user is not present
        }

        // Wait for the next measure interval tick
       __low_power_mode_1();
    }

}


#pragma vector=USCI_A1_VECTOR //P4.4 TX, P4.5 RX
__interrupt void USCI_A1_ISR(void)
{

    switch (__even_in_range(UCA1IV, 4))
    {
    case 0:
        break;                             // Vector 0 - no interrupt
    case 2:                                   // Vector 2 - RXIFG
        while (!(UCA1IFG & UCTXIFG));  // USCI_A1 TX buffer ready?
        if (ON) //if user is present
         {
            UCA0TXBUF = UCA1RXBUF; //send message to other UART peripheral
         }
         else
         {
             UCA1TXBUF = 0x09; //send user not found character
         }

    default:
        break;
    }
}



#pragma vector=USCI_A0_VECTOR //P3.3 TX, P3.4 RX
__interrupt void USCI_A0_ISR(void)
{

    switch (__even_in_range(UCA0IV, 4))
    {
    case 0:
        break;                             // Vector 0 - no interrupt
    case 2:                                   // Vector 2 - RXIFG
        while (!(UCA0IFG & UCTXIFG));  // USCI_A1 TX buffer ready?
        UCA1TXBUF = UCA0RXBUF; //echo receive to other UART peripheral
    default:
        break;
    }
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR (void) {
    // Measure interval tick
    __low_power_mode_off_on_exit();
    TA0CCR0 += MEASURE_INTERVAL;
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR (void) {
    // Echo pin state toggled
    __low_power_mode_off_on_exit();
    TA0IV = 0;
}
