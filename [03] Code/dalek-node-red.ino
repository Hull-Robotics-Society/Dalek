// Dalek serial: 49(F), 50(L), 51(R), 52(FL), 53(FR), 54(Lights), 55(NRon), 56(NRoff)

#include <Stepper.h>

const int stepsPerRevolution = 200;

Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

const int joystickXPin = A0;
const int joystickYPin = A1;
const int joystickTwistPin = A2;
const int joystickButtonPin = A3;

// Variables for light toggle
bool lightState = false; // Current state of lights
int lastButtonState = HIGH; // Last state of button (pulled up)
unsigned long lastButtonPress = 0; // Last time button was pressed
const unsigned long debounceDelay = 50; // Debounce delay in milliseconds

int joystickXValue = 0;
int joystickYValue = 0;
int joystickTwistValue = 0;

const int relayUpPin = 2;
const int relayLeftPin = 3;
const int relayRightPin = 4;
const int relayLightsPin = 5;

const int threshold = 100;

int nrControl;

String incomingMessage = "";

int incomingByte = 0;

void setup() {
  Serial.begin(115200);
  pinMode(relayUpPin, OUTPUT);
  pinMode(relayLeftPin, OUTPUT);
  pinMode(relayRightPin, OUTPUT);
  pinMode(relayLightsPin, OUTPUT);

  // Start all relays off (HIGH is off for active low relays)
  digitalWrite(relayUpPin, HIGH);
  digitalWrite(relayLeftPin, HIGH);
  digitalWrite(relayRightPin, HIGH);
  digitalWrite(relayLightsPin, HIGH); // Start with lights off

  pinMode(joystickButtonPin, INPUT_PULLUP);
  nrControl = 8;
  
  myStepper.setSpeed(50);
  }

void loop() {
  joystickXValue = analogRead(joystickXPin);
  joystickYValue = analogRead(joystickYPin);
  joystickTwistValue = analogRead(joystickTwistPin);

  if (Serial.available() > 0) {
        // read the incoming byte:
        incomingByte = Serial.read();
  }
  
  Serial.print("Joystick: X = ");
  Serial.println(joystickXValue);
  Serial.print("Joystick: Y = ");
  Serial.println(joystickYValue);
  Serial.print("Joystick: Twist = ");
  Serial.println(joystickTwistValue);
  Serial.print("incomingByte = ");
  Serial.println(incomingByte);
  
  int buttonState = digitalRead(joystickButtonPin); // Read button state (HIGH if not pressed)

  // Check for button press
  if ((buttonState == LOW && lastButtonState == HIGH && (millis() - lastButtonPress > debounceDelay)) || (incomingByte == 54)) {
    // Button was pressed, toggle light state
    lightState = !lightState; // Toggle the state
    digitalWrite(relayLightsPin, lightState ? LOW : HIGH); // Set light state
    lastButtonPress = millis(); // Update last button press time
  }

  // Update last button state
  lastButtonState = buttonState;

  // Process Forward/Backward (Y-axis)
  if ((joystickYValue < 512 - threshold) || (incomingByte == 49)) { // Forward
    digitalWrite(relayUpPin, LOW);
  } else {
    digitalWrite(relayUpPin, HIGH);
  }

  // Process Left/Right (X-axis)
  if ((joystickXValue < 512 - threshold) || (incomingByte == 50)) { // Left
    digitalWrite(relayLeftPin, LOW);
    digitalWrite(relayRightPin, HIGH); // Ensure right relay is off
  } else if ((joystickXValue > 512 + threshold) || (incomingByte == 51)) { // Right
    digitalWrite(relayRightPin, LOW);
    digitalWrite(relayLeftPin, HIGH); // Ensure left relay is off
  } else {
    digitalWrite(relayLeftPin, HIGH);
    digitalWrite(relayRightPin, HIGH);
  }

  if (incomingByte == 52) { // Foward - left
    digitalWrite(relayUpPin, LOW);
    digitalWrite(relayLeftPin, LOW);
    digitalWrite(relayRightPin, HIGH); // Ensure right relay is off
  }
  else if (incomingByte == 53) { // Foward - right
    digitalWrite(relayUpPin, LOW);
    digitalWrite(relayLeftPin, HIGH); // Ensure left relay is off
    digitalWrite(relayRightPin, LOW);
  }


   if (incomingByte == 0 && nrControl == 1) {
     digitalWrite(relayUpPin, HIGH);
     digitalWrite(relayLeftPin, HIGH);
     digitalWrite(relayRightPin, HIGH);
     digitalWrite(relayLightsPin, HIGH);
   }

  if (incomingByte == 7) {
     nrControl = 1;
   }
  if (incomingByte == 8) {
    nrControl = 0;
  }

  // Short delay to stabilize readings
  delay(50);
}
