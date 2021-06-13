#include "tm4c123gh6pm.h"
#include "stdint.h"
#include "stdio.h"
#include <stdlib.h>
#include <math.h>

#define M_PI 3.14159265358979323846

//Signatures
void SSD_init(void);
void LED_init(void);
void dist_to_display(uint16_t dist);
void GPS_Coordinates(void);
void UART_init(void);
void delay_milli(int n);
void delay_micro(int n);
void CalculateDistance(double newX,double newY );
void RED_LED(void);
void setX(double x);
void setY(double y);
void update_distance(void);
void update_lat_long_average(void);
void reached_pos(double lat, double lon);

	
//LCD FUNCTIONS
void lcd_command(uint8_t comm);
void lcd_data(uint8_t data);
void delay(void);
void lcd_display(uint16_t data);
void lcd_init(void);

/////////////Global Variables/////////////
double distance=0;
double oldX=0, oldY=0;
double newX=0, newY=0;
double startlong,startlat;
double newlong,newlat;
double approx;

char latitude[20];
char longitude[20];
const int AVERAGE_RADIUS_OF_EARTH_KM = 6372795;
/////////////Global Variables/////////////

void SystemInit(void){
	LED_init();
	lcd_init();
	UART_init();
}


int main (void){	
	GPS_Coordinates();
	startlat = newlat;
	startlong = newlong;
	lcd_command(1); 
		lcd_command(0x80); 
		lcd_data(longitude[0]);
		lcd_data(longitude[1]);
		lcd_data(longitude[2]);
		lcd_data(longitude[3]);
		lcd_data(longitude[4]);
		lcd_data(longitude[5]);
		lcd_data(longitude[6]);
		lcd_data(longitude[7]);
		delay_milli(100);
		
		lcd_command(0xC0);
		lcd_data(latitude[0]);
		lcd_data(latitude[1]);
		lcd_data(latitude[2]);
		lcd_data(latitude[3]);
		lcd_data(latitude[4]);
		lcd_data(latitude[5]);
		lcd_data(latitude[6]);
		lcd_data(latitude[7]);
	while(1){
		GPS_Coordinates();
		update_distance();
		RED_LED();
		reached_pos(30.0631, 31.2802);
		lcd_command(1); 
		lcd_command(0x80); 
		lcd_data('D');
		lcd_data('i');
		lcd_data('s');
		lcd_data('t');
		lcd_data('a');
		lcd_data('n');
		lcd_data('c');
		lcd_data('e');
		lcd_data(':');
		lcd_display(distance);
		delay_milli(100);
		lcd_command(0xC0);
		
		lcd_data(longitude[0]);
		lcd_data(longitude[1]);
		lcd_data(longitude[2]);
		lcd_data(longitude[3]);
		lcd_data(longitude[4]);
		lcd_data(longitude[5]);
		lcd_data(longitude[6]);
		delay_milli(100);
		
		lcd_data(' ');
		
		lcd_data(latitude[0]);
		lcd_data(latitude[1]);
		lcd_data(latitude[2]);
		lcd_data(latitude[3]);
		lcd_data(latitude[4]);
		lcd_data(latitude[5]);
		lcd_data(latitude[6]);
		delay_milli(100);
	}
}

//initialization functions

// 1st SSD will take pins PD0 - PD6  			HUNDREDS
// 2nd SSD will take pins PB0 - PB6  			TENS
// 3rd SSD will take pins PA2 - pA7,PD7  	ONES
void SSD_init(void){
	SYSCTL_RCGCGPIO_R |= 0x1B;   // Enable port A,B,D,E;
	while((SYSCTL_PRGPIO_R & 0x1B) == 0);

	GPIO_PORTA_LOCK_R = 0x4C4F434B;
	GPIO_PORTA_CR_R |= 0xFC;                    //UNCLOCKING PORT A PINS: 2-7

	GPIO_PORTB_LOCK_R = 0x4C4F434B;
	GPIO_PORTB_CR_R |= 0x7F;                    //UNCLOCKING PORT B PINS: 0-6

	GPIO_PORTD_LOCK_R = 0x4C4F434B;
	GPIO_PORTD_CR_R |= 0xFF;                    //UNCLOCKING PORT D PINS: 0-7

	GPIO_PORTA_DIR_R |= 0xFC;
	GPIO_PORTA_DEN_R |= 0xFC;
	GPIO_PORTA_AMSEL_R &= ~0xFC;            //SET PINS 2-7 AS DIGITAL OUTPUT

	GPIO_PORTB_DIR_R |= 0x7F;
	GPIO_PORTB_DEN_R |= 0x7F;
	GPIO_PORTB_AMSEL_R &= ~0x7F;            //SET PINS 2-7 AS DIGITAL OUTPUT

	GPIO_PORTD_DIR_R |= 0xFF;
	GPIO_PORTD_DEN_R |= 0xFF;
	GPIO_PORTD_AMSEL_R &= ~0xFF;            //SET PINS 0-7 AS DIGITAL OUTPUT


	GPIO_PORTA_AFSEL_R &= ~0xFC;
	GPIO_PORTA_PCTL_R  &= ~0xFFFFFF00;

	GPIO_PORTB_AFSEL_R &= ~0x7F;
	GPIO_PORTB_PCTL_R   &= ~0x0FFFFFFF;

	GPIO_PORTD_AFSEL_R &= ~0xFF;
	GPIO_PORTD_PCTL_R  &= ~0xFFFFFFFF;

}

void LED_init(void){
		SYSCTL_RCGCGPIO_R |= 0x20;
    while((SYSCTL_PRGPIO_R & 0x20) == 0) ;

    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    GPIO_PORTF_CR_R |= 0x0E;                    //UNCLOCKING PORT F

    GPIO_PORTF_DIR_R |= 0x0E;
    GPIO_PORTF_DEN_R |= 0x0E;
    GPIO_PORTF_AMSEL_R &= ~0x0E;            //SET PINS 1,2,3 AS DIGITAL OUTPUT

    GPIO_PORTF_AFSEL_R &= ~0x0E;
    GPIO_PORTF_PCTL_R &= ~0x0000FFF0; //SET PINS 1,2,3 AS GPIOs

}

// GPS Functions //
void UART_init(void){
SYSCTL_RCGCUART_R |=0x0020;  // enable clock for UART5
SYSCTL_RCGCGPIO_R |=0x0010;   // activate port E

	 
	 //UART5 configuration
UART5_CTL_R =0;      //disable UART
UART5_IBRD_R=104;
// IBRD=int(1000000/9600)=int(104.16667)	 
UART5_FBRD_R=11;
//FBRD=round(0.16667*64)+0.5=11;
 UART5_LCRH_R |=0x0070 ; //8-bit length ,enable FIFO,no parity bits,one stop
 UART5_CTL_R |=0x0301;  //enable Tx,Rx,UART5
	 
//portE initailization
	 GPIO_PORTE_CR_R |= 0x30;        //allow changes to PE5-4
	 GPIO_PORTE_AFSEL_R |=0x30;    //enable alt funct on PE5-4
	 GPIO_PORTE_PCTL_R = ((GPIO_PORTE_PCTL_R&0x00FFFF)+0x110000);  // configure PE5-4 for UART5
	 GPIO_PORTE_DEN_R |= 0x30;      //set PE5-4 as digital
	 GPIO_PORTE_AMSEL_R &= ~0x30;   //Disable analog on PE5-4
}

char UART5_Receiver(void){
	  while((UART5_FR_R&0x10)!=0){} //wait when data are available (RXFE is 0)
    return ((char)(UART5_DR_R&0xFF)); 
}
char *ptr;
void GPS_Coordinates(void){
	uint8_t c;
	uint8_t i = 0;
	double temp;

	c = UART5_Receiver();
	while(c != '$'){
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

							temp = strtod(latitude, &ptr);
							temp /= 60.0;
							temp += 30;					
							newlat = temp;

							c = UART5_Receiver(); /* for N */
							if(c == 'S')
								{temp *= -1;}
								setX(temp);
							sprintf(latitude, "%.8f", temp);
								
							c = UART5_Receiver(); /* for , */
							c = UART5_Receiver(); /* for 0 */
							while(c == '0')
							{c = UART5_Receiver();/* for 3 */}
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

							temp = strtod(longitude, &ptr);
							temp /= 60.0;
							temp += 31;
							newlong = temp;
							c = UART5_Receiver(); /* for direction (E or W ) */
							if(c == 'W')
							{temp *= -1;}
							setY(temp);
							sprintf(longitude, "%.8f", temp);
							break;
						}
					}
				}
			}
		}
	}
	}
}
double lats[10];
double longs[10];
int counter =0;
double sum;
void update_lat_long_average(){
// take multible points
		counter = 0;
		while(counter<10){
			GPS_Coordinates();
			lats[counter]=newlat;
			longs[counter] = newlong;
			counter++;
		}
		//calculate average 
		counter = 0;
		while(counter<10){
			sum +=lats[counter];
			counter++;
		}
		sum /=10;
		newlat = sum;
		
		counter = 0;
		while(counter<10){
			sum +=longs[counter];
			counter++;
		}
		sum /=10;
		newlong = sum;
}


// 7-segment-display functions
uint8_t decimal_to_BCD(uint8_t num){
	switch(num){
        case 0:
            return 0x3F;
        case 1:
            return 0x06;
        case 2:
            return 0x1B;
        case 3:
            return 0x4F;
        case 4:
            return 0x66;
        case 5:
            return 0x6D;
        case 6:
            return 0x7D;
        case 7:
            return 0x07;
        case 8:
            return 0x7F;
    }
    return 0x6F;
	
}

void dist_to_display(uint16_t dist){
		uint8_t hundreds = dist/100;
    uint8_t tens=dist/10 - hundreds*10 ;
    uint8_t ones=dist-hundreds*100 - tens*10;

    uint8_t h = decimal_to_BCD(hundreds);
    uint8_t t = decimal_to_BCD(tens);
    uint8_t o = decimal_to_BCD(ones);

    GPIO_PORTD_DATA_R |= h;
    GPIO_PORTB_DATA_R |= t;

    GPIO_PORTA_DATA_R |= (o<<2);  // a - f on A2 - A7
    GPIO_PORTD_DATA_R |=((o&0x40)<<1); //set pin 7
	
}
// delay functions // 
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
//LCD functions
void lcd_command(unsigned char command){
	GPIO_PORTE_DATA_R &= ~0x0E; // RS,R/W => 0  TO SELECT COMMAND AND WRITE
	GPIO_PORTB_DATA_R  = command;  // put command in the data
	GPIO_PORTE_DATA_R |= 0x08;  // Enable is set
	delay_micro(0);
	GPIO_PORTE_DATA_R &= ~0x0E;
	if (command < 4){
		delay_milli(2); // command 1 and 2 needs up to 1.64ms //
	}		
	else{
		delay_micro(40); /* all others 40 us */	
	}
}
void lcd_data(unsigned char data){
	
	GPIO_PORTE_DATA_R |= 0x02;  // RS => 1  TO SELECT Data
	GPIO_PORTB_DATA_R  = data;  // put command in the data
	GPIO_PORTE_DATA_R |= 0x08;  // ENABLE IS SET TO 1
	delay_micro(40);
	GPIO_PORTE_DATA_R &= ~0x0E; // ENABLE IS SET TO 0
}
void lcd_display(uint16_t data){
	uint8_t thousands = data/1000;
	uint8_t hundreds = data/100 - thousands*10;
	uint8_t tens=data/10 - hundreds*10 - thousands *100;
	uint8_t ones=data-thousands *1000 -hundreds*100 - tens*10;
	
	lcd_data(thousands+'0');
	lcd_data(hundreds +'0');
	lcd_data(tens +'0');
	lcd_data(ones +'0');
}
void lcd_init(void){
		SYSCTL_RCGCGPIO_R |= 0x12; // Enable clock for PORTB and PORTE
    while((SYSCTL_PRGPIO_R & 0x12) == 0){} ;

    //GPIO_PORTB_LOCK_R = 0x4C4F434B;
    GPIO_PORTE_LOCK_R = 0x4C4F434B;
    GPIO_PORTB_CR_R |= 0xFF;            // Unlocking PORTB PIN 1...8;
    GPIO_PORTE_CR_R |= 0x0E;            // Unlocking PORTE PIN 1 2 3;

    GPIO_PORTB_DEN_R  |= 0xFF;
    GPIO_PORTB_AMSEL_R &= ~0xFF;
    GPIO_PORTE_DEN_R  |= 0x0E;
    GPIO_PORTE_AMSEL_R &= ~0x0E;

    GPIO_PORTB_DIR_R  |= 0xFF;   // Configure PORTB Pin 1~8 digital output pins
    GPIO_PORTE_DIR_R  |= 0x0E;  // Configure PORTF Pin 1, 2 and 3 digital input pins


    GPIO_PORTB_AFSEL_R  = 0x00;
    GPIO_PORTE_AFSEL_R &= ~0x0E;
    GPIO_PORTB_PCTL_R   = 0x00000000;
    GPIO_PORTE_PCTL_R  &= ~0x0000FFF0;
			
		delay_milli(20); /* initialization sequence */
		lcd_command(0x30);
		delay_milli(5);
		lcd_command(0x30);
		delay_micro(100);
		lcd_command(0x30);

    lcd_command(0x38); //8-bits,2 display lines, 5x7 font

    lcd_command(0x06); //increments automatically

    lcd_command(0x0F); //Turn on display

    lcd_command(0x01); //clear display
}

// Distance Function // 
double to_radians(double degree){
	return degree*(M_PI/180.0);
}
void update_distance(void){
	double latdist = to_radians(newlat-startlat);
	double longdist = to_radians(newlong-startlong);
	double a = sin(latdist/2)*sin(latdist/2) + cos(to_radians(startlat))*cos(to_radians(newlat))*sin(longdist/2)*sin(longdist/2);
	double c = 2*atan2(sqrt(a),sqrt(1-a));
	
	double temp = (AVERAGE_RADIUS_OF_EARTH_KM * c);
	if(temp<=18)
		return;
	distance+=temp;
	startlong = newlong;
	startlat = newlat;
}


// LED functions //
void RED_LED(void){

    if (distance > 100.00){
    GPIO_PORTF_DATA_R|= 0x02;
    }
}
void reached_pos(double lat, double lon){
		double latdist = to_radians(lat-newlat);
		double longdist = to_radians(lon-newlong);
		double a = sin(latdist/2)*sin(latdist/2) + cos(to_radians(newlat))*cos(to_radians(lat))*sin(longdist/2)*sin(longdist/2);
		double c = 2*atan2(sqrt(a),sqrt(1-a));
	
		double temp = (AVERAGE_RADIUS_OF_EARTH_KM * c);
		if(temp <= 35)
			GPIO_PORTF_DATA_R|= 0x04;
		else
			GPIO_PORTF_DATA_R&= ~0x04;
			

}



// Setters for Global X-Coordinates //
void setX(double x){

    newX=x;
}
void setY(double y){

    newY=y;
}
void CalculateDistance(double newX,double newY ){
	if(oldX == 0.0 || oldY == 0.0){
				oldX=newX;
        oldY=newY;
	}
	else{
    double xsquare= (oldX-newX)*(oldX-newX);
    double ysquare= (oldY-newY)*(oldY-newY);
    double summing =  xsquare + ysquare;

    distance+= sqrt(summing)* 100000;
        oldX=newX;
        oldY=newY;
		}
}





