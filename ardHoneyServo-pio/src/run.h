
void manualRun() // Potentiometer
{
  if (manualAngleValue < manualAngleValueLast - 5 || manualAngleValue > manualAngleValueLast + 5)
  {
    powerControl();
    writeLCD();
    manualAngleValueLast = manualAngleValue;
  }

  if (startStopButtonState != startStopButtonStateLast) // Start/Stop-button
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

  if (leftSwitchState != leftSwitchStateLast) // Left/Right-switch toggled
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
  // Checking inputs
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

  if (startStopButtonState != startStopButtonStateLast) // Start/Stop-button toggled
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

  /**********
   *  Auto sequences
   **********/
  // Program 1
  if (program11 == 1)
  {               // Program 1-1 starts
    spinning = 1; // Rotation starts
    if (leftSwitchState == 1) // Check direction switch and set servo angle
    {
      servoAngle = 90 - maxAngle * program11Percent / 100;
    }
    else
    {
      servoAngle = 90 + maxAngle * program11Percent / 100;
    }
    writeLCD();
    powerControl();
  }
  if (program11 == 1 && millis() - program11StartTime > (program11RunTime * 1000)) // Program 1-2 starts
  {
    program11 = 0; // Stop sequence 1-1
    program12 = 1;
    program12StartTime = millis();

    if (leftSwitchState == 1) // Check direction switch and set servo angle
    {
      servoAngle = 90 - maxAngle * program12Percent / 100;
    }
    else
    {
      servoAngle = 90 + maxAngle * program12Percent / 100;
    }
    writeLCD();
    powerControl();
  }

  if (program12 == 1 && millis() - program12StartTime > (program12RunTime * 1000)) // Last sequence finished
  {
    servoAngle = 90;
    spinning = 0;
    writeLCD();
    powerControl();
    program12 = 0;
  }

  if (spinning == 1) // Running time left
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
