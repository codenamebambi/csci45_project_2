#include <wiringPi.h>
#include <stdio.h>
#include <iostream>
#include <ctime>



#define RelayPin     17 //Change this to the appropriate Gpio pin



using namespace std;
int main(){
	if(wiringPiSetupGpio() == -1){ //when initialize wiring failed,print messageto screen
		printf("setup wiringPi failed !");
		return 1; 
	}
	pinMode(RelayPin, OUTPUT);
		
	for(int i=0;i<20;i++){	
		digitalWrite(RelayPin,HIGH);
		delay(225);
		digitalWrite(RelayPin,LOW);
		delay(300);
	}







/*	int x=0;
	while(cin){
	cin>>x;
	if(x==0) digitalWrite(RelayPin, LOW);
	else digitalWrite(RelayPin, HIGH);
	}
*/
	return 0;
}

