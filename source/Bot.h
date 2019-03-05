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
	
	void waitButton(){
		int buttonState = 0;
		// pocakaj da je tipka pritisnjena
		while(buttonState == 0){
			buttonState = digitalRead(button);
		}
		usleep(10000);
		// pocakaj da je tipka spuscena
		while(buttonState == 1){
			buttonState = digitalRead(button);
		}
	}
	
	void moveServo(int angle, int servoPin){
		int delay = 20000;
		int pulz = angle;
		for(int i=0; i<10; i++){
			digitalWrite(servoPin, 1);
			usleep(pulz);
			digitalWrite(servoPin, 0);
			usleep(delay-pulz);
		}
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
		//odpri zgornji servo in zapri spodnjega
		moveServo(1500, servoLower);
		moveServo(2000, servoUpper);
		
		usleep(10000);
		// zapri zgornji servo
		moveServo(1500, servoUpper);
		usleep(100000);

		//odpri spodnji servo, da zeton pade ven
		moveServo(2000, servoLower);
		usleep(100000);
	}
};
