#include "tm4c123gh6pm.h"
#include "stdint.h"
#include <stdlib.h>


//Signatures
void SSD_init(void);
void LED_init(void);
void dist_to_display(uint16_t dist);

uint8_t latitude[20];
uint8_t longitude[20];

void UART_init(void){
SYSCTL_RCGCUART_R |=0x0020;  // enable clock for UART5
SYSCTL_RCGCGPIO_R |=0x0010;   // activate port E
 while ((SYSCTL_RCGCUART_R&0x0020)==0){};
 while ((SYSCTL_PRGPIO_R&0x0010)==0){};
	 
	 //UART5 initialization
UART5_CTL_R =0;      //disable UART
UART5_IBRD_R=104;
// IBRD=int(1000000/9600)=int(104.16667)	 
UART5_FBRD_R=11;
//FBRD=round(0.16667*64)=11;
 UART5_LCRH_R |=	0x0070 ; //8-bit length ,enable FIFO
 UART5_CTL_R |=0x0201;  //enable RXE,URT
	 
//portE initailization
	 GPIO_PORTE_DIR_R &= ~0x10;
	 GPIO_PORTE_CR_R |= 0x10;        //unlock commit register
	 GPIO_PORTE_AFSEL_R |= 0x10;    //use PE4 alternating function
	 GPIO_PORTE_PCTL_R |= 0x10000;  // configure PE4 for UART
	 GPIO_PORTE_DEN_R |= 0x10;      //set PE4 as digital
	 GPIO_PORTE_AMSEL_R &= ~0x10;   //turn off analoge function
	  
}



char UART5_Receiver(void)  
{
    char data;
	  while((UART5_FR_R&0x10)!=0){} //wait until Rx buffer is not full 
    data = UART5_DR_R ;  	//before giving it another byte 
    return (unsigned char) data; 
}






void GPS_Coordinates()
{
	uint8_t c;
	uint8_t i = 0;
	double temp;

	c = UART5_Receiver();

	if(c == '$')
	{
		c = UART5_Receiver();
		if(c == 'G')
		{
			c = UART5_Receiver();
			if(c == 'P')
			{
				c = UART5_Receiver();
				if(c == 'G')
				{
					c = UART5_Receiver();
					if(c == 'L')
					{
						c = UART5_Receiver();
						if(c == 'L')
						{
							c = UART5_Receiver(); /* For comma */
							c = UART5_Receiver(); /* For 3 */
							c = UART5_Receiver(); /* For 0 */

							c = UART5_Receiver();
							while(c != ',')
							{
								latitude[i] = c;
								i++;
								c = UART5_Receiver();
							}
							latitude[i] = '\0';

							temp = atof(latitude);
							temp /= 60;
							temp += 30;
							sprintf(latitude, "%.8f", temp);

							c = UART5_Receiver(); /* for N */
							c = UART5_Receiver(); /* for , */
							c = UART5_Receiver(); /* for 3 */
							c = UART5_Receiver(); /* for 1 */

							/* longitude */
							i=0;
							c = UART5_Receiver();
							while(c != ',')
							{
								longitude[i] = c;
								i++;
								c = UART5_Receiver();
							}
							longitude[i] = '\0';

							temp = atof(longitude);
							temp /= 60;
							temp += 31;
							sprintf(longitude, "%.8f", temp);
						}
					}
				}
			}
		}

	}
}






void SystemInit(void){
	LED_init();
}
int main (void)
{	
	while (1) 
		{  
			UART_init();
			GPS_Coordinates();
			
			GPIO_PORTF_DATA_R |= 0x02; // turn on red LED
		}
}

//initialization functions

// 1st SSD will take pins PD0 - PD6  			HUNDREDS
// 2nd SSD will take pins PB0 - PB6  			TENS
// 3rd SSD will take pins PA2 - pA7,PD7  	ONES
void SSD_init(void){
}

void LED_init(void){SYSCTL_RCGCGPIO_R |= 0x20;
    while((SYSCTL_PRGPIO_R & 0x20) == 0) ;

    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    GPIO_PORTF_CR_R |= 0x0E;                    //UNCLOCKING PORT F

    GPIO_PORTF_DIR_R |= 0x0E;
    GPIO_PORTF_DEN_R |= 0x0E;
    GPIO_PORTF_AMSEL_R &= ~0x0E;            //SET PINS 1,2,3 AS DIGITAL OUTPUT

    GPIO_PORTF_AFSEL_R &= ~0x0E;
    GPIO_PORTF_PCTL_R &= ~0x0000FFF0; //SET PINS 1,2,3 AS GPIOs

}

// 7-segment-display functions
uint8_t decimal_to_BCD(uint8_t num){switch(num){
        case 0:
            return 0b00111111;
        case 1:
            return 0b00000110;
        case 2:
            return 0b00011011;
        case 3:
            return 0b01001111;
        case 4:
            return 0b01100110;
        case 5:
            return 0b01101101;
        case 6:
            return 0b01111101;
        case 7:
            return 0b00000111;
        case 8:
            return 0b01111111;
    }
    return 0b01101111;
	
}

void dist_to_display(uint16_t dist){
	
}

// LEDs function
void turn_LED_100(uint16_t dist){

}