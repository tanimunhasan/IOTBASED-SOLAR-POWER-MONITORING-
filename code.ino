#include <WiFi.h>
#include "ThingSpeak.h"
const char* ssid = "COVID19";   // your network SSID (name)
const char* password = "Corona20@";   // your network password

WiFiClient  client;

unsigned long myChannelNumber = 1;
const char * myWriteAPIKey = "ZDBT9DGPSAXG9UFK";

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;
//const int current = D2;
// Variable to hold temperature readings
int analogInput = 0;
float vout = 0.0;
float vin = 0.0;
int value = 0;
float R1 = 2179.0; // resistance of R1
float R2 = 990.0; // resistance of R2


void setup() {
  Serial.begin(115200);  //Initialize serial

  WiFi.mode(WIFI_STA);

  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {

    // Connect or reconnect to WiFi
    if (WiFi.status() != WL_CONNECTED) {
      Serial.print("Attempting to connect");
      while (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(ssid, password);
        delay(5000);
      }
      Serial.println("\nConnected.");
    }

    // Get a new   reading
    value = analogRead(analogInput);
    vout = (value * 5.0) / 1024.0;
    vin = vout / (R2 / (R1 + R2));
    if (vin < 0.09) {
      vin = 0.0; //statement to quash undesired reading !
      delay(800);
    }
    Serial.print("V: ");
    Serial.println(vin);
    //Serial.println("v");

    float amp = analogRead(2);
    amp = amp - 510;
    amp = (amp * 5.0) / 1024.0;
    amp = amp * 10;
    amp = amp;
    Serial.print("A: ");
    Serial.println(amp);

    float watt = amp * vin;
    Serial.print("W: ");
    Serial.print(watt);

    // set the fields with the values
    ThingSpeak.setField(1, amp);
    //ThingSpeak.setField(1, temperatureF);
    ThingSpeak.setField(2, vin);
    ThingSpeak.setField(3, watt);

    // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
    // pieces of information in a channel.  Here, we write to field 1.
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    if (x == 200) {
      Serial.println("Channel update successful.");
    }
    else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    lastTime = millis();
  }
}
