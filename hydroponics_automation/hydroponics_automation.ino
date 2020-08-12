//Blynk Server Pins List
//V0 -- Plant Selection Menu
//V1 --Automated Intervention Toggle
//V5 -- pH Sensor
//V6 --Electrical Conductivity
//V7 -- Water Temp
//V8 --External Temp
//V9 --humidity
//V10 -- Plant Info Table

//---------------------------------------Include Statements-------------------------------//
//Blynk Includes
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

//Temp/Humidity Includes
#include <SparkFunHTU21D.h>

//ESP Communication Includes
#include <SoftwareSerial.h>

//-----------------------------------------Blynk Wifi Setup----------------------------------//
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "GYW0DZrc42Ty9CMnItNPq1ZyWdKG_VEa";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "NETGEAR10";
char pass[] = "redboat523";

//-----------------------------------------ESP/Uno Communication Setup---------------------------//
SoftwareSerial espSerial(5,6);
const byte numChars = 32;
char receivedChars[numChars]; // an array to store the received data
char tempChars[numChars];        // temporary array for use by strtok() function

float RC_sensor, EC, ppm, water_temp, pH;

boolean newData = false;

//-----------------Info Table Setup----------------------------------//
WidgetTable table;
BLYNK_ATTACH_WIDGET(table, V10);

//-----------------Timer Initialization------------------------------//
//BlynkTimer temperatureTimer;
BlynkTimer ECTimer;

//-----------------Sensor Definitions--------------------------------//
HTU21D humiditySensor;

//-----------------Setup Plant Selection Menu------------------------//
BLYNK_WRITE(V0) {
  switch (param.asInt())
  {
    case 1: //Tomatoes
      populateTable(1);
      adjustRange(1);
      break;
    case 2: //Basil
      populateTable(2);
      adjustRange(2);
      break;
    case 3: //Strawberries
      populateTable(3);
      adjustRange(3);
      break;
      
  }
}


void setup()
{
  // Debug console
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass, "192.168.1.35", 8080);
  
  //-------------------------Open Serial port to read EC Data-----------------------------//
  while(!Serial){
    ;
  }

  //-------------------------HTU21D Initiallization---------------------------------------//
  //humiditySensor.begin();
  

  //-------------------------Setup Sensor Readings----------------------------------------//
  //temperatureTimer.setInterval(1000L, readTemperatureHumidity);
  ECTimer.setInterval(60000L, readEC);  
 
}

void loop()
{
  Blynk.run();
  //temperatureTimer.run();  
  ECTimer.run();
}

//----------------------------Sensor Reading Function Definitions--------------------------//
void readTemperatureHumidity(){
  float temperatureData,humidityData;
  temperatureData = (1.8*humiditySensor.readTemperature()) + 32;
  humidityData = humiditySensor.readHumidity();
  Blynk.virtualWrite(V8, temperatureData);
  Blynk.virtualWrite(V9, humidityData);
}

void readEC(){
  recvWithEndMarker();
  if (newData == true){
    strcpy(tempChars, receivedChars);
    parseData();
    displayParsedData();
    Blynk.virtualWrite(V5, pH);
    Blynk.virtualWrite(V6, EC);
    Blynk.virtualWrite(V7, water_temp);
    newData = false;
 }
}


//---------------------------EC Communication Functions------------------------------------//

void recvWithEndMarker() {
 static byte ndx = 0;
 char endMarker = '\n';
 char rc;
 
 // if (Serial.available() > 0) {
 while (Serial.available() > 0 && newData == false) {
   rc = Serial.read();
  
   if (rc != endMarker) {
     receivedChars[ndx] = rc;
     ndx++;
     if (ndx >= numChars) {
      ndx = numChars - 1;
     }
   }
   else {
     receivedChars[ndx] = '\0'; // terminate the string
     ndx = 0;
     newData = true;
   }
 }
}


void parseData() {
  // split the data into its parts
  char * strtokIndx; // this is used by strtok() as an index
  
  strtokIndx = strtok(tempChars,",");      // get the first part - RC
  RC_sensor = atof(strtokIndx);

  
  strtokIndx = strtok(NULL, ","); // get second part- EC25
  EC = atof(strtokIndx); 
  
  strtokIndx = strtok(NULL, ","); // get third part - ppm
  ppm = atof(strtokIndx);     // convert this part to a float

  strtokIndx = strtok(NULL, ","); //get fourth part -- temp
  water_temp = atof(strtokIndx);
    
  strtokIndx = strtok(NULL, ","); //get last part- pH
  pH = atof(strtokIndx);

}

void displayParsedData(){
  Serial.print("Rc = ");
  Serial.print(RC_sensor);
  Serial.print(" EC = ");
  Serial.print(EC);
  Serial.print(" PPM = ");
  Serial.print(ppm);
  Serial.print(" Water temp = ");
  Serial.print(water_temp);
  Serial.println("");

}

void populateTable(int cropID){
  switch (cropID){
    case 1:
      table.clear();
      table.addRow(0, "Crop: ", "Tomatoes");
      table.addRow(1, "EC: ","2.0-5.0");
      table.addRow(2, "pH range: ", "5.5-6.5");
      table.addRow(3, "Light: ","14-18 hours");
      table.addRow(4, "Temperature: ","58-79F");
      break;
    case 2:
      table.clear();
      table.addRow(0, "Crop: ", "Basil");
      table.addRow(1, "EC: ","1.6-2.2");
      table.addRow(2, "pH range: ", "6.2-6.8");
      table.addRow(3, "Light: ","14-18 hours");
      table.addRow(4, "Temperature: ","70-85F");
      break;
     case 3:
      table.clear();
      table.addRow(0, "Crop: ","Strawberries");
      table.addRow(1, "EC: ","~1.0");
      table.addRow(2, "pH range: ", "5.5-6.0");
      table.addRow(3, "Light: ","14-16 hours");
      table.addRow(4, "Temperature: ","65-70F");
      break;
  }
}

void adjustRange(int cropID){
  switch (cropID){
    case 1: //Tomatoes
      //Set EC Range
      Blynk.setProperty(V6, "min", 2.0); 
      Blynk.setProperty(V6, "max", 5.0);
      //Set pH Range
      Blynk.setProperty(V5, "min", 1.6);
      Blynk.setProperty(V5, "max", 2.2);
      break;
    case 2: //Basil
      Blynk.setProperty(V6, "min", 1.6);
      Blynk.setProperty(V6, "max", 2.2);
      Blynk.setProperty(V5, "min", 6.2);
      Blynk.setProperty(V5, "max", 6.8);
      break;
    case 3:
      Blynk.setProperty(V6, "min", 0.75);
      Blynk.setProperty(V6, "max", 1.25);
      Blynk.setProperty(V5, "min", 5.5);
      Blynk.setProperty(V5, "max", 6.0);
      break;
  }
}
