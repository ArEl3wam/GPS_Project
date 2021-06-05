#include "tm4c123gh6pm.h"
#include "stdint.h"

//signatures
void SSD_init(void);
void LED_init(void);
void dist_to_display(uint16_t dist);

void SystemInit(void){
	LED_init();
}
int main (void)
{	
	while (1) 
		{  
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

void delay_milli(int n){
int i,j;
for(i=0;i<n;i++){
for(j=0;j<3180;j++)
{}
}
}
 
void delay_micro(int n){
int i,j;
for(i=0;i<n;i++){
for(j=0;j<3;j++)
{}
}
}