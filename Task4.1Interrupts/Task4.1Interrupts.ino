#include <BH1750.h>
#include <Wire.h>

// Set pins
const int hallwayLED = 3;
const int porchLED = 2;
const int motionSensor = 9;
const int slideSwitch = 10;

// Initialise variables 
volatile bool slideState;
volatile bool motionState;

const int sunlightLuxMin = 300;

BH1750 lightMeter;

// Arbitrary duration used to delay turning light off after last motion detected
int duration = 10000;

volatile unsigned long lastMotion;


void setup() 
{

  Serial.begin(115200);
  Wire.begin();
  lightMeter.begin();

  // define pinmodes
  pinMode(hallwayLED, OUTPUT);
  pinMode(porchLED, OUTPUT);
  pinMode(motionSensor, INPUT);
  pinMode(slideSwitch, INPUT_PULLUP);
  
  //Define interrupts
  attachInterrupt(digitalPinToInterrupt(9), motion, CHANGE);
  attachInterrupt(digitalPinToInterrupt(10), slide, CHANGE);
  
}

void loop() 
{

  // Conditional statements that trigger light behaviour.
  // String is passed to lightsOn function to ensure accurate serial output.
  // Manually turning on light with switch takes priority
  if (slideState == true)
  {
    lightsOn("Switch Activated: ");
  }
  else if(motionState == true)
  {
    if (sunlightCheck() == false) 
      {
        lightsOn("Motion Detected: ");
      }
  }
  // Checks for grace period after motion detected before turning off lights
  else if(slideState == false && motionState == false)
  {
    if (lastMotion == 0 || motionDelay(lastMotion)==true)
      {
        lightsOff();
      }
  }
}

// Checks if state has changed, turns on the lights, prints to serial. 
void lightsOn(String source)
{
  if (digitalRead(porchLED) == LOW)
  {
    digitalWrite(porchLED, HIGH);
    digitalWrite(hallwayLED, HIGH);
    Serial.println(source + "Lights On");
  }
}

// Checks if state has changed, turns off the lights, prints to serial. 
void lightsOff()
{
  if (digitalRead(porchLED) == HIGH)
  {
    digitalWrite(porchLED, LOW);
    digitalWrite(hallwayLED, LOW);
    Serial.println("Lights Off");
  }
}

// Defines ISR's, updates motion variable for motion detected.
void slide()
{
  slideState = digitalRead(slideSwitch);
}
void motion()
{
  motionState = digitalRead(motionSensor);
  if (motionState == true)
  {
    lastMotion = millis();
  }
}

// Applies a delay after motion has been detected to ensure that lights aren't flickering on and off constantly with movement.
bool motionDelay (unsigned long startTime)
  {
    if (startTime + duration <= millis())
    {
      return true;
    }
    else 
    {
      return false;
    }
  }

// Checks whether to activate motion sensed lights based on current ambient lighting.
bool sunlightCheck()
{
  if(lightMeter.readLightLevel() > sunlightLuxMin)
  {
    return true;
  }
  else
  {
    return false;
  }
}