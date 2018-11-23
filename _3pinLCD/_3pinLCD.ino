/*
* 3-pin Arduino interface for HD44780 LCDs via 74HC595 Shift Register
*     by Rowan Simms   code@rowansimms.com
*     License: Creative Commons - Attribution.
*     Full Documentation and Description:  http://rowansimms.com/article.php/lcd-hookup-in-seconds
*
* This sketch allows Arduinos to use a shift register to control an LCD, allowing
* a reduction in pins it requires from 6 to 3 while still retaining full control
* including backlight on/off.
* This requires the use of the LiquidCrystal595 library
* available at: http://code.google.com/p/arduino-lcd-3pin/
*/

#include <LiquidCrystal595.h>    // include the library
LiquidCrystal595 lcd(5, 6, 7);     // datapin, latchpin, clockpin
// Pin 5 - Data Enable/SER
// Pin 6 - Clock/SCL
// Pin 7 - SCK

void setup() {
    lcd.begin(16,2);             // 16 characters, 2 rows

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Wow. 3 pins!");
    lcd.setCursor(0,1);
    lcd.print("Fabulous");
}

void loop() {
    // not used.
}
