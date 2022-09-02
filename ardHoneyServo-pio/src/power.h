
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
