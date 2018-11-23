Adding an LCD display to Arduino projects can add real value but the cost of doing so can be significant. Not a financial cost - you can pick up 16 (characters) x 2 (rows) LCD for as little as £3.50. The cost is the pin count it can take to drive them. Using the built-in LiquidCrystal Display library it can take as many as 6 pins! That does not leave much for your sensors, motors and other components.

There are many projects that discuss using alternatives - such as a much more expensive Serial LCD (£10 up). Other projects discuss using two-wire interfaces, increasing the complexity of your code. The simplest way to drive the HD44780 style LCDs, in my opinion, is to use a 74HC595 shift register, taking the pin count down to 3.

##Arduino PIN Savings, use 3 not 6##
Using this library and associated breadboard or stripboard designs you save 3 pins when connecting your LCD. Instead of the 6 required by the Arduino LiquidCrystal library, this approach only needs 3 pins and is a drop-in replacement.

##Make the Shield##
Connecting an LCD takes an enormous amount of hookup wire, either using the old 6-pin approach or via the shift register - making this shield means you can be up to prototyping in less than 6 seconds. Attach power and the three hookup wires and you're done.

  * http://rowansimms.com/article.php/lcd-hookup-in-seconds Breadboard Design
  * http://rowansimms.com/article.php/lcd-hookup-in-seconds-shield Stripboard Design aka Shield

##LiquidCrystal595 Library Download##
All downloads are available from  https://bitbucket.org/rowansimms/arduino-lcd-3pin

###Credits###

  * [Stephen Hobley](http://stephenhobley.com) deserves a lot of credit, he was the original creator of this code.
  * The [http://arduino.cc](Arduino) team for their fantastic platform.
  * [http://fritzing.org](Fritzing) for their fantastic product to prototype stripboards, breadboards, generate schematics and pcbs. Available for Mac, PC and Linux for free.