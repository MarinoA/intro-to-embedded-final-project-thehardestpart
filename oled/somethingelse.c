/*
 * File: main.c
 * Author: Will Cronin
 * Board Used: MSP430G2553
 */

#include <msp430.h>

void somethingelse(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer
  P1DIR |= BIT0;                            // Set P1.0 to output direction
  P1IE |=  BIT3;                            // P1.3 interrupt enabled
  P1IES |= BIT3;                            // P1.3 falling edge
  P1REN |= BIT3;                            // Enable Pull Up on SW2 (P1.3)
  P1IFG &= ~BIT3;                           // P1.3 IFG cleared

  DCOCTL = 0;                               // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
  DCOCTL = CALDCO_1MHZ;
  P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
  P1SEL2 = BIT1 + BIT2 ;                    // P1.1 = RXD, P1.2=TXD

  //UCA0CTL1 |= UCSWRST;        //State machine reset + small clock initialization
  UCA0CTL1 |= UCSSEL_2;       //SMCLK
  UCA0BR0 = 104;                //9600 baud
  UCA0BR1 = 0;                //9600 baud
  UCA0MCTL = UCBRS0;
  UCA0CTL1 &= ~UCSWRST;       // Initialize USCI state machine
  IE2 |= UCA0RXIE;           // Enable USCI_A0 RX interrupt
}
