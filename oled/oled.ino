#include <Wire.h>
#include "Font.h"
#include <string.h>
#include "images.h"
#include <msp430.h>
//#include "somethingelse.c"

//extern void somethingelse(void);

#define OLED_Write_Address 0x3C

int i = 0;

void OLED_Data(char *DATA) /* Function for sending data to OLED */
{
  int len = strlen(DATA);
  for (int g=0; g<len; g++)
  {    
    for (int index=0; index<5; index++)
    {     
      Wire.beginTransmission(OLED_Write_Address); /* Begin transmission to slave device */
	  /* Queue data to be transmitted */
      Wire.write(0x40); /* For Data Transmission, C = 0 and D/C = 1 */ 
      Wire.write(ASCII[DATA[g] - 0x20][index]); 
      Wire.endTransmission(); /* Transmit the queued bytes and end transmission to slave device */ 
    }    
  }
}

void OLED_Command(char DATA) /* Function for sending command to OLED */
{
  Wire.beginTransmission(OLED_Write_Address); /* Begin transmission to slave device */
  /* Queue data to be transmitted */
  Wire.write(0x00); /* For Data Transmission, C = 0 and D/C = 0 */ 
  Wire.write(DATA); 
  Wire.endTransmission(); /* Transmit the queued bytes and end transmission to slave device */
}

void OLED_clear(void) /* Function for clearing OLED */   
{
  OLED_setXY(0x00, 0x7F, 0x00, 0x07); /* Column Start Address 0, Column End Address 127, Page Start Address 0, Page End Address 7  */
  for (int k=0; k<=1023; k++)
  {
    Wire.beginTransmission(OLED_Write_Address); /* Begin transmission to slave device */
	/* Queue data to be transmitted */
    Wire.write(0x40); /* For Data Transmission, C = 0 and D/C = 1 */ 
    Wire.write(0x00); 
    Wire.endTransmission(); /* Transmit the queued bytes and end transmission to slave device */  
  }  
}

void OLED_setXY(char col_start, char col_end, char page_start, char page_end) /* Function for setting cursor for writing data */  
{
  Wire.beginTransmission(OLED_Write_Address); /* Begin transmission to slave device */
  /* Queue data to be transmitted */
  Wire.write(0x00); /* For Data Transmission, C = 0 and D/C = 0 */
  Wire.write(0x21); /* Set Column Start and End Address */ 
  Wire.write(col_start); /* Column Start Address col_start */
  Wire.write(col_end); /* Column End Address col_end */
  Wire.write(0x22); /* Set Page Start and End Address */ 
  Wire.write(page_start); /* Page Start Address page_start */
  Wire.write(page_end); /* Page End Address page_end */
  Wire.endTransmission(); /* Transmit the queued bytes and end transmission to slave device */
}

void OLED_init(void) /* Function for initializing OLED */
{
  OLED_Command(0xAE); /* Entire Display OFF */
  OLED_Command(0xD5); /* Set Display Clock Divide Ratio and Oscillator Frequency */
  OLED_Command(0x80); /* Default Setting for Display Clock Divide Ratio and Oscillator Frequency that is recommended */
  OLED_Command(0xA8); /* Set Multiplex Ratio */
  OLED_Command(0x3F); /* 64 COM lines */
  OLED_Command(0xD3); /* Set display offset */
  OLED_Command(0x00); /* 0 offset */
  OLED_Command(0x40); /* Set first line as the start line of the display */
  OLED_Command(0x8D); /* Charge pump */
  OLED_Command(0x14); /* Enable charge dump during display on */
  OLED_Command(0x20); /* Set memory addressing mode */
  OLED_Command(0x00); /* Horizontal addressing mode */
  OLED_Command(0xA1); /* Set segment remap with column address 127 mapped to segment 0 */
  OLED_Command(0xC8); /* Set com output scan direction, scan from com63 to com 0 */
  OLED_Command(0xDA); /* Set com pins hardware configuration */
  OLED_Command(0x12); /* Alternative com pin configuration, disable com left/right remap */
  OLED_Command(0x81); /* Set contrast control */
  OLED_Command(0x80); /* Set Contrast to 128 */
  OLED_Command(0xD9); /* Set pre-charge period */
  OLED_Command(0xF1); /* Phase 1 period of 15 DCLK, Phase 2 period of 1 DCLK */
  OLED_Command(0xDB); /* Set Vcomh deselect level */
  OLED_Command(0x20); /* Vcomh deselect level ~ 0.77 Vcc */
  OLED_Command(0xA4); /* Entire display ON, resume to RAM content display */
  OLED_Command(0xA6); /* Set Display in Normal Mode, 1 = ON, 0 = OFF */
  OLED_Command(0x2E); /* Deactivate scroll */
  OLED_Command(0xAF); /* Display on in normal mode */
}

void OLED_image(const unsigned char *image_data)   /* Function for sending image data to OLED */
{
  OLED_setXY(0x00, 0x7F, 0x00, 0x07);    
  for (int k=0; k<=1023; k++)
  {    
    Wire.beginTransmission(OLED_Write_Address); /* Begin transmission to slave device */
	/* Queue data to be transmitted */
    Wire.write(0x40); /* For Data Transmission, C = 0 and D/C = 1 */
    Wire.write(image_data[k]);
    Wire.endTransmission(); /* Transmit the queued bytes and end transmission to slave device */ 
  }   
}



void setup() {
  Wire.begin(); /* Initiate wire library and join I2C bus as a master */
  OLED_init(); /* Initialize OLED */
  delay(100);  
  OLED_clear(); /* Clear OLED */
  delay(1000);
  //OLED_image(Launchpad_Logo);
  //delay(2000);
  //OLED_clear();
  //delay(200);
  OLED_setXY(0x31, 0x7F, 0x03, 0x02);
  OLED_Data("Emoji");
  OLED_setXY(0x36, 0x7F, 0x04, 0x03);
  OLED_Data("Sender");
  OLED_setXY(0x31, 0x7F, 0x05, 0x04);
  OLED_Data("3000");
  OLED_setXY(0x00, 0x7F, 0x00, 0x08);
  delay(2000);

  P1DIR |= BIT0;                            // Set P1.0 to output direction
  P1SEL |= 0;
  
  P1DIR &= ~(BIT3);
  P1IE |=  BIT3;                            // P1.3 interrupt enabled
  P1IES |= BIT3;                            // P1.3 falling edge
  P1REN |= BIT3;                            // Enable Pull Up on SW2 (P1.3)
  P1IFG &= ~(BIT3);                           // P1.3 IFG cleared

  P2DIR &= ~(BIT0);
  P2IE |=  BIT0;                            // P2.0 interrupt enabled
  P2IES |= BIT0;                            // P2.0 falling edge
  P2REN |= BIT0;                            // Enable Pull Up on SW2 (P2.0)
  P2IFG &= ~(BIT0);                           // P2.0 IFG cleared
  
  Serial.begin(9600);
}

void loop()
{
  switch(Serial.read())
  {
    case 1:  
      delay(1000);
      OLED_image(Eggplant);
      break;
    case 2: 
      delay(1000);
      OLED_image(XD);
      break;
    case 3:  
      delay(1000);
      OLED_image(MiddleFinger);
      break;
    case 4:  
      delay(1000);
      OLED_image(B);
      break;
    case 5: 
      delay(1000);
      OLED_image(Fire);
      break;
    case 6: 
      delay(1000);
      OLED_image(Heart);
      break;
    case 7:  
      delay(1000);
      OLED_image(Cry);
      break;
    case 8: 
      delay(1000);
      OLED_image(Smile);
      break;
    case 9:  
      delay(1000);
      OLED_setXY(0x31, 0x7F, 0x03, 0x02);
      OLED_Data("Receiver");
      OLED_setXY(0x36, 0x7F, 0x04, 0x03);
      OLED_Data("Not");
      OLED_setXY(0x31, 0x7F, 0x05, 0x04);
      OLED_Data("Found");
      delay(1000);
      OLED_clear();
      break;
    default:
      break;
  }
}


#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{ 

  const unsigned char * image[8] = {Eggplant, XD, MiddleFinger, B, Fire, Heart, Cry, Smile};
  OLED_image(image[i]);
  P1IFG &= ~BIT3;                           // P1.3 IFG cleared
  delay(200);
  if(i < 7)
  {
      i++;
  }
  else
  {
    i = 0;
  }
}

#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{ 
  P1OUT ^= BIT0;

  Serial.write(i);
   
  i = 0;
  
  OLED_clear();
  OLED_setXY(0x31, 0x7F, 0x03, 0x02);
  OLED_Data("Message");
  OLED_setXY(0x36, 0x7F, 0x04, 0x03);
  OLED_Data("Sent");
  
  P2IFG &= ~BIT0;                           // P2.0 IFG cleared
  delay(200);
  
  OLED_clear();
  OLED_setXY(0x31, 0x7F, 0x03, 0x02);
  OLED_Data("Emoji");
  OLED_setXY(0x36, 0x7F, 0x04, 0x03);
  OLED_Data("Sender");
  OLED_setXY(0x31, 0x7F, 0x05, 0x04);
  OLED_Data("3000");
  OLED_setXY(0x00, 0x7F, 0x00, 0x08);

}


/*#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
  rx = (const unsigned char *) UCA0RXBUF;
  P1OUT ^= BIT0;
  OLED_image(rx);
}*/
