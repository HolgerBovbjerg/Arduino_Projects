const int portNr = 8;
const int LED = 13;
const int nrMaal = 5;
const int nrBlink = 4;
const int minn = 1200;
const int maxx = 2500;

unsigned long maalinger[nrMaal]; // maal nr 0,1,2,3,4 naar nrMaal er 5

unsigned long middelVaerdi;

void initHW() // Initialising pins and Serial.
{
  Serial.begin(9600);
  pinMode(portNr, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
}

unsigned long doSingleMeasurement()
{
  unsigned long tStart, tStop;
  delay(random(minn, maxx));
  tStart = millis();
  digitalWrite(LED,HIGH);

  while(digitalRead(portNr) == HIGH)
  {
   //do nothing  
  }

  tStop = millis();
  digitalWrite(LED, LOW);

  return (tStop - tStart);
}


void setup() {
  // put your setup code here, to run once:
  initHW();
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(doSingleMeasurement());
}
