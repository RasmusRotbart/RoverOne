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
int maxDist = 300;
NewPing sonar(TRIG_PIN, ECHO_PIN, maxDist);
int dist;
int* dists;
float momentum = 0.75;

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

bool onoff = false;
int angle = 45;
int stepsize = 15;
int numBins = 2*angle/stepsize+1;
int count = (numBins-1)/2;
int perSide = angle/stepsize+1;

byte leds;

int veloLeft = 0;
int veloRight = 0;
int minVeloDist = 20;
int maxVeloDist = 50;
int maxBackVelo = -10;


void updateLEDS(){
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, leds);
  digitalWrite(LATCH_PIN, HIGH);
} // end updateLEDS

void updateDists(){
  myservo.write(90+angle-count*stepsize);
  dist = sonar.convert_cm(sonar.ping_median(3));
  //if(dist==0) dist=666;
  if(dist!=0){
    dists[count] = int((1-momentum)*dists[count] + momentum*dist + 0.5);
  } else {
    // ??? change dist ?
  }
  if(dist>50) bitSet(leds, count);
  else bitClear(leds, count);
} // end updateDists

void computeVelocity(){
  int d = 0;
  int dSum = 0;
  for(int l=0; l<perSide; l++){
    //d = (dists[l]<minVeloDist)?minVeloDist:dists[l];
    //d = (d>maxVeloDist)?maxVeloDist:d;
    d = (dists[l]>maxVeloDist)?maxVeloDist:dists[l];

    dSum += d;
  }
  veloRight = 100.*float(dSum-perSide*minVeloDist)/float((maxVeloDist-minVeloDist)*perSide);
  veloRight = (veloRight>=maxBackVelo)?veloRight:maxBackVelo;

  d = 0;
  dSum = 0;
  for(int l=perSide; l<perSide*2; l++){
    //d = (dists[l]<minVeloDist)?minVeloDist:dists[l];
    //d = (d>maxVeloDist)?maxVeloDist:d;
    d = (dists[l]>maxVeloDist)?maxVeloDist:dists[l];

    dSum += d;
  }
  veloLeft = 100.*float(dSum-perSide*minVeloDist)/float((maxVeloDist-minVeloDist)*perSide);
  veloLeft = (veloLeft>=maxBackVelo)?veloLeft:maxBackVelo;
}

void setupTest(){
  updateLEDS();
  Serial.begin(9600);
  for(int i=0; i<numBins;i++){
    Serial.print(0-angle+i*stepsize);
    Serial.print("\t");
  }
  Serial.print(" | ");
  Serial.print("vL");
  Serial.print("\t");
  Serial.print("vR");
  Serial.println();
  for(int i=0; i<numBins;i++){
    Serial.print("--------");
  }
  Serial.print("-|------");
  Serial.println("--------");
  for(int i=0; i<numBins;i++){
    Serial.print(dists[i]);
    Serial.print("\t");
  }
  Serial.print(" | ");
  Serial.print(0);
  Serial.print("\t");
  Serial.print(0);
  Serial.println();
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
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  setupTest();
}

void loop() {
  if(digitalRead(BUTTON_PIN) == LOW){
    onoff = !onoff;
    delay(300);
  }

  if(onoff){

    while(count<numBins){
      if(digitalRead(BUTTON_PIN) == LOW){
        onoff = !onoff;
        delay(300);
        return;
      }
      count++;
      updateDists();
      updateLEDS();
      //delay(200);
    }
  
    while(count>=0){
      if(digitalRead(BUTTON_PIN) == LOW){
        onoff = !onoff;
        delay(300);
        return;
      }
      count--;
      updateDists();
      updateLEDS();
    }

    computeVelocity();

    for(int i=0; i<numBins;i++){
      Serial.print(dists[i]);
      Serial.print("\t");
    }
    Serial.print(" | ");
    Serial.print(veloLeft);
    Serial.print("\t");
    Serial.print(veloRight);
    Serial.println();
  }
}

