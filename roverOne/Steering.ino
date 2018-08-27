// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
    Name:       Steering.ino
    Created:	27.08.2018 22:15:41
    Author:     USER-HOME\creit
*/

// Define User Types below here or use a .h file
// 
#include <Stepper.h>;

// Define Function Prototypes that use User Types below here or use a .h file
//
#define POTI_PIN A0
#define SPU 2048

int pos0;
int pos;
Stepper Motor(SPU, 3, 5, 4, 6);

// Define Functions below here or use other .ino or cpp files
//
int calculateSteps(int numberReads)
{
	int total = 0;

	for (int i = 0; i < numberReads; i++)
	{
		total += analogRead(POTI_PIN);
	}

	int result = total / numberReads;

	delay(1);

	return result;
}

// The setup() function runs once each time the micro-controller starts
void setup()
{
	Serial.begin(14400);

	Motor.setSpeed(15);

	pos0 = 0;
}

// Add the main program code into the continuous loop() function
void loop()
{	
	int pos = map(calculateSteps(10), 0, 1023, -512, 512);

	Serial.println(pos);

	Motor.step(pos - pos0);

	pos0 = pos;
}
