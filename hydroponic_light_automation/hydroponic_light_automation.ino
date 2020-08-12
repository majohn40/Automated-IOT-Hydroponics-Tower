
//V12 is Light Time Input Widget

//---------------------------------------Include Statements-------------------------------//
//Blynk Includes
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>


//-----------------------------------------Blynk Wifi Setup----------------------------------//
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "SnFizjvPp3BxkziE5TxCKfnnJldUGW33";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "NETGEAR10";
char pass[] = "redboat523";

void setup(){
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass, "192.168.1.35", 8080);
}

void loop(){
  Blynk.run();
}
