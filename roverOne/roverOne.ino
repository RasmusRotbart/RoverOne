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
float momentum = 0.8;

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

bool onoff = false;
int angle = 45;
int stepsize = 15;
int numBins = 2*angle/stepsize+1;
int count = 0;//(numBins-1)/2;
int perSide = angle/stepsize;

byte leds;

float veloForward = 0;
float veloLeft = 0;
float veloRight = 0;
int minVeloDist = 20;
int maxVeloDist = 50;
float maxBackVelo = -0.1;


void updateLEDS(){
  leds = 0;
  
  if(veloLeft<0) bitSet(leds, 3);  
  if(veloLeft>0){
    bitSet(leds, 2);
    if(veloLeft>0.5){
      bitSet(leds, 1);
      if(veloLeft>0.9) bitSet(leds, 0);
    }
  }

  if(veloRight<0) bitSet(leds, 4);  
  if(veloRight>0){
    bitSet(leds, 5);
    if(veloRight>0.50){
      bitSet(leds, 6);
      if(veloRight>0.90) bitSet(leds, 7);
    }
  }
  
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
  //if(dist>50) bitSet(leds, count);
  //else bitClear(leds, count);
} // end updateDists

void initDists(){
  myservo.write(90+angle-count*stepsize);
  delay(500);
  while(count<numBins){
    count++;
    updateDists();
  }
} // end initDists()

void computeVelocity(){
  int d = 0;
  veloForward = 0;
  for(int l=perSide-1; l<perSide+2; l++){
    d = (dists[l]>maxVeloDist)?maxVeloDist:dists[l];
    veloForward += d;
    if(l==perSide) veloForward += d;
  }
  veloForward = max(0,float(veloForward-4*minVeloDist)/float((maxVeloDist-minVeloDist)*4));
  
  d = 0;
  int dSum = 0;
  for(int l=0; l<perSide; l++){
    //d = (dists[l]<minVeloDist)?minVeloDist:dists[l];
    //d = (d>maxVeloDist)?maxVeloDist:d;
    d = (dists[l]>maxVeloDist)?maxVeloDist:dists[l];

    dSum += d;
  }
  veloRight = float(dSum-perSide*minVeloDist)/float((maxVeloDist-minVeloDist)*perSide);
  veloRight *= veloForward;
  veloRight = (veloRight>=maxBackVelo)?veloRight:maxBackVelo;

  d = 0;
  dSum = 0;
  for(int l=perSide+2; l<perSide*2+2; l++){
    //d = (dists[l]<minVeloDist)?minVeloDist:dists[l];
    //d = (d>maxVeloDist)?maxVeloDist:d;
    d = (dists[l]>maxVeloDist)?maxVeloDist:dists[l];

    dSum += d;
  }
  veloLeft = float(dSum-perSide*minVeloDist)/float((maxVeloDist-minVeloDist)*perSide);
  veloLeft *= veloForward;
  veloLeft = (veloLeft>=maxBackVelo)?veloLeft:maxBackVelo;

  if(veloRight == 0 && veloLeft == 0){
    // short part back and turn right
    veloLeft = maxBackVelo;
    veloRight = maxBackVelo;

    // print velos
    for(int i = 0; i < numBins; i++) Serial.print("\t");
    Serial.print(" | ");
    Serial.print(veloLeft);
    Serial.print("\t");
    Serial.println(veloRight);
    delay(1000);
    veloLeft = -maxBackVelo;
    // print velos
    for(int i = 0; i < numBins; i++) Serial.print("\t");
    Serial.print(" | ");
    Serial.print(veloLeft);
    Serial.print("\t");
    Serial.println(veloRight);
    delay(1000);
    initDists();
  }
  
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
    if(onoff == true){
      count=0;
      initDists();
    }
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
      computeVelocity();
      updateLEDS();
    }
  
    while(count>=0){
      if(digitalRead(BUTTON_PIN) == LOW){
        onoff = !onoff;
        delay(300);
        return;
      }
      count--;
      updateDists();
      computeVelocity();
      updateLEDS();
    }
    
    for(int i=0; i<numBins;i++){
      Serial.print(dists[i]);
      Serial.print("\t");
    }
    Serial.print(" | ");
    Serial.print(veloLeft);
    Serial.print("\t");
    Serial.print(veloRight);
    Serial.print("\t");
    Serial.print(veloForward);
    Serial.println();
  }
}

