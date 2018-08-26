#include <Servo.h>
//#include "SR04.h"
#include <NewPing.h>

#define BUTTON_PIN 4
#define SERVO_PIN 5
#define TRIG_PIN 6
#define ECHO_PIN 7

#define CLOCK_PIN 8
#define LATCH_PIN 9
#define DATA_PIN 10

//SR04 sr04 = SR04(ECHO_PIN, TRIG_PIN);
int maxDist = 500;
NewPing sonar(TRIG_PIN, ECHO_PIN, maxDist);
int dist;

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int angle = 45;
int stepsize = 15;
int numBins = 2*angle/stepsize+1;
int count = (numBins-1)/2;
int* dists;
byte leds;
bool onoff = false;

void updateShiftRegister(){
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, leds);
  digitalWrite(LATCH_PIN, HIGH);
}

void setup() {
  dists = new int[numBins];
  for(int i=0; i<numBins; i++){
    dists[i] = 666;
  }
  
  myservo.attach(SERVO_PIN);  // attaches the servo on pin 9 to the servo object
  myservo.write(90);

  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);

  updateShiftRegister();
  
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  for(int i=0; i<numBins;i++){
    Serial.print(0-angle+i*stepsize);
    Serial.print("\t");
  }
  Serial.println();
  for(int i=0; i<numBins;i++){
    Serial.print("--------");
  }
  Serial.println();
  for(int i=0; i<numBins;i++){
    Serial.print(dists[i]);
    Serial.print("\t");
  }
  Serial.println();
}

void loop() {
  if(digitalRead(BUTTON_PIN) == LOW){
    onoff = !onoff;
    delay(300);
  }

  if(onoff){

    while(count<numBins){
      count++;
      myservo.write(90+angle-count*stepsize);
      dist = sonar.convert_cm(sonar.ping_median(3));
      if(dist==0) dist=666;
      dists[count] = dist;
      if(dist>50) bitSet(leds, count);
      else bitClear(leds, count);
      updateShiftRegister();
      //delay(200);
    }
  
    while(count>=0){
      count--;
      myservo.write(90+angle-count*stepsize);
      //delay(100);
      dist = sonar.convert_cm(sonar.ping_median(3));
      if(dist==0) dist=666;
      dists[count] = dist;
      if(dist>50) bitSet(leds, count);
      else bitClear(leds, count);
      updateShiftRegister();
      //delay(200);
    }
    for(int i=0; i<numBins;i++){
      Serial.print(dists[i]);
      Serial.print("\t");
    }
    Serial.println();
  }
}

