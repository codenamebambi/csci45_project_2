#include <wiringPi.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <sys/time.h>
#include <vector>
#include <utility>
#include <string>
#include "screen.h"

using namespace std;





#define ObstaclePin_1  19 //	13
#define ObstaclePin_2  13//	19
#define RelayPin	17
#define Trig	5
#define Echo	6
#define button_1	20
#define button_2	21
#define RelayPin_2	16


static volatile bool closed = false;
static volatile bool open = false;

void loadLogs (vector<string>&, int&, int&);
void saveLogs (vector<string>&, int&, int&);

void ultraInit() {
	pinMode (Echo, INPUT);
	pinMode (Trig, OUTPUT);
}

float disMeasure (void) {
	struct timeval tv1;
	struct timeval tv2;
	long time1, time2;
	float dis;

	digitalWrite (Trig, LOW);
	delayMicroseconds (2);

	digitalWrite (Trig, HIGH);
	delayMicroseconds (10);     //发出超声波脉冲
	digitalWrite (Trig, LOW);

	while (! (digitalRead (Echo) == 1));
	gettimeofday (&tv1, NULL);          //获取当前时间

	while (! (digitalRead (Echo) == 0));
	gettimeofday (&tv2, NULL);          //获取当前时间

	time1 = tv1.tv_sec * 1000000 + tv1.tv_usec;   //微秒级的时间
	time2  = tv2.tv_sec * 1000000 + tv2.tv_usec;

	dis = (float) (time2 - time1) / 1000000 * 34000 / 2; //求出距离

	return dis;
}


void ISR_1() {
	cout << "function done got called" << endl;
	closed = true;
	open = false;
}
void ISR_2() {
	closed = false;
	open = true;
}
void print_counter (int one, int two) {

	string temp_1 = to_string (one);
	write (0, 0, temp_1.c_str());
	string temp_2 = to_string (two);
	write (0, 1, temp_2.c_str());
}

void open_door();
void close_door();
void vibration_on();
void vibration_off();


void debug() {
	cout << "hey" << endl;
}





int main() {
	string temp;
	temp = "xset dpms force off"; //standby instead of off to turn screen back on
	//system (temp.c_str());
	if (wiringPiSetupGpio() == -1) {
		return 1;
	}
	if (wiringPiISR (ObstaclePin_1, INT_EDGE_RISING, &debug) < 0) { //initiates first obstacle sensor
		return 1;
	}

	if (wiringPiISR (ObstaclePin_2, INT_EDGE_FALLING, &ISR_2) < 0) { //intiates second obstacle sensor
		return 1;
	}

	pinMode (RelayPin, OUTPUT); //initiates relay
	digitalWrite (RelayPin, LOW); //makes sure its off defaultly
	pinMode (button_1, INPUT);
	pinMode (button_2, INPUT);
	pinMode (RelayPin_2, OUTPUT);



	float dis;
	int button_1_counter = 0;
	int button_2_counter = 0;
	vector<string> fileLogs;
	time_t temp_time;
	time_t temp_time_2;
	loadLogs (fileLogs, button_1_counter, button_2_counter);
	ultraInit();
	const string video_name = "button_reaction_";

	//start up for lcd screen
	fd = wiringPiI2CSetup (LCDAddr);
	init();




//	fileLogs.pushback(video_name + to_string(button_1_counter));
	/*
		temp="raspivid -t 10000 -o " + video_name; //10000 is 10 seconds
		system(temp.c_str());
		if( button 1 done got pushed ){
			button_1_counter++;
			temp="MP4Box -fps 30 -add "+video_name+" "+video_name+to_string(button_1_counter)+".mp4";
			system(temp.c_str());
			fileLogs.pushback(video_name+button_1_counter+".mp4");
		}

		//if second button gets pressed
		button_2_counter++;
		temp="xset dpms force standby";//standby instead of off to turn screen back on
		system(temp.c_str());
		temp="omxplayer "+fileLogs.back();
		system(temp.c_str());
		delay(10000);//delays 10 seconds while video plays
		temp="xset dpms force off";//standby instead of off to turn screen back on
		system(temp);
	*/


	while (true) {
		print_counter (button_1_counter, button_2_counter);
		dis = disMeasure();
		delay (300);


		if (dis <= 65) {
			delay (1000);				//this bit just checks that they stayed in front of the box and didnt just pass by
			if (dis <= 65) {

				temp_time = time (NULL);
				temp = "raspivid -t 10000 -o " + video_name; //10000 is 10 seconds
				system (temp.c_str());


				while (true/*dis <= 65*/) {
					//				if (time (NULL) >= temp_time + 10) {
					//open_door();
					temp_time_2 = time (NULL);
					//while (time (NULL) < temp_time + 10) {
					if (digitalRead (button_2)) {
						button_2_counter++;
						temp = "xset dpms force standby"; //standby instead of off to turn screen back on
						system (temp.c_str());
						temp = "omxplayer " + fileLogs.back();
						system (temp.c_str());
						delay (10000); //delays 10 seconds while video plays
						temp = "xset dpms force off"; //standby instead of off to turn screen back on
						system (temp.c_str());
						while (temp_time + 11 > time (NULL)) {
						delay (50);
						}
						break;
					}
					//	}
					//close_door();
					//	break;
					//	}
					if (digitalRead (button_1)) {
						while (digitalRead (button_1))
							vibration_on();
						vibration_off();
						button_1_counter++;
						while (time (NULL) < temp_time + 11) {
							delay (50);
						}
						//should probably change the final directory of the video files to a usb and change the code accordingly
						temp = "MP4Box -fps 30 -add " + video_name + " " + video_name + to_string (button_1_counter) + ".mp4";
						system (temp.c_str());
						fileLogs.push_back (video_name + to_string (button_1_counter) + ".mp4");
						break;
					} else vibration_off();
				}
			}

		}
		while (temp_time + 11 > time (NULL)) {
		delay (50);
		}
	}



//	close_door();
//	open_door();



	saveLogs (fileLogs, button_1_counter, button_2_counter);
	clear();
	temp = "xset dpms force standby"; //standby instead of off to turn screen back on
	system (temp.c_str());

}



void loadLogs (vector<string>& vec, int& b_1_count, int& b_2_count) {
	string temp_string;
	int temp_int;
	ifstream filenames;
	ifstream counters;
	filenames.open ("filename_log.txt");
	counters.open ("counter_log.txt");
	while (getline (filenames, temp_string)) {
		vec.push_back (temp_string);
	}
	counters >> b_1_count;
	counters >> b_2_count;
	filenames.close();
	counters.close();
}

void saveLogs (vector<string>& vec, int& b_1_count, int& b_2_count) {
	ofstream filenames;
	ofstream counters;
	filenames.open ("filename_log.txt");
	counters.open ("counter_log.txt");
	for (string i : vec) {
		filenames << i << endl;
	}
	counters << b_1_count << endl;
	counters << b_2_count << endl;
	filenames.close();
	counters.close();
}



void open_door() {
//	while (open == false) {
//		digitalWrite (RelayPin, HIGH);
//		delay (100);
//		digitalWrite (RelayPin, LOW);
//		delay (300);
//	}
	while (open == false)
		digitalWrite (RelayPin, HIGH);
	cout << "Exit OpenDoor: " << bool (open) << endl;
	digitalWrite (RelayPin, LOW);
}
void close_door() {
//	int meh=0;
//	while (closed == false) {
	//cout<<meh++<<endl;
//		digitalWrite (RelayPin, HIGH);
//		delay (100);
//		digitalWrite (RelayPin, LOW);
//		delay (300);
//	}
//	cout<<closed<<endl;
	while (closed == false)
		digitalWrite (RelayPin, HIGH);
	cout << "Exit closedDoor: " << bool (closed) << endl;
	digitalWrite (RelayPin, LOW);
}
void vibration_on() {
	digitalWrite (RelayPin_2, HIGH);
}
void vibration_off() {
	digitalWrite (RelayPin_2, LOW);
}






