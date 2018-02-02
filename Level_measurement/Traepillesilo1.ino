#include <NewPing.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

// Måler niveau i træpillesilo og sender til MQTT server
#define TRIGGER_PIN  4  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     5  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
long lastMsg = 0;
float h = 0;
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

const char* ssid = "SSAV4";
const char* password = "6180jens";
const char* mqtt_server = "192.168.1.202";

WiFiClient espClient;
PubSubClient client(espClient);
long duration, cm;
void setup() {
Serial.begin(115200);
Serial.println();
Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);

WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected");
client.setServer(mqtt_server, 1883);
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient_level_sensor")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
 
 void loop() {
  
  duration = sonar.ping_median(50);
 // convert the duration into a distance
 
 cm = (duration/2) / 29.1; 
  
  
h = 160-cm;
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  
  long now = millis();
  if (now - lastMsg > 600000) {
    lastMsg = now;
   
   
   if((h > 10) && (h <160))
   //   {
      
    // client.publish("Siloniveau", String(h).c_str(),TRUE);
      client.publish("Siloniveau", String(h).c_str());
      }
//  }

}
}
