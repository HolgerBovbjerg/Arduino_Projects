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


unsigned long doSingleMeasurements() //Does a single measurement
{
  unsigned long tStart, tStop;

  delay(random (minn, maxx));
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

void doMeasurements() // Makes "nrMaal" amount of measurements and saves them in the array "maalinger".
{
  int index;

  index = 0;
  
  while (index < nrMaal) {
    maalinger[index] = doSingleMeasurements();
    index++;
  }
}

unsigned long calcMeanValue() // calculates mean value of measurements.
{
  int index;
  unsigned long middel;

  index = 0;
  middel = 0;

  while (index < nrMaal) {
    
    middel = middel + maalinger[index];

    index++; 
  }
  return (middel / nrMaal);
}

void blinkToYou() // Makes the LED blink "nrBlink" amount of times to notify that the program has started.
  {
  int loopCnt;
  loopCnt = 0;
  while (loopCnt < nrBlink) {
    digitalWrite(LED, HIGH);
    delay(200);
    digitalWrite(LED, LOW);
    delay(200);
    loopCnt++;
  }
}


void displayResults () // Displays your average reaction time flollowed by each result.
{
  int index;
  Serial.println("RESULTS");
  Serial.print("Middel reaktid "); Serial.println(middelVaerdi);

  index = 0;
  while (index < nrMaal) {
  Serial.print("maal nr "); Serial.print(index + 1); Serial.print(" ");
  Serial.println(maalinger[index]);

  index++;
  }
  Serial.println("slut paa data");
}

void waitSomeTime() // Waits some time before starting new measurement.
{
  delay(4000);
}

void setup() {
  initHW(); 
}

void loop() {
  blinkToYou();
  doMeasurements();
  blinkToYou();
  middelVaerdi = calcMeanValue();
  displayResults();
  waitSomeTime();
}
