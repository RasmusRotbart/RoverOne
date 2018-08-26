#include <Servo.h>
#include "SR04.h"

#define BUTTON_PIN 4
#define SERVO_PIN 9
#define TRIG_PIN 11
#define ECHO_PIN 12

SR04 sr04 = SR04(ECHO_PIN, TRIG_PIN);
long dist;
long dists[3] = {666, 666, 666};

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos[] = {135, 90, 45, 90};
int count = 1;

void setup() {
  myservo.attach(SERVO_PIN);  // attaches the servo on pin 9 to the servo object
  myservo.write(pos[count]);
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  for(int i=0; i<3;i++){
    Serial.print(dists[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void loop() {
  if(digitalRead(BUTTON_PIN) == LOW){
    count = (count + 1) % 4;

    //Serial.print(count);
    //Serial.print(": ");
    
    myservo.write(pos[count]);
    delay(500);
    
    dist = sr04.Distance();
    dists[count==3?1:count] = dist;
    for(int i=0; i<3;i++){
      Serial.print(dists[i]);
      Serial.print(" ");
    }
    Serial.println();
    //delay(200);
  }
}

