# arduino_1602_jump_game
A simple jumping game for Arduino and 16x2 LCD display.

Instructions: 
- connect the LCD via I2C and set the pins on the line `LiquidCrystal_I2C lcd(0x27, 16, 2);`
- connect a button to a digital pin and set the pin number on the line `#define BUTTON_PIN 50`
- compile, upload and play
