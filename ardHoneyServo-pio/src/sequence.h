
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
    else if (programModeValue == 2) // setting first time
    {
      //
    }
    else if (programModeValue == 3) // setting second servo degrees
    {
      //
    }
    else // setting second time
    {
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
