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
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int potpin = 0;  // analog pin used to connect the potentiometer
int val;    // variable to read the value from the analog pin

int lastVal=0;

void setup() {
  Serial.begin(9600);      // open the serial port at 9600 bps:
  Serial.println("Booting...");
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.print("Booting...");
  
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
}

void loop() {
  val = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023)
  val = map(val, 0, 1023, 90, 180);     // scale it to use it with the servo (value between 0 and 180)
  myservo.write(val);                  // sets the servo position according to the scaled value
  delay(15);                           // waits for the servo to get there

  if (val != lastVal) {
    Serial.println(val);
    lcd.setCursor(0, 1);
    lcd.print(val);
    lastVal = val;
  }
}

