const int LED1 = 13;
const int button1 = 10;
const int button2 = 11;
const int noOfMeasurements = 5;


struct measure_t {
  int measurement;
}

struct measure_t reactionTime[noOfMeasurements];

void configureSerial()
{
  Serial.begin(9600);
  Serial.println("Serial configured");
}

void configurePins()
{
  pinMode(LED1, OUTPUT);
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  Serial.println("Pins configured");
}

void configureData()
{

}

void welcome()
{
  Serial.println("Reactiontime program");
  delay(5000);
}

void waitSomeTime()
{
  delay(random(1000, 3000));
}

void waitForButtonPress()
{
  while (digitalRead(button1) == HIGH)
  {
  }
}

unsigned long doOneReactMeasurement()
{
  unsigned long startT, stopT;
  Serial.println("Taking measurement");
  waitSomeTime();
  digitalWrite(LED1, HIGH);
  startT = millis();
  waitForButtonPress();
  digitalWrite(LED1, LOW);
  stopT = millis();
  return (stopT - startT);
}

void saveTime(unsigned long Time, int space)
{
  Serial.println("Saving measurement");
  Serial.println(Time);
  if (cheatTest(Time) == 1)
  {
    Serial.println("You are a cheater!");
    Time = 2000;
  }
  

}

int cheatTest(unsigned long Time)
{
  if (Time < 100)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

void setup() {
  configureSerial();
  configurePins();
  configureData();
  welcome();
}

void loop() {
  while (i = 0, i <= noOfMeasurements; i++)
  {
    unsigned long reactTime;
    delay(3000);
    reactTime = doOneReactMeasurement();
    saveTime(reactTime);
  }
  Serial.print("Test done");
  delay(10000);
}
