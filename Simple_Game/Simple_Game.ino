#include <Joystick_Shield.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>


#define OLED_RESET 4

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


Adafruit_SSD1306 myDisplay(OLED_RESET);  // create myDisplay varaibale/object  named myDisplay

void setup() {
  // put your setup code here, to run once:
myDisplay.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize OLED  with the I2C addr 0x3C (for the 128x32)
myDisplay.clearDisplay();
myDisplay.display();

for (int i = 0; i <= 8; i++) {
  pinMode(i, INPUT_PULLUP);
}
}

Joystick joystick;

void loop() {
  // put your main code here, to run repeatedly:
  joystick.button_Pressed();
  
  delay(10);
}


