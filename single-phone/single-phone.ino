// MUDANZAS PORTAS (Ringing Code)
// This code uses a L298N to create a 20Hz square wave and impose a European ringing pattern on a cord telephone

// L298N ins (squarewave generation)
// ENA is tied high with a jumper
const int IN1 = 2;
const int IN2 = 3;

// relay
const int relayPin = 4;
bool relayState = HIGH;

// timing
bool ringing = false;
unsigned long previousMillis = 0;
unsigned long interval = 1200;
const unsigned long halfPeriodMs = 25;  // frequency -> 20 Hz

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode (relayPin, OUTPUT);
}

void square() {
  // Forward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  delay(halfPeriodMs);

  // Reverse
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  delay(halfPeriodMs);
}

void loop() {
  unsigned long currentMillis = millis();

  square();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    // Toggle state
    relayState = (relayState == LOW) ? HIGH : LOW;
    digitalWrite(relayPin, relayState);
  }
}
