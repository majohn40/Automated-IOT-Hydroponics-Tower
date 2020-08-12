
//V11 is Pump Time Input Widget

//---------------------------------------Include Statements-------------------------------//
//Blynk Includes
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>

#include <TimeLib.h>
#include <WidgetRTC.h>

//--------------------------------------Timer widget setup-------------------------------//
char currentTime[9];
char startTime[9];
char stopTime[9];
int on_interval_hour;
int on_interval_min;
int on_interval_sec;
int on_interval_total;
int off_interval_hour;
int off_interval_min;
int off_interval_sec;
int off_interval_total;
int pump_change_time = 0;
int current_unix_time;

bool pump_on = false;

BlynkTimer timer;
WidgetRTC rtc;

//-----------------------------------------Blynk Wifi Setup----------------------------------//
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "wzGDTgx7aZ7-JOHkvBlY56aDpreDRjYJ";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "NETGEAR10";
char pass[] = "redboat523";

//Initialize Variable
const int relay_pin = 12;
int switch_time = 0;



void setup(){
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass, "192.168.1.35", 8080);
  pinMode(relay_pin, OUTPUT);
  rtc.begin();
  setSyncInterval(360);
  timer.setInterval(30000L, TimeCheck);  // Update Time Check every 30 seconds
  
}

BLYNK_CONNECTED() {
  Blynk.syncVirtual(V11); //  Synchronize Time Input Widget when connected
  TimeCheck(); // Initial Time Check
}


void loop(){
  Blynk.run();
  timer.run();
}

BLYNK_WRITE(V11) {  // Called whenever setting Time Input Widget
  TimeInputParam t(param);
  on_interval_hour = t.getStartHour();
  on_interval_min = t.getStartMinute();
  on_interval_sec = t.getStartSecond();
  Serial.print("On Hours: ");
  Serial.println(on_interval_hour);
  Serial.print("On Minutes: ");
  Serial.println(on_interval_min);
  Serial.print("On interval sec: ");
  Serial.println(on_interval_sec);
  on_interval_total = convertToSeconds(on_interval_hour, on_interval_min, on_interval_sec);
  
  off_interval_hour = t.getStopHour();
  off_interval_min = t.getStopMinute();
  off_interval_sec = t.getStopSecond();
  off_interval_total = convertToSeconds(off_interval_hour, off_interval_min, off_interval_sec);

  Serial.print("on_interval_total: ");
  Serial.println(on_interval_total);
  Serial.print("off_interval_total: ");
  Serial.println(off_interval_total);
  
  
}

int convertToSeconds(int hours, int minutes, int seconds){
  return (60*60*hours) + (60 * minutes)+seconds;
}



void TimeCheck() {  // call with timer every 30 seconds or so
  current_unix_time = now();
  Serial.println(current_unix_time);
  Serial.println(pump_change_time);
  if (current_unix_time >= pump_change_time){
    state_switch();
    Serial.println("State changed");
  }
}

void state_switch(){
  if (pump_on == true){
    digitalWrite(relay_pin, LOW);
    pump_change_time = current_unix_time + off_interval_total;
    pump_on = false;
    Serial.println("pump turned off");
  } else if (pump_on == false){
    digitalWrite(relay_pin, HIGH);
    pump_change_time = current_unix_time + on_interval_total;
    pump_on = true;
    Serial.println("pump turned on");
  }
}
