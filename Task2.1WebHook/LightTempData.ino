#include "DHT.h"
#include <WiFiNINA.h>
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros


char ssid[] = SECRET_SSID;   
char pass[] = SECRET_PASS;  
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

// Initialise DHT Module

DHT dht(2, DHT22);
float temperature;

// Initialise LDR Module

int LDRSensorPin = A1;
int LDRSensorValue;

void setup() 
{
  Serial.begin(115200);  // Initialize serial **************************************

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv != "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
  dht.begin();
  ThingSpeak.begin(client);  //Initialize ThingSpeak

}

void loop() 
{

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED)
    {
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  updateChannel();
  delay(30000);

}

// Capture light sensor data
int captureLight()
{
  LDRSensorValue = analogRead(LDRSensorPin);
  
  if (LDRSensorValue < 0 || LDRSensorValue > 1023)
  {
    Serial.println("Failed to read light data");  
    return 0;
  }
  else
  {
    return LDRSensorValue;
  }
}

// Capture and validate temperature data
float captureTemp()
{
  temperature = dht.readTemperature();

  if(isnan(temperature))
  {
    Serial.println("Failed to read temperature data");
    return 0;
  }
  else
  {
    return temperature;
  }
}

// Set fields to latest data value, write to ThingSpeak channel
void updateChannel()
{
  int setLight = captureLight();
  float setTemp = captureTemp();

  // validate successful data read, set values for write
  if (setLight == 0)
  {
    Serial.println("Unable to set light data");
    return;
  }
  else { ThingSpeak.setField(1, setLight); }

  if (setTemp == 0)
  {
      Serial.println("Unable to set temperature data");
      return;
  }
  else {  ThingSpeak.setField(2, setTemp);}  

  // Attempt to write data to channel, return error code if unsuccessful
  int validateSuccess = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (validateSuccess == 200)
  {
    Serial.println("Channel updated successfully");
  }
  else
  {
    Serial.println("Channel update failed. Error Code: " + String(validateSuccess));
  }
  
}



