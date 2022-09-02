
void writeLCDProgramMode(int firstPercent, int firstRunTime, int secondPercent, int secondRunTime)
{
  /**********
   *  Print values to LCD
   **********/
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

void writeLCD()
{
  lcd.setCursor(0, 0);
  if (autoSwitchState == 1) // Manual/Auto
  {
    lcd.print("Auto  ");
  }
  else
  {
    lcd.print("Manual");
  }

  lcd.setCursor(8, 0);
  if (spinning == 1 && autoSwitchState == 0) // Stopped/Running
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

  lcd.setCursor(0, 1);
  if (leftSwitchState == 1) // Direction
  {
    lcd.print("Left:");
  }
  else
  {
    lcd.print("Right:");
  }

  if (leftSwitchState == 1) // Speed
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
