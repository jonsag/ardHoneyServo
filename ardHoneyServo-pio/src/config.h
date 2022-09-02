/*
  ardHoneyServo
  by Jon Sagebrand
  jonsagebrand@gmail.com
  github.com/jonsag/ardHoneyServo
*/

#include <Servo.h>
#include <LiquidCrystal595.h> // 3 pin LCD
//#include <LiquidCrystal.h> // 6 pin LCD

Servo myServo; // create servo object to control a servo

/**********
 * Digital pins
 **********/
#define encoderCLK 2 // Rotary encoder CLK signal
#define encoderDT 3 // Rotary encoder DT signal
#define encoderSW 4  // Rotary encoders switch, goes LOW when pressed

#define spinnerRelay 5 // pin for spinner relay

#define servoPin 6

#define autoSwitch 9
#define leftSwitch 10
#define startStopButton 11
#define program1Button 12
#define program2Button 13

/**********
 * Analog pins
 **********/
#define manualAnglePin A0 // analog pin used to connect the potentiometer
#define maxAnglePin A1    // analog pin used to set max angle
#define zeroOffsetPin A2  // analog pin used to set zero angle

#define data A3
#define latch A4
#define clock A5

/**********
 * LCD
 **********/
// LiquidCrystal lcd(7, 6, 5, 4, 3, 2); // 6 pin LCD
LiquidCrystal595 lcd(data, latch, clock); // 3 pin LCD: datapin, latchpin, clockpin
/*
 74595
 DIP				Arduino
 10 - Reset - GND
 11 - Shift Clock - 7
 12 - Latch Clock - 6
 13 - Output Enable - Vcc
 14 - Data - 5
 */

int manualAngleValue;         // variable to read the value from the analog pin
int manualAngleValueLast = 0; // stores last value from pot
String potString = "";        // holds pot value as string
int potLength = 0;            // holds number of characters for potString
int potPosition = 0;          // holds position of print to lcd

int maxAngleValue;   // variable to read the value from analog pin
int zeroOffsetValue; // variable to read the value from analog pin

// String motorSpeedString = ""; // holds speed as string
// int motorSpeedLength = 0; // holds number of characters for motorSpeedString

int maxAngle;   // maximum degrees for the servo in each direction
int zeroOffset; // value for zero offset of servo
int servoAngle = 0;
int motorSpeed = 0;
int servoLow = 0;
int servoHigh = 0;

bool spinning = 0;    // high if spinner is spinning

int startStopButtonState = 0;
int program1ButtonState = 0;
int program2ButtonState = 0;

int startStopButtonStateLast = 0;
int program1ButtonStateLast = 0;
int program2ButtonStateLast = 0;

///// Man/Auto-switch
// const int manSwitch = 1;
int autoSwitchState = 0;
// int manSwitchState = 0;
int autoSwitchStateLast = 0;
// inst manSwitchStateLast = 0;


// const int rightSwitch = 10;
int leftSwitchState = 0;
// int rightSwitchState = 1;
int leftSwitchStateLast = 0;
// int rightSwitchStateLast = 0;

///// Rotary encoder

int encoderCLKState = 0;
int encoderDTState = 0;
int encoderSWState = 0;
int encoderDTStateLast = 0;
int encoderSWStateLast = 0;

long startStopTimeMillis = 0; // the last time Start/Stop was toggled
long encoderSWTimeMillis = 0; // the last time encoder button was toggled
long debounce = 200;          // the debounce time, increase if the output flickers

bool program11 = 0;                  // set when in auto sequence 1-1
int program11Percent = 25;           // initial value, in percent, for servo in auto sequence 1-1
unsigned long program11RunTime = 60; // initial time, in seconds, for auto sequence 1-1
unsigned long program11StartTime = 0;
bool program12 = 0;                   // set when in auto sequence 1-2
int program12Percent = 50;            // initial value, in percent,  for servo in auto sequence 1-2
unsigned long program12RunTime = 120; // initial time, in seconds, for auto sequence 1-2
unsigned long program12StartTime = 0;
bool programMode1 = 0;
int programModeValue = 0; // which of the values are being programmed
unsigned long blinkColonMillis;
// bool blinkOut = 0;
String blinkChar = "=";

bool program21 = 0;                   // set when in auto sequence 2-1
int program21Percent = 50;            // initial value, in percent,  for servo in auto sequence 2-1
unsigned long program21RunTime = 30;  // initial time, in seconds, for auto sequence 2-1
bool program22 = 0;                   // set when in auto sequence 2-2
int program22Percent = 75;            // initial value, in percent,  for servo in auto sequence 2-2
unsigned long program22RunTime = 150; // initial time in, in seconds, for auto sequence 2-2
bool programMode2 = 0;

int displayTime = 0;
// String timeString = "";
// int timeLength = 0;

String valString = ""; // for calculation of length of values
int valLength;

int i = 0; // just a counter