/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x32 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

 #include <SPI.h>
 #include <Wire.h>
 #include <Adafruit_GFX.h>
 #include <Arduino.h>
 #include <stdlib.h>
 #include <Adafruit_SSD1306.h>
 
 #define SCREEN_WIDTH 128 // OLED display width, in pixels
 #define SCREEN_HEIGHT 32 // OLED display height, in pixels
 
 #define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
 #define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
 Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
 
 unsigned long last_tick;
 int freq = 100;
 char freq_str[10];
 
 void print_rpm(const char[]);
 void freqmeter();
 
 void setup() {
   attachInterrupt(digitalPinToInterrupt(2), freqmeter, RISING);
 
   if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
     for(;;);
   }
 
   display.display();
 
   delay(1000);
 }
 
 void print_rpm(const char* rpm) {
   display.clearDisplay();
 
   display.setTextSize(5);
   display.setTextColor(SSD1306_WHITE);
   display.setCursor(0, 0);
   display.cp437(true);
 
   display.print(rpm);
   
   display.display();
 }
 
 void freqmeter() {
   if (last_tick > 0) {
     unsigned long diff = micros() - last_tick;
 
     freq = 30000000 / diff; // импульсы/сек / 2 (импульсов катушки за один оборот распредвала, но мы считаем обороты коленвала, поэтому делим на 2) * 60 (секунд)
   }
 
   last_tick = micros();
 }
 
 void loop() {
   itoa(freq, freq_str, 10);
   print_rpm(freq_str);
 
   if (last_tick > 0) {
     unsigned long diff  = micros() - last_tick;
     if (diff > 500000) {
       freq = 0;
       last_tick = 0;
     }
   }
 
   delay(40);
 }