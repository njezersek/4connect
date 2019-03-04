#pragma once
#include <iostream>
#include <wiringPi.h>
#include <unistd.h> // usleep

class Bot{
public:
	// koracni motor za premikanje roke
	int dir = 5; // 24BCM
	int step = 4; // 23BCM
	// stikalo za umeritev roke (home)
	int armCtrl = 27;  //16BCM
	// gumb za naslednji krog
	int button = 28; // 20BCM
	// servo za spuščanje žetonov
	int servoUpper = 26; // 12BCM
	int servoLower = 1; // 18BCM
	
	int columnWidth = 167; // širina stolpv v korakih
	int homeToColumn = 350; // število korakov od začetne pozicije do prvega stolpca
	
	Bot(){
		if(wiringPiSetup()<0){
			std::cout<<"setup wiring pi failed\n";
			return;
		}
		std::cout << "Bot created\n";
		pinMode(dir, OUTPUT);
		pinMode(step, OUTPUT);
		pinMode(button,INPUT);
		pinMode(armCtrl,INPUT);
		pinMode(servoUpper,OUTPUT);
		pinMode(servoLower,OUTPUT);
		pullUpDnControl(button, PUD_UP); // prizgi pull up resistor
		pullUpDnControl(armCtrl, PUD_UP); // prizgi pull up resistor
	}
	
	~Bot(){
	
	}
	
	void goHome(){
		digitalWrite(dir, 0); // levo
		int armCtrlState = 1;
		while(armCtrlState == 1){
			armCtrlState = digitalRead(armCtrl);
			digitalWrite(step, 1);
			usleep(1000); // 1ms
			digitalWrite(step, 0);
			usleep(1000); // 1ms
		}
	}
	
	void dropColumn(int column){
		goHome();
		if(column < 0 || column > 6)return;
		move(homeToColumn + columnWidth*column);
		drop();
		goHome();
	}
	
	void move(int steps){
		if(steps < 0){
			digitalWrite(dir, 0); // levo
			steps *= -1;
		}
		else digitalWrite(dir, 1); // desno
		
		for(int i=0; i<steps; i++){
			digitalWrite(step, 1);
			usleep(1000); // 1ms
			digitalWrite(step, 0);
			usleep(1000); // 1ms
		}
	}
	
	void closeHead(){
		// postavi oba servota v zaprto pozicijo
		int pulz = 1500;
		int delay = 20000;
		for(int i=0; i<10; i++){
			digitalWrite(servoUpper, 1);
			digitalWrite(servoLower, 1);
			usleep(pulz);
			digitalWrite(servoUpper, 0);
			digitalWrite(servoLower, 0);
			usleep(delay-pulz);
		}
		
	}
	
	void drop(){
		int pulzUpper;
		int pulzLower;
		int delay = 20000;
		//odpri zgornji servo in zapri spodnjega
		pulzUpper = 0;
		pulzLower = 1500;
		for(int i=0; i<10; i++){
			digitalWrite(servoLower, 1);
			usleep(pulzLower);
			digitalWrite(servoLower, 0);
			usleep(delay-pulzLower);
		}
		for(int i=0; i<10; i++){
			digitalWrite(servoUpper, 1);
			usleep(pulzUpper);
			digitalWrite(servoUpper, 0);
			usleep(delay-pulzUpper);
		}
		
		// zapri zgornji servo
		pulzUpper = 1500;
		for(int i=0; i<10; i++){
			digitalWrite(servoUpper, 1);
			usleep(pulzUpper);
			digitalWrite(servoUpper, 0);
			usleep(delay-pulzUpper);
		}
		
		//odpri spodnji servo, da zeton pade ven
		pulzLower = 0;
		for(int i=0; i<10; i++){
			digitalWrite(servoLower, 1);
			usleep(pulzLower);
			digitalWrite(servoLower, 0);
			usleep(delay-pulzLower);
		}
		
	}
};
