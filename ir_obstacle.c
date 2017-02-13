#include <wiringPi.h>
#include <stdio.h>
#include <ctime>
#include <iostream>
#define ObstaclePin      13

void myISR(void)
{
	printf("Detected Barrier !\n");
	std::cout<<time(NULL)<<std::endl;
}


int main(void)
{
	if(wiringPiSetupGpio() == -1){ //when initialize wiring failed,print messageto screen
		printf("setup wiringPi failed !\n");
		return 1; 
	}
	
	if(wiringPiISR(ObstaclePin, INT_EDGE_RISING, &myISR) < 0){
		printf("Unable to setup ISR !!!\n");
		return 1;
	}
	
	while(1){
	;
		//	printf("Test\n");
	}

	return 0;
}

