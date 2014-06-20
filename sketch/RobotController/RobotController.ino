#include "ZumoMotors.h"


#define LED_PIN       13 // user LED pin

#define MAX_SPEED             400 // max motor speed
#define PULSE_WIDTH_DEADBAND   25 // pulse width difference from 1500 us (microseconds) to ignore (to compensate for control centering offset)
#define PULSE_WIDTH_RANGE     350 // pulse width difference from 1500 us to be treated as full scale input (for example, a value of 350 means
                                  //   any pulse width <= 1150 us or >= 1850 us is considered full scale)
#define DIR_LEFT 0
#define DIR_UP 1
#define DIR_DOWN 2
#define DIR_RIGHT 3


char inChar;
int left_speed;
int right_speed;

void setup()
{
  Serial.begin(57600);
  pinMode(LED_PIN, OUTPUT);

}

void stop(){
	
	ZumoMotors::setSpeeds(0, 0);	
}

void move(int dir){
	left_speed=0;
	right_speed=0;
	switch (dir){
		case DIR_LEFT:
			left_speed = 0;
			right_speed = 150;
			break;
		case DIR_UP:
			left_speed = 100;
			right_speed = 100;
			break;
		case DIR_DOWN:
			left_speed = -100;
			right_speed = -100;
			break;
		case DIR_RIGHT:
			left_speed =150;
			right_speed = 0;
			break;
		default:
			break;
	}		
        ZumoMotors::setSpeeds(left_speed,right_speed);
	
	delay(2000);
	stop();
}


void loop()
{
	while (Serial.available()){
		inChar = Serial.read();
                Serial.print(inChar);
		switch (inChar){
			case '0':
				move(DIR_LEFT);
				break;
			case '1':
				move(DIR_UP);
				break;
			case '2':
				move(DIR_DOWN);
				break;
			case '3':
				move(DIR_RIGHT);
				break;
			default:
				break;
			
		}
		
	}
}
