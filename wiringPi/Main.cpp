#include <iostream>
#include <wiringPi.h>
#include <unistd.h> // usleep
using namespace std;

// koracni motor za premikanje roke
int dir = 5; // 24BCM
int step = 4; // 23BCM
// stikalo za umeritev roke (home)
int armCtrl = 27;  //16BCM
// gumb za naslednji krog
int button = 28; // 20BCM
// servo za spuščanje žetonov
int servoUpper = 1; // 18BCM
int servoLower = 26; // 12BCM


int main(void)//(int argc, char **argv)
{
	if(wiringPiSetup()<0){
		cout<<"setup wiring pi failed"<<endl;
		return 1;
	}
	pinMode(dir, OUTPUT);
	pinMode(step, OUTPUT);
	pinMode(button,INPUT);
	pinMode(armCtrl,INPUT);
	pinMode(servoUpper,OUTPUT);
	pinMode(servoLower,OUTPUT);
	pullUpDnControl(button, PUD_UP); // prizgi pull up resistor
	pullUpDnControl(armCtrl, PUD_UP); // prizgi pull up resistor
	
	int delay = 20000;
	int pulz = 2000;
	
	std::cin >> pulz;
	for(int i=0; i<10; i++){
		digitalWrite(servoLower, 1);
		usleep(pulz);
		digitalWrite(servoLower, 0);
		usleep(delay-pulz);
	}
	
	
	
	
	return 0;
	int steps = 1;
    while(steps != 0){
        std::cout << " steps: ";
        std::cin >> steps;
        
        
        // move stepper
        if(steps < 0){
            digitalWrite(dir, 0); // levo
            steps *= -1;
        }
        else{
            digitalWrite(dir, 1); // desno
        }
        if(steps == 1000){ // go home
            digitalWrite(dir, 0); // levo
            int armCtrlState = 1;
            while(armCtrlState == 1){
                std::cout << armCtrlState << "\n";
                armCtrlState = digitalRead(armCtrl);
                digitalWrite(step, 1);
                usleep(1000); // 1ms
                digitalWrite(step, 0);
                usleep(1000); // 1ms
            }
        }
        else{
            for(int i=0; i<steps; i++){
                digitalWrite(step, 1);
                usleep(1000); // 1ms
                digitalWrite(step, 0);
                usleep(1000); // 1ms
            }
        }
        
    }
	
	return 0;
}
