// include the library code:
#include <ShiftLCD.h>

// initialize the library with the numbers of the interface pins
ShiftLCD lcd(2, 4, 3);

void setup() {
  // set up the LCD's number of rows and columns: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Hello, World!");
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(millis()/1000);
}


/*
Flagrantly stolen from 
https://github.com/Chris--A/ShiftLCD_Fixed/blob/master/examples/HelloWorld/HelloWorld.ino
and used as a proof of concept.
*/