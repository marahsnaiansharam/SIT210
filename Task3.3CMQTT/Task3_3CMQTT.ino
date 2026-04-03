#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>
#include "secrets.h"

// Define secrets
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;  

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

// Define mqtt configuration
const char broker[] = "broker.emqx.io";
int port = 1883;
const char* wave = "ES/Wave";
const char* pat = "ES/Pat";

// Set pin variables
const int ultraTrigger = 6;
const int ultraEcho = 7;
const int hallwayLED = 2;
const int bathroomLED = 3;

// Message used for mqtt
const char* uniqueMessage = "IanSharam";


void setup() {
  Serial.begin(9600);

  while(!Serial)
  {}

  // Connect to network and mqtt broker
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);

  while(WiFi.begin(ssid, pass) != WL_CONNECTED)
  {
    Serial.print("...attempting again");
    delay(5000);
  }

  Serial.println("Connected to network");

  Serial.print("Attempting to connecto to MQTT broker: ");
  Serial.println(broker);


  if (!mqttClient.connect(broker, port))
  {
    Serial.print("MQTT connection failed with error code: ");
    Serial.println(mqttClient.connectError());
  }

  // mqtt topics used for task
  mqttClient.subscribe(wave);
  mqttClient.subscribe(pat);

  // Define pinmodes
  pinMode(ultraTrigger, OUTPUT);
  pinMode(ultraEcho, INPUT);
  pinMode(hallwayLED, OUTPUT);
  pinMode(bathroomLED, OUTPUT);

  // Set the message receieve callback, determines if message is relevant, calls appropriate functions if so.
  mqttClient.onMessage(receiveMessage);
}

void loop() 
{

  // Keeps mqtt client connection active
  mqttClient.poll();

  // Checks for distance value from ultrasonic sensor value that satisfies conditions, if found, it will call the appropriate function to publish a message via the mqtt client. 
  processUltrasonic(ultrasonicDistance());
  
}

// Takes an average measurement over a duration of 1.5 seconds to prevent falsely triggering a wave at the beginning of a pat.
int ultrasonicDistance()
{
  long averageDistance = 0;
  int count = 0;
  unsigned long startTime = millis();
  while (millis() - startTime < 1500)
  {
    // Check for incoming mqtt messages during the duration
    mqttClient.poll();
    digitalWrite(ultraTrigger, LOW);
    delayMicroseconds(2);
    digitalWrite(ultraTrigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(ultraTrigger, LOW);

    long duration = pulseIn(ultraEcho, HIGH, 20000);
    int distance = duration * 0.034 /2;

    if (distance > 0)
    {
      averageDistance += distance;
      count++;
    }

    delay(10);
  }

  if (count == 0)
  {
    return 0;
  }
  
  else
  {
    return (averageDistance/count);
  }
}

// Takes average distance value as a parameter and calls the appropriate function if the condition is met
void processUltrasonic(int distance)
{
  if (distance <= 10 && distance >= 0)
  {
    Serial.println("Pat Detected: Publishing Message");
    patDetected();
  } 
  else if(distance > 10 && distance < 40)
  {
    Serial.println("Wave Detected: Publishing Message");
    waveDetected();
  }
}

// Publishes message to MQTT broker
void publishMessage(const char* topic)
{
  mqttClient.beginMessage(topic);
  mqttClient.print(uniqueMessage);
  mqttClient.endMessage();
}

// Defines action upon message being receieved.
void receiveMessage(int messageSize)
{
  Serial.println("Message receieved");
  String topic = mqttClient.messageTopic();
  String message = "";

  while (mqttClient.available())
  {
    message += (char)mqttClient.read();
  }

  if (message == uniqueMessage)
  {
    if (topic == wave)
    {
      lightsOn();
    }
    else if (topic == pat)
    {
      lightsOff();
    }
  }
}

// Functions to control lights
void lightsOn()
{
  digitalWrite(hallwayLED, HIGH);
  digitalWrite(bathroomLED, HIGH);
}

void lightsOff()
{
  digitalWrite(hallwayLED, LOW);
  digitalWrite(bathroomLED, LOW);
}

// Functions to publish message based on topic
void patDetected()
{
  publishMessage(pat);
  delay(1000);
}

void waveDetected()
{
  publishMessage(wave);
  delay(1000);
}