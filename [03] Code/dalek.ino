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

void setup() {
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
  
  myStepper.setSpeed(50);
  
  Serial.begin(9600);
}

void loop() {
  joystickXValue = analogRead(joystickXPin);
  joystickYValue = analogRead(joystickYPin);
  joystickTwistValue = analogRead(joystickTwistPin);
  
  Serial.print("Joystick: X = ");
  Serial.println(joystickXValue);
  Serial.print("Joystick: Y = ");
  Serial.println(joystickYValue);
  Serial.print("Joystick: Twist = ");
  Serial.println(joystickTwistValue);
  
  int buttonState = digitalRead(joystickButtonPin); // Read button state (HIGH if not pressed)

  // Check for button press
  if (buttonState == LOW && lastButtonState == HIGH && (millis() - lastButtonPress > debounceDelay)) {
    // Button was pressed, toggle light state
    lightState = !lightState; // Toggle the state
    digitalWrite(relayLightsPin, lightState ? LOW : HIGH); // Set light state
    lastButtonPress = millis(); // Update last button press time
  }

  // Update last button state
  lastButtonState = buttonState;

  // Process Forward/Backward (Y-axis)
  if (joystickYValue < 512 - threshold) { // Forward
    digitalWrite(relayUpPin, LOW);
  } else {
    digitalWrite(relayUpPin, HIGH);
  }

  // Process Left/Right (X-axis)
  if (joystickXValue < 512 - threshold) { // Left
    digitalWrite(relayLeftPin, LOW);
    digitalWrite(relayRightPin, HIGH); // Ensure right relay is off
  } else if (joystickXValue > 512 + threshold) { // Right
    digitalWrite(relayRightPin, LOW);
    digitalWrite(relayLeftPin, HIGH); // Ensure left relay is off
  } else {
    digitalWrite(relayLeftPin, HIGH);
    digitalWrite(relayRightPin, HIGH);
  }

  // Short delay to stabilize readings
  delay(50);
}
