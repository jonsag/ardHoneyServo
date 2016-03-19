/*
  ardHoneyServo
  by Jon Sagebrand
  jonsagebrand@gmail.com
  github.com/jonsag/ardHoneyServo
*/

#include <Servo.h>
#include <LiquidCrystal.h>

Servo myservo;  // create servo object to control a servo

///// initialize the LCD library with the numbers of the interface pins
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

int manualAnglePin = 0;  // analog pin used to connect the potentiometer
int maxAnglePin = 1; // analog pin used to set max angle
int zeroOffsetPin = 2; // analog pin used to set zero angle

int manualAngleValue;    // variable to read the value from the analog pin
int lastmanualAngleValue = 0; // stores last value from pot
String potString = ""; // holds pot value as string
int potLength = 0; // holds number of characters for potString
int potPosition = 0; // holds position of print to lcd

int maxAngleValue; // variable to read the value from analog pin
int zeroOffsetValue; // variable to read the value from analog pin

String motorSpeedString = ""; // holds speed as string
int motorSpeedLength = 0; // holds number of characters for motorSpeedString

int maxAngle; // maximum degrees for the servo in each direction
int zeroOffset; // value for zero offset of servo
int servoAngle = 0; 
int motorSpeed = 0;
int servoLow = 0;
int servoHigh = 0;

///// Outputs:
int spinnerRelay = 1;

int spinning = 0;

///// Buttons:
const int startStopButton = 11;
const int program1Button = 12;
const int program2Button = 13;

int startStopButtonState = 0;
int program1ButtonState = 0;
int program2ButtonState = 0;

int lastStartStopButtonState = 0;
int lastProgram1ButtonState = 0;
int lastProgram2ButtonState = 0;

///// Man/Auto-switch
const int autoSwitch = 8;
//const int manSwitch = 1;

int autoSwitchState = 0;
//int manSwitchState = 0;

int lastAutoSwitchState = 0;
// inst lastManSwitchState = 0;

///// Left/Right-switch
const int leftSwitch = 10;
//const int rightSwitch = 10;

int leftSwitchState = 0;
//int rightSwitchState = 1;

int lastLeftSwitchState = 0;
// int lastRightSwitchState = 0;

long startStopTime = 0;         // the last time Start/Stop was toggled
long debounce = 200;   // the debounce time, increase if the output flickers
int i = 0; // just a counter

int program11 = 0;
int program11Percent = 25;
long program11RunTime = 60; // first sequence run time in seconds
long program11StartTime = 0;
int program12 = 0;
int program12Percent = 50;
long program12RunTime = 120; // second sequence run time in seconds
long program12StartTime = 0;

int program21 = 0;
int program21Percent = 50;
long program21RunTime = 2;
int program22 = 0;
int program22Percent = 75;
long program22RunTime = 2;

int displayTime = 0;
String timeString = "";
int timeLength = 0;

void setup() {
  ///// set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.print("Booting...");
  lcd.setCursor(0, 1);
  lcd.print("Init. in/outputs");
  pinMode(startStopButton, INPUT);
  pinMode(program1Button, INPUT);
  pinMode(program2Button, INPUT);
  pinMode(autoSwitch, INPUT);
  //pinMode(manSwitch, INPUT);
  pinMode(leftSwitch, INPUT);
  //pinMode(rightSwitch, INPUT);
  pinMode(spinnerRelay, OUTPUT);
  lcd.setCursor(0, 1);
  lcd.print("Attaching servo ");
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  lcd.clear();
}

void loop() {
  manualAngleValue = analogRead(manualAnglePin); // reads the value of the potentiometer (value between 0 and 1023)
  maxAngleValue = analogRead(maxAnglePin);
  zeroOffsetValue = analogRead(zeroOffsetPin);
  
  startStopButtonState = digitalRead(startStopButton);
  program1ButtonState = digitalRead(program1Button);
  program2ButtonState = digitalRead(program2Button);
  autoSwitchState = digitalRead(autoSwitch);
  //manSwitchState = digitalRead(manSwitch);
  if (spinning == 0) { // Don't change direction during spinning
    leftSwitchState = digitalRead(leftSwitch);
    //rightSwitchState = digitalRead(rightSwitch);
  }

  ///// Check if Manual/Auto-switch is switched. Then stop everything
  if (autoSwitchState != lastAutoSwitchState) {
    spinning = 0;
    servoAngle = 0;
    powerControl();
    writeLCD();
    lastAutoSwitchState = autoSwitchState;
  }

  ///// Set zero offset for servo
  zeroOffset = map(zeroOffsetValue, 1023, 0, -25, 25);
  ///// Set max angle for servo
  maxAngle = map(maxAngleValue, 0, 1023, 0, 90);
  
  if (autoSwitchState == 0) {
    manualRun();
  }
  else {
    autoRun();
  }
}

void writeLCD() {
  ///// Manual/Auto
  lcd.setCursor(0, 0);
  if (autoSwitchState == 1) {
    lcd.print("Auto  ");
  }
  else {
    lcd.print("Manual");
  }

  ///// Stopped/Running
  lcd.setCursor(8, 0);
  if (spinning == 1 && autoSwitchState == 0) {
    lcd.print("Running ");
  }
  else if (spinning == 1 && program11 == 1) {
    lcd.print("Prgm 1-1");
  }
  else if (spinning == 1 && program12 == 1) {
    lcd.print("Prgm 1-2");
  }
  else if (spinning == 1 && program21 == 1) {
    lcd.print("Prgm 2-1");
  }
  else if (spinning == 1 && program11 == 1) {
    lcd.print("Prgm 2-2");
  }
  else if (spinning == 0) {
    lcd.print("Stopped ");
  }
  else {
    lcd.print("Error...");
  }

  ///// Direction and speed
  lcd.setCursor(0, 1);
  if (leftSwitchState == 1) {
    lcd.print("Left:");
  }
  else {
    lcd.print("Right:");
  }

  if (leftSwitchState == 1) {
    motorSpeed = map(servoAngle, 90, 90 - maxAngle, 0, 100);
  }
  else {
    motorSpeed = map(servoAngle, 90, 90 + maxAngle, 0, 100);
  }

  motorSpeedString = String(motorSpeed);
  motorSpeedLength = motorSpeedString.length();
  if (leftSwitchState == 1) {
    lcd.setCursor(5, 1);
    lcd.print(motorSpeed);
    lcd.setCursor(5 + motorSpeedLength, 1);
    lcd.print("%  ");
  }
  else {
    lcd.setCursor(6, 1);
    lcd.print(motorSpeed);
    lcd.setCursor(6 + motorSpeedLength, 1);
    lcd.print("%  ");
  }

  if (autoSwitchState == 1) {
    // Time left on each auto sequence
    timeString = String(displayTime);
    timeLength = timeString.length();
    lcd.setCursor(11, 1);
    lcd.print(displayTime);
    lcd.setCursor(11 + timeLength, 1);
    lcd.print("s   ");
  }
  else {
    potPosition = map(manualAngleValue, 0, 1023, 0, 100);
    potString = String(potPosition);
    potLength = potString.length();
    lcd.setCursor(11, 1);
    lcd.print(potPosition);
    lcd.setCursor(11 + potLength, 1);
    lcd.print("%  ");
  }
}

void manualRun() {
  ////////////////////////////////////////// Checking inputs ///////////////////////////////////////////
  ///// Potentiometer
  if (manualAngleValue < lastmanualAngleValue - 5 || manualAngleValue > lastmanualAngleValue + 5) {
    powerControl();
    writeLCD();
    lastmanualAngleValue = manualAngleValue;
  }
  ///// Start/Stop-button
  if (startStopButtonState != lastStartStopButtonState) {
    if (startStopButtonState == 1 && lastStartStopButtonState == 0 && millis() - startStopTime > debounce) {
      if (spinning == 1) {
        spinning = 0;
      }
      else {
        spinning = 1;
      }
      //program11 = 0;
      //program12 = 0;
      powerControl();
    }
    writeLCD();
    lastStartStopButtonState = startStopButtonState;
    startStopTime = millis();
  }
  ///// Left/Right-switch
  if (leftSwitchState != lastLeftSwitchState) {
    powerControl();
    writeLCD();
    lastLeftSwitchState = leftSwitchState;
  }
  if (leftSwitchState == 1) {
    servoLow = 90 - maxAngle;
    servoHigh = 90;
  }
  else {
    servoLow = 90 + maxAngle;
    servoHigh = 90;
  }
  powerControl();
}

void autoRun() {
  ////////////////////////////////////////// Checking inputs ///////////////////////////////////////////

  if (program1ButtonState != lastProgram1ButtonState) {
    if (program1ButtonState == 1 && lastProgram1ButtonState == 0 && millis() - program11StartTime > debounce) {
      program11 = 1;
      program11StartTime = millis();
    }
    writeLCD();
    lastProgram1ButtonState = program1ButtonState;
  }
  /*
    if (program2ButtonState != lastProgram2ButtonState) {
      writeLCD();
      lastProgram2ButtonState = program2ButtonState;
    }
    if (program11 == 1) {
      if (leftSwitchState == 1) {
        servoAngle = 90 - program11Percent;
      }
      else {
        servoAngle = 90 + program11Percent;
      }
      spinning = 1;
      writeLCD();
      powerControl();
    }
  */
  ///// Start/Stop-button
  if (startStopButtonState != lastStartStopButtonState) {
    if (startStopButtonState == 1 && lastStartStopButtonState == 0 && millis() - startStopTime > debounce) {
      if (spinning == 1) {
        spinning = 0;
      }
      program11 = 0;
      program12 = 0;
      powerControl();
    }
    writeLCD();
    lastStartStopButtonState = startStopButtonState;
    startStopTime = millis();
  }

  ////////////////////////////////////////// Auto sequences ///////////////////////////////////////////
  ///// Program 1
  if (program11 == 1) { // Program 1-1 starts
    spinning = 1; // Rotation starts
    if (leftSwitchState == 1) { ///// Check direction switch and set servo angle
      servoAngle = 90 - maxAngle * program11Percent / 100;
    }
    else {
      servoAngle = 90 + maxAngle * program11Percent / 100;
    }
    writeLCD();
    powerControl();
  }
  if (program11 == 1 && millis() - program11StartTime > (program11RunTime * 1000)) { // Program 1-2 starts
    program11 = 0; // Sequence 1-1 stops
    program12 = 1;
    program12StartTime = millis();
    if (leftSwitchState == 1) { ///// Check direction switch and set servo angle
      servoAngle = 90 - maxAngle * program12Percent / 100;
    }
    else {
      servoAngle = 90 + maxAngle * program12Percent / 100;
    }
    writeLCD();
    powerControl();
  }

  if (program12 == 1 && millis() - program12StartTime > (program12RunTime * 1000)) { // Last sequence finished
    servoAngle = 90;
    spinning = 0;
    writeLCD();
    powerControl();
    program12 = 0;
  }

  ///// Running time left
  if (spinning == 1) {
    if (program11 == 1) {
      displayTime = ((program11RunTime * 1000) - millis() + program11StartTime) / 1000;
    }
    else if (program12 == 1) {
      displayTime = ((program12RunTime * 1000) - millis() + program12StartTime) / 1000;
    }
  }
  else {
    displayTime = 0;
  }
  writeLCD();
}

void powerControl() {
  if (spinning == 0) {
    servoAngle = 90;
  }
  else if (autoSwitchState == 0) {
    servoAngle = map(manualAngleValue, 1023, 0, servoLow, servoHigh);
  }

  myservo.write(servoAngle + zeroOffset); // sets the servo position according to the scaled value
  delay(15); // waits for the servo to get there
  digitalWrite(spinnerRelay, spinning);
}

