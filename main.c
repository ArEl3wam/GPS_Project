#include "tm4c123gh6pm.h"
#include "stdint.h"
void SystemInit(void){
	SYSCTL_RCGCGPIO_R |= 0x20; // Enable clock for PORTF
	while((SYSCTL_PRGPIO_R & 0x20) ==0) ;
	GPIO_PORTF_LOCK_R = 0x4C4F434B;
	GPIO_PORTF_CR_R |= 0xE;			// Unlocking PORTF PIN 1 2 3;
	
	GPIO_PORTF_DEN_R  |= 0x0E; 
	GPIO_PORTF_AMSEL_R &= ~0x0E;	
	GPIO_PORTF_DIR_R  |= 0x0E;  // Configure PORTF Pin1, 2 and 3 digital output pins

	
	GPIO_PORTF_AFSEL_R &= ~0x0E;
	GPIO_PORTF_PCTL_R &= ~0x0000FFF0;
}
int main (void)
{	
	while (1) 
		{  
			GPIO_PORTF_DATA_R |= 0x02; // turn on red LED
		}
}

