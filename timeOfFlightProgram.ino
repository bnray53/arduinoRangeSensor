#include <LiquidCrystal.h>

LiquidCrystal LCD(10, 9, 5, 4, 3, 2);

//Pin variables
const int trigger = 13;
const int echo = 11;
const int ledRed = 6;
const int ledGreen = 7;

//Distance variables (in inches)
float targetDistance;
float setDistance;

//Flag variables for while loops
boolean initializeDistanceFlag = true;
boolean timerFlag = true;

//Timer variables
unsigned long timeMoment, startTime, stopTime;
float totalTime = 0.0;
float tempTime = 0.0;

void setup() {

  Serial.begin(9600);
  
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);

  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);

  LCD.begin(16, 2);
  LCD.setCursor(2, 0);
  LCD.print("Initializing");

  //Get the initial target distance
  while (initializeDistanceFlag) {
    digitalWrite(ledRed, HIGH);
    targetDistance=getDistance(trigger, echo);
    if (targetDistance < 50.0) {
      digitalWrite(ledRed, LOW);
      setDistance = targetDistance + 3.0;
      initializeDistanceFlag = false;
    }
  }

  //Give user confirmation of aquisition of distance with message and 4 green LED blinks
  LCD.clear();
  LCD.setCursor(4, 0);
  LCD.print("Aquired");
  for (int i = 0; i < 4; i++) {
    digitalWrite(ledGreen, HIGH);
    delay(200);
    digitalWrite(ledGreen, LOW);
    delay(200);
  }
  LCD.clear();

  //Setup screen
  LCD.setCursor(2, 0);
  LCD.print("Time In Air:");
  LCD.setCursor(2, 1);
  LCD.print(totalTime);
  LCD.print(" seconds");

}

void loop() {
  timerFlag = true;
  targetDistance=getDistance(trigger, echo);

  if (targetDistance > setDistance) {
    //Start timer here
    startTime = millis();
    digitalWrite(ledRed, LOW);
    digitalWrite(ledGreen, HIGH);

    while (timerFlag) {
      targetDistance=getDistance(trigger, echo);
      if (!(targetDistance > setDistance)) {
        timerFlag = false;
      }
      
      //Display timer in real time here
      timeMoment = millis();
      tempTime = (timeMoment - startTime) / 1000.0;
      LCD.setCursor(0, 1);
      LCD.print("                ");
      LCD.setCursor(2, 1);
      LCD.print(tempTime);
      LCD.print(" seconds");
    }
    stopTime = millis();
    totalTime = (stopTime - startTime) / 1000.0;
    digitalWrite(ledRed, HIGH);
    digitalWrite(ledGreen, LOW);
  } else {
    digitalWrite(ledRed, HIGH);
    digitalWrite(ledGreen, LOW);
  }

  //Display time until next jump
  LCD.setCursor(0, 1);
  LCD.print("                ");
  LCD.setCursor(2, 1);
  LCD.print(totalTime);
  LCD.print(" seconds");
  delay(250);
}

float getDistance(int trigPin, int echoPin) {
  float speedOfSound = 776.5;
  digitalWrite(trigPin, LOW); //Set trigger pin low
  delayMicroseconds(2000); //Let signal settle
  digitalWrite(trigPin, HIGH); //Set trigPin high
  delayMicroseconds(15); //Delay in high state
  digitalWrite(trigPin, LOW); //ping has now been sent
  delayMicroseconds(10); //Delay in high state

  float pingTime = pulseIn(echoPin, HIGH);  //pingTime is presented in microceconds
  pingTime = pingTime / 1000000; //convert pingTime to seconds by dividing by 1000000 (microseconds in a second)
  pingTime = pingTime / 3600; //convert pingtime to hours by dividing by 3600 (seconds in an hour)
  float distance = speedOfSound * pingTime; //This will be in miles, since speed of sound was miles per hour
  distance = distance / 2; //Remember ping travels to target and back from target, so you must divide by 2 for actual target distance.
  distance = distance * 63360; //Convert miles to inches by multipling by 63360 (inches per mile)
  return distance;
}
