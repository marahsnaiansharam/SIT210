
// Detected - https://maker.ifttt.com/trigger/Sunlight_Detected/with/key/d7TiVZSgudITkzxhe4aNk1
// Not - https://maker.ifttt.com/trigger/Sunlight_Not_Detected/with/key/d7TiVZSgudITkzxhe4aNk1

#include <BH1750.h>
#include <Wire.h>
#include <WiFiNINA.h>
#include "secrets.h"

char HOST_NAME[] = "maker.ifttt.com";
String detectedPath = "/trigger/Sunlight_Detected/with/key/";
String notDetectedPath = "/trigger/Sunlight_Not_Detected/with/key/";
String queryString = "d7TiVZSgudITkzxhe4aNk1";

char ssid[] = SECRET_SSID;   
char pass[] = SECRET_PASS;

WiFiClient client;
BH1750 lightMeter;

// Initialise variables for lux values/status
const float sunlightLuxMin = 300; // Would require calibration for desired lux values
bool isInSunlight;


void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, pass);
  Wire.begin();
  lightMeter.begin();


  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  // set the initial sunlight status and values
  if(lightMeter.readLightLevel() > sunlightLuxMin)
  {
    isInSunlight = true;
  }
  else
  {
    isInSunlight = false;
  }

}

void loop() {
  
  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED)
    {
      WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  // Calls functions to check for change in light (measured in lux) value, triggers webhook to IFTTT if change is detected
  if (checkLuxChange() == true)
  {
    triggerIFTTT();
  }

}

// Determines if current lux value is above or below threshold, and whether a notification should be triggered. 
bool checkLuxChange()
{
  if (lightMeter.readLightLevel() > sunlightLuxMin && isInSunlight == false)
  {
    isInSunlight = true;
    return true;
  }
  else if (lightMeter.readLightLevel() < sunlightLuxMin && isInSunlight == true)
  {
    if (checkConsistentLux() == true)
    {
      isInSunlight = false;
      return true;
    }
    return false;
  }
  else
  {
    return false;
  }
}

// Verifies that a light value below threshold is ocurring for an extended duration so emails aren't sent during temporary reductions of light.
bool checkConsistentLux()
{
  Serial.println("Checking light consistency");
  for(int i = 1; i<20; i++)
  {
    if (lightMeter.readLightLevel() > sunlightLuxMin)
    {
      Serial.println("Light has returned to normal");
      return false;
    }
    delay (15000);
  }
  return true;
}

// Checks connection, triggers webhook to IFTTT Applet which prompts email. 
void triggerIFTTT()
{
  // Ensures connection to the IFTTT server
  if (client.connect(HOST_NAME, 80))
  {
    Serial.println("Connected to server");
  }
  else
  {
    Serial.println("Connection failed");
  }

  if (isInSunlight == true)
  {
    client.println("GET " + detectedPath + queryString + " HTTP/1.1");
    client.println("Host: " + String(HOST_NAME));
    client.println("");
    Serial.println("Webhook Triggered");
  }
  else if (isInSunlight == false)
  {
    client.println("GET " + notDetectedPath + queryString + " HTTP/1.1");
    client.println("Host: " + String(HOST_NAME));
    client.println("");
    Serial.println("Webhook Triggered");
  }
}

