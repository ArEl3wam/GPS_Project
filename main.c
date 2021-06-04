#include "tm4c123gh6pm.h"
#include "stdint.h"

//signatures
void SSD_init(void);
void LED_init(void);
void dist_to_display(uint16_t dist);


//LCD FUNCTIONS
void lcd_command(uint8_t comm);
void lcd_data(uint8_t data);
void delay(void);
void lcd_display(uint16_t data,uint8_t pos);
void lcd_init(void);

void SystemInit(void){
	LED_init();
	lcd_init();
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

//LCD functions
void lcd_command(uint8_t comm){
	
	GPIO_PORTE_DATA_R &= ~0x03; // RS,R/W => 0  TO SELECT COMMAND AND WRITE
	GPIO_PORTD_DATA_R  = comm;  // put command in the data
	GPIO_PORTE_DATA_R |= 0x04;  // Enable is set 
	delay();
	GPIO_PORTE_DATA_R &= ~0x04;	
}
void lcd_data(uint8_t data){
	
	GPIO_PORTE_DATA_R |= 0x01; 	// RS => 1  TO SELECT Data 
	GPIO_PORTE_DATA_R &= ~0x02; // R/w => 0 TO SELECT WRITE
	
	GPIO_PORTD_DATA_R  = data;  // put command in the data
	GPIO_PORTE_DATA_R |= 0x04;  // ENABLE IS SET TO 1
	delay();
	GPIO_PORTE_DATA_R &= ~0x04;	// ENABLE IS SET TO 0
}
void delay(void){
	int temp =12000;
	while(temp--);
}
void lcd_display(uint16_t data,uint8_t pos){
	uint8_t hundreds = data/100;
	uint8_t tens=data/10 - hundreds*10 ;
	uint8_t ones=data-hundreds*100 - tens*10;
	lcd_command(pos); 
	delay();
	lcd_data(hundreds);
	delay();
	lcd_data(tens);
	delay();
	lcd_data(ones);
	delay();
}
void lcd_init(void){
	SYSCTL_RCGCGPIO_R |= 0x18; // Enable clock for PORTD and PORTE
	while((SYSCTL_PRGPIO_R & 0x18) == 0) ;
	
	GPIO_PORTD_LOCK_R = 0x4C4F434B;
	GPIO_PORTE_LOCK_R = 0x4C4F434B;
	GPIO_PORTD_CR_R |= 0xFF;			// Unlocking PORTD PIN 1...8; 
	GPIO_PORTE_CR_R |= 0x07;			// Unlocking PORTE PIN 0 1 2;
	
	GPIO_PORTD_DEN_R  |= 0xFF; 
	GPIO_PORTD_AMSEL_R &= ~0xFF;	
	GPIO_PORTE_DEN_R  |= 0x07;    
	GPIO_PORTE_AMSEL_R &= ~0x07;	
	
	GPIO_PORTD_DIR_R  |= 0xFF;   // Configure PORTF Pin 1~8 digital output pins
	GPIO_PORTE_DIR_R  &= ~0x07;  // Configure PORTF Pin 0, 1 and 2 digital input pins

	
	GPIO_PORTD_AFSEL_R  = 0x00;
	GPIO_PORTE_AFSEL_R &= ~0x07;
	GPIO_PORTD_PCTL_R   = 0x00000000;
	GPIO_PORTE_PCTL_R  &= ~0x00000FFF;
	 
	lcd_command(0x30); //wake up 
	lcd_command(0x38); //8bit data
	lcd_command(0x01); //clear display
	lcd_command(0x0F); //display on cursor blinking
}