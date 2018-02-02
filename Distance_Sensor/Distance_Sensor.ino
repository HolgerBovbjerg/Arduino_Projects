#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4

Adafruit_SSD1306 display(OLED_RESET);

const int trigPin = 7;
const int echoPin = 8;

const int sampleSize = 20;

long duration;
int distance;

int samples[sampleSize];
int average;

void doMeasurement()
{
  //Clear
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  //Send out signal for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  //Read signal
  duration = pulseIn(echoPin, HIGH);

  //Calculate distance
  distance = (duration * 0.034) / 2;
  delay(50);
}

void makeSamples()
{
  for (int i = 0; i < sampleSize; i++) {
    doMeasurement();
    samples[i] = distance;
    delay(10);
  }
}

void calcAverage()
{
  average = 0;
  int sumSamples = 0;
  for (int i = 0; i < sampleSize; i++)
  {
    sumSamples = sumSamples + samples[i];
  }
  Serial.println(sumSamples);
  average = sumSamples / sampleSize;
}

void displayDistance()
{
  //
  Serial.print("Distance: ");
  Serial.println(distance);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Afstand: "); display.print(distance); display.println("cm");
  display.display();
  delay(50);
}

void displayAverageDistance()
{
  //
  Serial.print("Distance: ");
  Serial.println(average);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Afstand: "); display.print(average); display.println("cm");
  display.display();
  delay(50 * sampleSize);
}

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(9600);
  Serial.println("Ultralydssensor med OLED-skærm");
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

}

void loop() {
  //doMeasurement();
  //displayDistance();
  makeSamples();
  calcAverage();
  displayAverageDistance();
  for ( int n = 0; n < 10; n++) {
    Serial.print(samples[n]);
    Serial.print(", ");
  } 
}
