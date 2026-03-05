/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://docs.arduino.cc/hardware/

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman
  modified 5 mar 2026
  by Ian Sharam

  This example code is in the public domain.

  https://docs.arduino.cc/built-in-examples/basics/Blink/
*/

// initialises the variables for LED's and button
const int porchLight = 7;
const int hallwayLight = 8;
const int pushButton = 12;

int buttonState = 0; 

// initialises the variable for tracking time of button press. 
unsigned long startPress = 0;

void setup() 
 {
  pinMode(porchLight, OUTPUT);
  pinMode(hallwayLight, OUTPUT);
  pinMode(pushButton, INPUT);
 }


void lightsOn(int light) // takes a light variable and turns on the corresponding LED
  {
    digitalWrite(light, HIGH);
  }

  // takes a specific LED and checks against the start time to ensure the duration has elapsed
  void lightsOff(int light, unsigned long startTime, int duration)
  {
    if (startTime + duration <= millis())
    {
      digitalWrite(light, LOW);
    }
  }

void loop() 
 {
  // retrieves if the button has been pushed
  buttonState = digitalRead(pushButton);

  // turns on the LEDs 
  if (buttonState == HIGH)
  {
    startPress = millis();
    lightsOn(porchLight);
    lightsOn(hallwayLight);
  }

  // loop will continue, turning lights off only if the specificed duration for each light has been met since the user let go of the push button
  lightsOff(porchLight, startPress, 30000);
  lightsOff(hallwayLight, startPress, 60000);
 }