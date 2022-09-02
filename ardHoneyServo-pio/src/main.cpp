#include <Arduino.h>

#include <Servo.h>
#include <LiquidCrystal595.h> // 3 pin LCD
//#include <LiquidCrystal.h> // 6 pin LCD

Servo myServo; // create servo object to control a servo

void setup()
{
  ///// set up the LCD's number of columns and rows:
  lcd.begin(16, 2); // define LCD layout
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
  // pinMode(manSwitch, INPUT);
  pinMode(leftSwitch, INPUT);
  // pinMode(rightSwitch, INPUT);
  pinMode(spinnerRelay, OUTPUT);
  pinMode(encoderCLK, INPUT);
  pinMode(encoderDT, INPUT);
  pinMode(encoderSW, INPUT);
  lcd.setCursor(0, 1);
  lcd.print("Attaching servo ");
  myServo.attach(9); // attaches the servo on pin 9 to the servo object
  delay(500);
  lcd.clear();
}

void loop()
{
  manualAngleValue = analogRead(manualAnglePin); // reads the value of the potentiometer (value between 0 and 1023)
  maxAngleValue = analogRead(maxAnglePin);
  zeroOffsetValue = analogRead(zeroOffsetPin);

  startStopButtonState = digitalRead(startStopButton);
  program1ButtonState = digitalRead(program1Button);
  program2ButtonState = digitalRead(program2Button);
  autoSwitchState = digitalRead(autoSwitch);
  // manSwitchState = digitalRead(manSwitch);
  if (spinning == 0)
  { // Don't change direction during spinning
    leftSwitchState = digitalRead(leftSwitch);
    // rightSwitchState = digitalRead(rightSwitch);
  }

  ///// Check if Manual/Auto-switch is switched. Then stop everything
  if (autoSwitchState != autoSwitchStateLast)
  {
    spinning = 0;
    servoAngle = 0;
    powerControl();
    writeLCD();
    autoSwitchStateLast = autoSwitchState;
  }

  ///// Set zero offset for servo
  zeroOffset = map(zeroOffsetValue, 1023, 0, -25, 25);
  ///// Set max angle for servo
  maxAngle = map(maxAngleValue, 0, 1023, 0, 90);

  if (autoSwitchState == 0 && program1ButtonState == 1 && spinning == 0 && programMode1 != 1)
  {
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
  if (programMode1 == 1 && startStopButtonState == 1)
  {
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
    writeLCD();
  }

  if (programMode1 == 1)
  {
    programSequence1();
  }
  else if (autoSwitchState == 0)
  {
    manualRun();
  }
  else
  {
    autoRun();
  }
}

void writeLCD()
{
  ///// Manual/Auto
  lcd.setCursor(0, 0);
  if (autoSwitchState == 1)
  {
    lcd.print("Auto  ");
  }
  else
  {
    lcd.print("Manual");
  }

  ///// Stopped/Running
  lcd.setCursor(8, 0);
  if (spinning == 1 && autoSwitchState == 0)
  {
    lcd.print("Running ");
  }
  else if (spinning == 1 && program11 == 1)
  {
    lcd.print("Prgm 1-1");
  }
  else if (spinning == 1 && program12 == 1)
  {
    lcd.print("Prgm 1-2");
  }
  else if (spinning == 1 && program21 == 1)
  {
    lcd.print("Prgm 2-1");
  }
  else if (spinning == 1 && program11 == 1)
  {
    lcd.print("Prgm 2-2");
  }
  else if (spinning == 0)
  {
    lcd.print("Stopped ");
  }
  else
  {
    lcd.print("Error...");
  }

  ///// Direction and speed
  lcd.setCursor(0, 1);
  if (leftSwitchState == 1)
  {
    lcd.print("Left:");
  }
  else
  {
    lcd.print("Right:");
  }

  if (leftSwitchState == 1)
  {
    motorSpeed = map(servoAngle, 90, 90 - maxAngle, 0, 100);
  }
  else
  {
    motorSpeed = map(servoAngle, 90, 90 + maxAngle, 0, 100);
  }

  valLength = intToStringToLength(motorSpeed);
  if (leftSwitchState == 1)
  {
    lcd.setCursor(5, 1);
    lcd.print(motorSpeed);
    lcd.setCursor(5 + valLength, 1);
    lcd.print("%  ");
  }
  else
  {
    lcd.setCursor(6, 1);
    lcd.print(motorSpeed);
    lcd.setCursor(6 + valLength, 1);
    lcd.print("%  ");
  }

  if (autoSwitchState == 1)
  {
    // Time left on each auto sequence
    valLength = intToStringToLength(displayTime);
    lcd.setCursor(11, 1);
    lcd.print(displayTime);
    lcd.setCursor(11 + valLength, 1);
    lcd.print("s   ");
  }
  else
  {
    potPosition = map(manualAngleValue, 0, 1023, 0, 100);
    valLength = intToStringToLength(potPosition);
    lcd.setCursor(11, 1);
    lcd.print(potPosition);
    lcd.setCursor(11 + valLength, 1);
    lcd.print("%  ");
  }
}

void writeLCDProgramMode(int firstPercent, int firstRunTime, int secondPercent, int secondRunTime)
{
  ////////////////////////////////////////// print values to LCD ///////////////////////////////////////////
  lcd.setCursor(5, 0);
  lcd.print(firstPercent);
  valLength = intToStringToLength(firstPercent);
  if (valLength == 1)
  {
    lcd.setCursor(5 + valLength, 0);
    lcd.print("  ");
  }
  else if (valLength == 2)
  {
    lcd.setCursor(5 + valLength, 0);
    lcd.print(" ");
  }
  lcd.setCursor(13, 0);
  lcd.print(firstRunTime);
  valLength = intToStringToLength(firstRunTime);
  if (valLength == 1)
  {
    lcd.setCursor(13 + valLength, 0);
    lcd.print("  ");
  }
  else if (valLength == 2)
  {
    lcd.setCursor(13 + valLength, 0);
    lcd.print(" ");
  }
  lcd.setCursor(5, 1);
  lcd.print(secondPercent);
  valLength = intToStringToLength(secondPercent);
  if (valLength == 1)
  {
    lcd.setCursor(5 + valLength, 1);
    lcd.print("  ");
  }
  else if (valLength == 2)
  {
    lcd.setCursor(5 + valLength, 1);
    lcd.print(" ");
  }
  lcd.setCursor(13, 1);
  lcd.print(secondRunTime);
  valLength = intToStringToLength(secondRunTime);
  if (valLength == 1)
  {
    lcd.setCursor(13 + valLength, 1);
    lcd.print("  ");
  }
  else if (valLength == 2)
  {
    lcd.setCursor(13 + valLength, 1);
    lcd.print(" ");
  }
}

void manualRun()
{
  ////////////////////////////////////////// Checking inputs ///////////////////////////////////////////
  ///// Potentiometer
  if (manualAngleValue < manualAngleValueLast - 5 || manualAngleValue > manualAngleValueLast + 5)
  {
    powerControl();
    writeLCD();
    manualAngleValueLast = manualAngleValue;
  }
  ///// Start/Stop-button
  if (startStopButtonState != startStopButtonStateLast)
  {
    if (startStopButtonState == 1 && startStopButtonStateLast == 0 && millis() - startStopTimeMillis > debounce)
    {
      if (spinning == 1)
      {
        spinning = 0;
      }
      else
      {
        spinning = 1;
      }
      // program11 = 0;
      // program12 = 0;
      powerControl();
    }
    writeLCD();
    startStopButtonStateLast = startStopButtonState;
    startStopTimeMillis = millis();
  }
  ///// Left/Right-switch
  if (leftSwitchState != leftSwitchStateLast)
  {
    powerControl();
    writeLCD();
    leftSwitchStateLast = leftSwitchState;
  }
  if (leftSwitchState == 1)
  {
    servoLow = 90 - maxAngle;
    servoHigh = 90;
  }
  else
  {
    servoLow = 90 + maxAngle;
    servoHigh = 90;
  }
  powerControl();
}

void autoRun()
{
  ////////////////////////////////////////// Checking inputs ///////////////////////////////////////////

  if (program1ButtonState != program1ButtonStateLast)
  {
    if (program1ButtonState == 1 && program1ButtonStateLast == 0 && millis() - program11StartTime > debounce)
    {
      program11 = 1;
      program11StartTime = millis();
    }
    writeLCD();
    program1ButtonStateLast = program1ButtonState;
  }
  /*
    if (program2ButtonState != program2ButtonStateLast) {
      writeLCD();
      program2ButtonStateLast = program2ButtonState;
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
  if (startStopButtonState != startStopButtonStateLast)
  {
    if (startStopButtonState == 1 && startStopButtonStateLast == 0 && millis() - startStopTimeMillis > debounce)
    {
      if (spinning == 1)
      {
        spinning = 0;
      }
      program11 = 0;
      program12 = 0;
      powerControl();
    }
    writeLCD();
    startStopButtonStateLast = startStopButtonState;
    startStopTimeMillis = millis();
  }

  ////////////////////////////////////////// Auto sequences ///////////////////////////////////////////
  ///// Program 1
  if (program11 == 1)
  {               // Program 1-1 starts
    spinning = 1; // Rotation starts
    if (leftSwitchState == 1)
    { ///// Check direction switch and set servo angle
      servoAngle = 90 - maxAngle * program11Percent / 100;
    }
    else
    {
      servoAngle = 90 + maxAngle * program11Percent / 100;
    }
    writeLCD();
    powerControl();
  }
  if (program11 == 1 && millis() - program11StartTime > (program11RunTime * 1000))
  {                // Program 1-2 starts
    program11 = 0; // Sequence 1-1 stops
    program12 = 1;
    program12StartTime = millis();
    if (leftSwitchState == 1)
    { ///// Check direction switch and set servo angle
      servoAngle = 90 - maxAngle * program12Percent / 100;
    }
    else
    {
      servoAngle = 90 + maxAngle * program12Percent / 100;
    }
    writeLCD();
    powerControl();
  }

  if (program12 == 1 && millis() - program12StartTime > (program12RunTime * 1000))
  { // Last sequence finished
    servoAngle = 90;
    spinning = 0;
    writeLCD();
    powerControl();
    program12 = 0;
  }

  ///// Running time left
  if (spinning == 1)
  {
    if (program11 == 1)
    {
      displayTime = ((program11RunTime * 1000) - millis() + program11StartTime) / 1000;
    }
    else if (program12 == 1)
    {
      displayTime = ((program12RunTime * 1000) - millis() + program12StartTime) / 1000;
    }
  }
  else
  {
    displayTime = 0;
  }
  writeLCD();
}

void programSequence1()
{
  encoderCLKState = digitalRead(encoderCLK);
  encoderDTState = digitalRead(encoderDT);
  encoderSWState = digitalRead(encoderSW);

  if ((encoderDTStateLast == 0) && (encoderDTState == 1))
  {
    if (programModeValue == 1)
    { // setting first servo degrees
      if (encoderCLKState == 0)
      {
        program11Percent--;
        if (program11Percent <= 0)
        {
          program11Percent = 0;
        }
      }
      else
      {
        program11Percent++;
        if (program11Percent >= 100)
        {
          program11Percent = 100;
        }
      }
    }
    else if (programModeValue == 2)
    { // setting first time
      //
    }
    else if (programModeValue == 3)
    { // setting second servo degrees
      //
    }
    else
    { // setting second time
      //
    }
    writeLCDProgramMode(program11Percent, program11RunTime, program12Percent, program12RunTime);
  }
  encoderDTStateLast = encoderDTState;

  if (encoderSWState != encoderSWStateLast)
  {
    if (encoderSWState == 0 && encoderSWStateLast == 1 && millis() - encoderSWTimeMillis > debounce)
    {
      programModeValue++;
      if (programModeValue >= 5)
      {
        programModeValue = 1;
      }
      encoderSWStateLast = encoderSWState;
      encoderSWTimeMillis = millis();
    }
  }
  blinkColon();
}

void programSequence2()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Program mode 2");
}

void blinkColon()
{
  if (millis() - blinkColonMillis >= 1000)
  {
    if (blinkChar == ":")
    {
      blinkChar = "=";
    }
    else
    {
      blinkChar = ":";
    }
    switch (programModeValue)
    {
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

int intToStringToLength(int val)
{
  valString = String(val);
  valLength = valString.length();
  return valLength;
}

void powerControl()
{
  if (spinning == 0)
  {
    servoAngle = 90;
  }
  else if (autoSwitchState == 0)
  {
    servoAngle = map(manualAngleValue, 1023, 0, servoLow, servoHigh);
  }

  myServo.write(servoAngle + zeroOffset); // sets the servo position according to the scaled value
  delay(15);                              // waits for the servo to get there
  digitalWrite(spinnerRelay, spinning);
}
