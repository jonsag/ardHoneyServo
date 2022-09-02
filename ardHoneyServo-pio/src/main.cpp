#include <Arduino.h>

#include <config.h>
#include <misc.h>
#include <LCD.h>
#include <power.h>
#include <run.h>
#include <sequence.h>

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
  myServo.attach(servoPin); // attaches the servo on pin 9 to the servo object
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
  
  if (spinning == 0) // Don't change direction during spinning
  {
    leftSwitchState = digitalRead(leftSwitch);
    // rightSwitchState = digitalRead(rightSwitch);
  }

  if (autoSwitchState != autoSwitchStateLast) // Check if Manual/Auto-switch is switched. If so stop everything
  {
    spinning = 0;
    servoAngle = 0;
    powerControl();
    writeLCD();
    autoSwitchStateLast = autoSwitchState;
  }

  zeroOffset = map(zeroOffsetValue, 1023, 0, -25, 25); // Set zero offset for servo
  maxAngle = map(maxAngleValue, 0, 1023, 0, 90); // Set max angle for servo

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
