#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
//int D1 = 5;
//int D2 = 4;
 
LiquidCrystal_I2C lcd(0x27, 20, 4);

int setup() {

Serial.begin(115200);
Wire.begin(D4, D5);   //Use predefined PINS consts
lcd.begin(20,2);      // The begin call takes the width and height. This
                      // Should match the number provided to the constructor.
lcd.backlight();      // Turn on the backlight.

lcd.home();

lcd.setCursor(3, 0);  // Move the cursor at origin
lcd.print("HELLO READERS,");
lcd.setCursor(0, 1);
lcd.print("Interfacing nodemcu");
lcd.setCursor(0, 2);
lcd.print("with LCD 20X4.");
lcd.setCursor(5, 3);
lcd.print("GOOD LUCK!");

}

void loop() {         

// do nothing here 

}
