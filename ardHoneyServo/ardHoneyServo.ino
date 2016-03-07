/*
  ardHoneyServo
  by Jon Sagebrand
  jonsagebrand@gmail.com
  github.com/jonsag/ardHoneyServo
*/

#include <Servo.h>
#include <LiquidCrystal.h>

Servo myservo;  // create servo object to control a servo

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

int potPin = 0;  // analog pin used to connect the potentiometer
int potVal;    // variable to read the value from the analog pin
int lastPotVal = 0; // stores last value from pot
int maxAngle = 70; // maximum degrees for the servo in each direction
String motorSpeedString = "";
int motorSpeedLength = 0;
int servoAngle = 0;
int motorSpeed = 0;
int servoLow = 0;
int servoHigh = 0;

// Outputs:
int spinnerRelay = 1;

int spinning = 0;

// Buttons:
const int startStopButton = 11;
const int program1Button = 12;
const int program2Button = 13;

int startStopButtonState = 0;
int program1ButtonState = 0;
int program2ButtonState = 0;

int lastStartStopButtonState = 0;
int lastProgram1ButtonState = 0;
int lastProgram2ButtonState = 0;

// Man/Auto-switch
const int autoSwitch = 8;
//const int manSwitch = 1;

int autoSwitchState = 0;
//int manSwitchState = 0;

int lastAutoSwitchState = 0;
// inst lastManSwitchState = 0;

// Left/Right-switch
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
int program11Angle = 25;
long program11Time = 2000;
long program11StartTime = 0;
int program12 = 0;
int program12Angle = 50;
long program12Time = 2000;
long program12StartTime = 0;

int program21 = 0;
int program21Angle = 50;
long program21Time = 2000;
int program22 = 0;
int program22Angle = 75;
long program22Time = 2000;

void setup() {
  //Serial.begin(9600);      // open the serial port at 9600 bps:
  //Serial.println("Booting...");

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.print("Booting...");

  //Serial.println("Initializing in/outputs...");
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

  //Serial.println("Attaching servo...");
  lcd.setCursor(0, 1);
  lcd.print("Attaching servo ");
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  lcd.clear();
}

void loop() {
  potVal = analogRead(potPin); // reads the value of the potentiometer (value between 0 and 1023)
  startStopButtonState = digitalRead(startStopButton);
  program1ButtonState = digitalRead(program1Button);
  program2ButtonState = digitalRead(program2Button);
  autoSwitchState = digitalRead(autoSwitch);
  //manSwitchState = digitalRead(manSwitch);
  leftSwitchState = digitalRead(leftSwitch);
  //rightSwitchState = digitalRead(rightSwitch);

  checkStatusChange();

  if (autoSwitchState == 1) {
    autoRun();
  }
  else {
    manualRun();
  }
}

void writeLCD() {
  lcd.setCursor(0, 0);
  if (autoSwitchState == 1) {
    lcd.print("Auto  ");
  }
  else {
    lcd.print("Manual");
  }

  lcd.setCursor(7, 0);
  if (spinning == 1) {
    lcd.print("Running");
  }
  else {
    lcd.print("Stopped");
  }

  lcd.setCursor(0, 1);
  if (leftSwitchState == 1) {
    lcd.print("Left ");
  }
  else {
    lcd.print("Right");
  }

  if (leftSwitchState == 1) {
    motorSpeed = map(servoAngle, 90, 90 - maxAngle, 0, 100);
  }
  else {
    motorSpeed = map(servoAngle, 90, 90 + maxAngle, 0, 100);
  }
  motorSpeedString = String(motorSpeed);
  motorSpeedLength = motorSpeedString.length();
  //lcd.setCursor(7, 0);
  //lcd.print(motorSpeedLength);
  lcd.setCursor(6, 1);
  lcd.print("Speed:    ");
  lcd.setCursor(12, 1);
  lcd.print(motorSpeed);
  lcd.setCursor(12 + motorSpeedLength, 1);
  lcd.print("%");
}

/*
  void writeSerial() {
  char buffer [100];
  i = sprintf (buffer, "Pot value: %d, Start/Stop Button: %d, Program 1: %d, Program 2: %d, Auto: %d, Left: %d", potVal, startStopButtonState, program1ButtonState, program2ButtonState, autoSwitchState, leftSwitchState);
  for (int l = 0; l <= i; l++) {
    Serial.print(buffer[l]);
  }
  Serial.println();
  }
*/

void checkStatusChange() {
  if (potVal < lastPotVal - 5 || potVal > lastPotVal + 5 && autoSwitchState == 0) {
    servoControl();
    //writeSerial();
    writeLCD();
    lastPotVal = potVal;
  }
  if (startStopButtonState != lastStartStopButtonState) {
    if (startStopButtonState == 1 && lastStartStopButtonState == 0 && millis() - startStopTime > debounce && autoSwitchState == 0) {
      if (spinning == 1) {
        spinning = 0;
      }
      else {
        spinning = 1;
      }
      powerControl();
    }
    //writeSerial();
    writeLCD();
    lastStartStopButtonState = startStopButtonState;
    startStopTime = millis();
  }
  if (program1ButtonState != lastProgram1ButtonState) {
    if (program1ButtonState == 1 && lastProgram1ButtonState == 0) {
      program11 = 1;
    }
    //writeSerial();
    writeLCD();
    lastProgram1ButtonState = program1ButtonState;
  }
  if (program2ButtonState != lastProgram2ButtonState) {
    //writeSerial();
    writeLCD();
    lastProgram2ButtonState = program2ButtonState;
  }
  if (autoSwitchState != lastAutoSwitchState) {
    servoControl();
    //writeSerial();
    writeLCD();
    lastAutoSwitchState = autoSwitchState;
  }
  if (leftSwitchState != lastLeftSwitchState) {
    servoControl();
    //writeSerial();
    writeLCD();
    lastLeftSwitchState = leftSwitchState;
  }
}

void manualRun() {
  if (leftSwitchState == 1) {
    servoLow = 90 - maxAngle;
    servoHigh = 90;
  }
  else {
    servoLow = 90 + maxAngle;
    servoHigh = 90;
  }
  servoControl();
}

void autoRun() {
  if (program11 == 1) {
    if (leftSwitchState == 1) {
      servoAngle = 90 - program11Angle;
    }
    else {
      servoAngle = 90 + program11Angle;
    }
    spinning = 1;
    writeLCD();
    powerControl();
    program11StartTime = millis();
  }
  if (program11 == 1 && program11Time >= millis() - program11StartTime) {
    if (leftSwitchState == 1) {
      servoAngle = 90 - program12Angle;
    }
    else {
      servoAngle = 90 + program12Angle;
    }
    writeLCD();
    servoControl();
    program12StartTime = millis();
  }
  if (program12 == 1 && program12Time >= millis() - program12StartTime) {
    servoAngle = 90;
    spinning = 0;
    writeLCD();
    powerControl();
    program11StartTime = 0;
    program12StartTime = 0;
  }
}

void powerControl() {
  servoControl();
  digitalWrite(spinnerRelay, spinning);
}

void servoControl() {
  if (spinning == 0) {
    servoAngle = 90;
  }
  else if (autoSwitchState == 0) {
    servoAngle = map(potVal, 1023, 0, servoLow, servoHigh);
  }

  myservo.write(servoAngle);                  // sets the servo position according to the scaled value
  delay(15);                           // waits for the servo to get there
}

