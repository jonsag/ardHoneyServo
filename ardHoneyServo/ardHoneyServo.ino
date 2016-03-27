
/*
  ardHoneyServo
  by Jon Sagebrand
  jonsagebrand@gmail.com
  github.com/jonsag/ardHoneyServo
*/

#include <Servo.h>
#include <LiquidCrystal595.h> // 3 pin LCD
//#include <LiquidCrystal.h> // 6 pin LCD


Servo myservo;  // create servo object to control a servo

///// initialize the LCD library with the numbers of the interface pins
LiquidCrystal595 lcd(5, 6, 7); // 3 pin LCD: datapin, latchpin, clockpin
// LiquidCrystal lcd(7, 6, 5, 4, 3, 2); // 6 pin LCD

///// manual inputs
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

//String motorSpeedString = ""; // holds speed as string
//int motorSpeedLength = 0; // holds number of characters for motorSpeedString

int maxAngle; // maximum degrees for the servo in each direction
int zeroOffset; // value for zero offset of servo
int servoAngle = 0;
int motorSpeed = 0;
int servoLow = 0;
int servoHigh = 0;

///// Digital outputs:
int spinnerRelay = 4; // pin for spinner relay
bool spinning = 0; // high if spinner is spinning

///// Digital inputs:
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

///// Rotary encoder
const int encoderCLK = 1; // Rotary encoder CLK signal
const int encoderDT = 2; // Rotary encoder DT signal
const int encoderSW = 3; // Rotary encoders switch, goes LOW when pressed
int encoderCLKState = 0;
int encoderDTState = 0;
int encoderSWState = 0;
int lastEncoderDTState = 0;
int lastEncoderSWState = 0;

long startStopTimeMillis = 0; // the last time Start/Stop was toggled
long encoderSWTimeMillis = 0; // the last time encoder button was toggled
long debounce = 200;   // the debounce time, increase if the output flickers

int program11 = 0; // set when in auto sequence 1-1
int program11Percent = 25; // initial value, in percent, for servo in auto sequence 1-1
int program11RunTime = 60; // initial time, in seconds, for auto sequence 1-1
int program11StartTime = 0;
int program12 = 0; // set when in auto sequence 1-2
int program12Percent = 50; // initial value, in percent,  for servo in auto sequence 1-2
int program12RunTime = 120; // initial time, in seconds, for auto sequence 1-2
int program12StartTime = 0;
bool programMode1 = 0;
int programModeValue = 0; // what of the values are being programmed
unsigned long blinkColonMillis;
//bool blinkOut = 0;
String blinkChar = "=";

int program21 = 0; // set when in auto sequence 2-1
int program21Percent = 50; // initial value, in percent,  for servo in auto sequence 2-1
int program21RunTime = 30; // initial time, in seconds, for auto sequence 2-1
int program22 = 0; // set when in auto sequence 2-2
int program22Percent = 75; // initial value, in percent,  for servo in auto sequence 2-2
int program22RunTime = 150; // initial time in, in seconds, for auto sequence 2-2
bool programMode2 = 2;

int displayTime = 0;
//String timeString = "";
//int timeLength = 0;


String valString = ""; // for calculation of length of values
int valLength;

int i = 0; // just a counter

void setup() {
  ///// set up the LCD's number of columns and rows:
  lcd.begin(16, 2);  // define LCD layout
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Booting...");
  delay(500);
  lcd.setCursor(0, 1);
  lcd.print("Init. in/outputs");
  delay(500);
  pinMode(startStopButton, INPUT);
  pinMode(program1Button, INPUT);
  pinMode(program2Button, INPUT);
  pinMode(autoSwitch, INPUT);
  //pinMode(manSwitch, INPUT);
  pinMode(leftSwitch, INPUT);
  //pinMode(rightSwitch, INPUT);
  pinMode(spinnerRelay, OUTPUT);
  pinMode(encoderCLK, INPUT);
  pinMode(encoderDT, INPUT);
  pinMode(encoderSW, INPUT);
  lcd.setCursor(0, 1);
  lcd.print("Attaching servo ");
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  delay(500);
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

  if (autoSwitchState == 0 && program1ButtonState == 1 && spinning == 0 && programMode1 != 1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Entering auto");
    lcd.setCursor(0, 1);
    lcd.print("program 1...");
    programMode1 = 1;
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("1-1%:");
    lcd.setCursor(8, 0);
    lcd.print("1-1s:");
    lcd.setCursor(0, 1);
    lcd.print("1-2%:");
    lcd.setCursor(8, 1);
    lcd.print("1-2s:");
    writeLCDProgramMode(program11Percent, program11RunTime, program12Percent, program12RunTime);
    programModeValue = 1;
    blinkColonMillis = millis();
  }
  if (programMode1 == 1 && startStopButtonState == 1) {
    programMode1 = 0;
    programModeValue = 0;
    startStopButtonState = 0;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Exiting program");
    lcd.setCursor(0, 1);
    lcd.print("mode...");
    delay(1000);
    lcd.clear();
  }

  if (programMode1 == 1) {
    programSequence1();
  }
  else if (autoSwitchState == 0) {
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

  valLength = intToStringToLength(motorSpeed);
  if (leftSwitchState == 1) {
    lcd.setCursor(5, 1);
    lcd.print(motorSpeed);
    lcd.setCursor(5 + valLength, 1);
    lcd.print("%  ");
  }
  else {
    lcd.setCursor(6, 1);
    lcd.print(motorSpeed);
    lcd.setCursor(6 + valLength, 1);
    lcd.print("%  ");
  }

  if (autoSwitchState == 1) {
    // Time left on each auto sequence
    valLength = intToStringToLength(displayTime);
    lcd.setCursor(11, 1);
    lcd.print(displayTime);
    lcd.setCursor(11 + valLength, 1);
    lcd.print("s   ");
  }
  else {
    potPosition = map(manualAngleValue, 0, 1023, 0, 100);
    valLength = intToStringToLength(potPosition);
    lcd.setCursor(11, 1);
    lcd.print(potPosition);
    lcd.setCursor(11 + valLength, 1);
    lcd.print("%  ");
  }
}

void writeLCDProgramMode(int firstPercent, int firstRunTime, int secondPercent, int secondRunTime) {
  ////////////////////////////////////////// print values to LCD ///////////////////////////////////////////
  lcd.setCursor(5, 0);
  lcd.print(firstPercent);
  valLength = intToStringToLength(firstPercent);
  if (valLength == 1) {
    lcd.setCursor(5 + valLength, 0);
    lcd.print("  ");
  }
  else if (valLength == 2) {
    lcd.setCursor(5 + valLength, 0);
    lcd.print(" ");
  }
  lcd.setCursor(13, 0);
  lcd.print(firstRunTime);
  valLength = intToStringToLength(firstRunTime);
  if (valLength == 1) {
    lcd.setCursor(13 + valLength, 0);
    lcd.print("  ");
  }
  else if (valLength == 2) {
    lcd.setCursor(13 + valLength, 0);
    lcd.print(" ");
  }
  lcd.setCursor(5, 1);
  lcd.print(secondPercent);
  valLength = intToStringToLength(secondPercent);
  if (valLength == 1) {
    lcd.setCursor(5 + valLength, 1);
    lcd.print("  ");
  }
  else if (valLength == 2) {
    lcd.setCursor(5 + valLength, 1);
    lcd.print(" ");
  }
  lcd.setCursor(13, 1);
  lcd.print(secondRunTime);
  valLength = intToStringToLength(secondRunTime);
  if (valLength == 1) {
    lcd.setCursor(13 + valLength, 1);
    lcd.print("  ");
  }
  else if (valLength == 2) {
    lcd.setCursor(13 + valLength, 1);
    lcd.print(" ");
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
    if (startStopButtonState == 1 && lastStartStopButtonState == 0 && millis() - startStopTimeMillis > debounce) {
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
    startStopTimeMillis = millis();
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
    if (startStopButtonState == 1 && lastStartStopButtonState == 0 && millis() - startStopTimeMillis > debounce) {
      if (spinning == 1) {
        spinning = 0;
      }
      program11 = 0;
      program12 = 0;
      powerControl();
    }
    writeLCD();
    lastStartStopButtonState = startStopButtonState;
    startStopTimeMillis = millis();
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

void programSequence1() {
  encoderCLKState = digitalRead(encoderCLK);
  encoderDTState = digitalRead(encoderDT);
  encoderSWState = digitalRead(encoderSW);

   if ((lastEncoderDTState == 0) && (encoderDTState == 1)) {
     if (encoderCLKState == 0) {
       program11Percent++;
     } else {
       program11Percent--;
     }
     writeLCDProgramMode(program11Percent, program11RunTime, program12Percent, program12RunTime);
   } 
   lastEncoderDTState = encoderDTState;

  if (encoderSWState != lastEncoderSWState) {
    if (encoderSWState == 0 && lastEncoderSWState == 1 && millis() - encoderSWTimeMillis > debounce) {
      programModeValue++;
      if (programModeValue >= 5) {
        programModeValue = 1;
      }
      lastEncoderSWState = encoderSWState;
      encoderSWTimeMillis = millis();
    }
  }
  blinkColon();
}


void programSequence2() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Program mode 2");
}

void blinkColon() {
  if (millis() - blinkColonMillis >= 1000) {
    if (blinkChar == ":") {
      blinkChar = "=";
    }
    else {
      blinkChar = ":";
    }
    switch (programModeValue) {
      case 1:
        lcd.setCursor(4, 0);
        lcd.print(blinkChar);
        break;
      case 2:
        lcd.setCursor(12, 0);
        lcd.print(blinkChar);
        break;
      case 3:
        lcd.setCursor(4, 1);
        lcd.print(blinkChar);
        break;
      case 4:
        lcd.setCursor(12, 1);
        lcd.print(blinkChar);
        break;
    }
    blinkColonMillis = millis();
  }
}

int intToStringToLength(int val) {
  valString = String(val);
  valLength = valString.length();
  return valLength;
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

