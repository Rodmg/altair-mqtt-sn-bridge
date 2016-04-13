#include <Wire.h>
#include <Mesh.h>
#include <AquilaProtocol.h>

/*
  Example_ButtonEvent
  In this example we will use Altair's internal
  button for triggering an event that can be connected
  to other devices via interactions.
*/

// Altair's internal button
#define BUTTON 33

// Event that will be emited when the button is pressed
Event buttonPressed;

// Simple button status check routine
bool buttonLastState = HIGH;
bool buttonIsPressed()
{
  bool actualState = digitalRead(BUTTON);

  if(actualState == LOW && buttonLastState == HIGH)
  {
    buttonLastState = LOW;
    return true;
  }
  else if(actualState == HIGH)
  {
    buttonLastState = HIGH;
  }

  return false;
}

void setup()
{
  // Button setup
  pinMode(BUTTON, INPUT);
  // Aquila setup
  Mesh.begin();
  Aquila.begin();
  Aquila.setClass("mx.makerlab.button");
  Aquila.setName("Button");
  // Registering event
  buttonPressed = Aquila.addEvent("Button Pressed");
  // Anouncing device
  Mesh.announce(HUB);
}

void loop()
{
  // Attending requests
  Mesh.loop();
  Aquila.loop();
  // Checking button
  if(buttonIsPressed())
  {
    Aquila.emit(buttonPressed);
  }
}
