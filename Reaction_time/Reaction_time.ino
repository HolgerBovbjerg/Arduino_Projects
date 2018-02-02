void setup() {
  // put your setup code here, to run once:'
Serial.begin(9600);
Serial.println("Lad testen starte!");
Serial.println(" ");
pinMode(13,OUTPUT);
digitalWrite(13, LOW);

pinMode(8, INPUT_PULLUP);

}

boolean isButtonPressed()
  {
    if ( digitalRead(8) == LOW )
      return true;
    else
      return false; 
  }
  
unsigned long tStart, tStop, reakTid;

void loop() {
  // put your main code here, to run repeatedly:
  delay(random(500,10000));
  Serial.println("tryk!!");
  tStart = millis();
  
  while (isButtonPressed() == false) {
    
  }

  tStop = millis();

  reakTid = tStop - tStart;

  Serial.println(reakTid);
   
}

