
#include "DHT.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define DHTPIN 2
#define DHTTYPE DHT22
#define OLED_RESET 4
DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(OLED_RESET);


void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
  Serial.println("DHT22 med OLED-skærm");
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  dht.begin();  
}

void loop() {
  // put your main code here, to run repeatedly:
delay(2000);

float h = dht.readHumidity();
float t = dht.readTemperature();
float hic = dht.computeHeatIndex(t, h, false);

Serial.print("Luftfugtighed: ");
Serial.print(h);
Serial.print(" %\t");
Serial.print("Temperatur: ");
Serial.print(t);
Serial.print(" *C\t");
Serial.print("Varmeindex: ");
Serial.print(hic);
Serial.println(" *C ");

display.clearDisplay();
display.setTextSize(1);
display.setTextColor(WHITE);
display.setCursor(0,0);
display.print("Temperatur:   "); display.print(t); display.print((char)247);display.println("C");
display.setTextSize(1);
display.setTextColor(WHITE); 
display.print("Luftfugtighed:"); display.print(h); display.println("%");
display.setTextSize(1);
display.setTextColor(WHITE); 
display.print("Varmeindex:   "); display.print(hic); display.print((char)247);display.println("C");
display.display();
delay(2000);

if(isnan(h) || isnan(t)) {
  Serial.println("Kunne ikke læse fra data fra sensoren");
  return;
}




}
