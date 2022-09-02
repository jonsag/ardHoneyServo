/*
  ardHoneyServo
  by Jon Sagebrand
  jonsagebrand@gmail.com
  github.com/jonsag/ardHoneyServo
*/

///// initialize the LCD library with the numbers of the interface pins
// LiquidCrystal lcd(7, 6, 5, 4, 3, 2); // 6 pin LCD
LiquidCrystal595 lcd(5, 6, 7); // 3 pin LCD: datapin, latchpin, clockpin
// Pin 5 - D - Data Enable/SER
// Pin 6 - Clock/SCL
// Pin 7 - SCK
/*
 74595
 DIP				Arduino
 10 - Reset - GND
 11 - Shift Clock - 7
 12 - Latch Clock - 6
 13 - Output Enable - Vcc
 14 - Data - 5
 */

///// analog inputs
int manualAnglePin = 0; // analog pin used to connect the potentiometer
int maxAnglePin = 1;    // analog pin used to set max angle
int zeroOffsetPin = 2;  // analog pin used to set zero angle

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

///// Digital outputs:
int spinnerRelay = 4; // pin for spinner relay
bool spinning = 0;    // high if spinner is spinning

///// Digital inputs:
///// Buttons:
const int startStopButton = 11;
const int program1Button = 12;
const int program2Button = 13;

int startStopButtonState = 0;
int program1ButtonState = 0;
int program2ButtonState = 0;

int startStopButtonStateLast = 0;
int program1ButtonStateLast = 0;
int program2ButtonStateLast = 0;

///// Man/Auto-switch
const int autoSwitch = 8;
// const int manSwitch = 1;
int autoSwitchState = 0;
// int manSwitchState = 0;
int autoSwitchStateLast = 0;
// inst manSwitchStateLast = 0;

///// Left/Right-switch
const int leftSwitch = 10;
// const int rightSwitch = 10;
int leftSwitchState = 0;
// int rightSwitchState = 1;
int leftSwitchStateLast = 0;
// int rightSwitchStateLast = 0;

///// Rotary encoder
const int encoderCLK = 1; // Rotary encoder CLK signal
const int encoderDT = 2;  // Rotary encoder DT signal
const int encoderSW = 3;  // Rotary encoders switch, goes LOW when pressed
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