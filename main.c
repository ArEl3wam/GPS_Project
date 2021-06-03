#include "tm4c123gh6pm.h"
#include "stdint.h"

//Signatures
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

void LED_init(void){
}

// 7-segment-display functions
uint8_t decimal_to_BCD(uint8_t num){
	return 0;
}

void dist_to_display(uint16_t dist){
	
}

// LEDs function
void turn_LED_100(uint16_t dist){

}