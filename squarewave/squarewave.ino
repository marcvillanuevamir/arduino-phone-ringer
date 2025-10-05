// MUDANZAS PORTAS (Ringing Code)
// This code uses a L298N to create a 20Hz square wave and relays it to three cord telephones
#include <debounce.h>

//const int ENA = 3;  // PWM capable pin (not in use)
// ENA tied high with a jumper
// L298N ins (squarewave generation)
const int IN1 = 2;
const int IN2 = 7;

// relay pins
const int relayChannels = 3;
const int relay1 = 4;
const int relay2 = 5;
const int relay3 = 6;

// input pins
//const int knobPin = A0;
const int buttonPin = A1;

// states and values
int relay1State = HIGH;
int relay2State = HIGH;
int relay3State = HIGH;
//int knobValue;
int buttonState;
int lastButtonState;

// ringing flags
bool ring1 = false;
bool ring2 = false;
bool ring3 = false;

// active ringing flags
bool ring1Active = false;
bool ring2Active = false;
bool ring3Active = false;

bool armedScene1 = true;
bool armedScene2 = true;
bool armedScene3 = true;

// timing
unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
unsigned long previousMillis3 = 0;
//timing for delaying scene rings
unsigned long scene2Start = 0;
unsigned long scene3Start = 0;
bool scene2Started = false;
bool scene3Started = false;

// intervals
unsigned long interval = 1200;  // ring wait

const unsigned long halfPeriodMs = 25;  // frequency -> 20 Hz

int scene = 0;
int lastScene = -1;
int ring1Counter = 0;
int ring2Counter = 0;
int ring3Counter = 0;

// scenes
void scene1(bool &armed) {
  if (armed) {
    if (ring3Counter < 9) {  // ring 5 times
      ring3 = true;
    } else {
      stopall();
      armed = false;
    }
  }
}

void scene2(bool &armed, unsigned long millis) {
  if (armed) {
    if (!scene2Started) {
      scene2Start = millis;
      scene2Started = true;
    }
    ring2 = true;
    if (!ring3 && millis - scene2Start >= 1300) {
      ring3 = true;
    }
    if (!ring1 && millis - scene2Start >= 3500) {
      ring1 = true;
    }
    if (ring3Counter > 15) {
      stopall();
      armed = false;
      scene2Started = false;
    }
  }
}

void scene3(bool &armed, unsigned long millis) {
  if (armed) {
    if (!scene3Started) {
      scene3Start = millis;
      scene3Started = true;
    }

    unsigned long elapsed = millis - scene3Start;

    // Delayed activation
    ring1 = true;
    if (!ring2 && elapsed >= 2200) {
      ring2 = true;
    }
    if (!ring3 && elapsed >= 3100) {
      ring3 = true;
    }

    if (elapsed < 60000) {
      interval = random(60, 1200);
    } else if (elapsed < 62000) {
      float t = (float)(elapsed - 60000) / 2000.0;  // 0 -> 1
      interval = 800 + t * (100 - 800);             // 800 -> 100
    } else {
      interval = 100;
    }

    // Time-Stop Scene
    if (elapsed >= 65000) {
      resetScenes();
      armed = false;
      scene3Started = false;
    }
  }
}

void stopall() {
  ring1 = false;
  ring2 = false;
  ring3 = false;
  ring3Counter = 0;
}

void resetScenes() {
  stopall();

  // Reset flags
  armedScene1 = true;
  armedScene2 = true;
  armedScene3 = true;

  // Reset counters
  ring1Counter = 0;
  ring2Counter = 0;
  ring3Counter = 0;

  // Reset timers
  previousMillis1 = 0;
  previousMillis2 = 0;
  previousMillis3 = 0;
  scene2Start = 0;
  scene2Started = false;
  scene3Start = 0;
  scene3Started = false;

  // Reset interval
  interval = 1200;
}

// BUTTON DEBOUNCING
bool useShort;
// Short press button
static void shortPressHandler(uint8_t btnId, uint8_t btnState) {
  if (btnState == BTN_PRESSED) {
    useShort = true;
  } else {
    if (useShort) {
      scene++;
      return;
    }
  }
}
// Long press button
static void longPressHandler(uint8_t btnId, uint8_t btnState) {
  if (btnState == BTN_PRESSED) {
    useShort = false;
  } else {
    if (!useShort) {
      scene = 0;  // After it has been held down a long time and released
      return;
    }
  }
}
// Button definitions
static Button shortBtn(0, shortPressHandler);
static Button longBtn(1, longPressHandler);

static void pollButtons() {
  uint8_t pinState = digitalRead(buttonPin);
  shortBtn.update(pinState);
  longBtn.update(pinState);
}

void setup() {
  //inMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  //pinMode(knobPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  for (int i = 0; i < relayChannels; i++) {
    pinMode(relay1 + i, OUTPUT);
    digitalWrite(relay1 + i, HIGH);
  }
  longBtn.setPushDebounceInterval(1000);
  //digitalWrite(ENA, HIGH);  // or analogWrite(ENA, 200) to limit amplitude
  Serial.begin(9600);
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
  // read input
  pollButtons();
  //knobValue = map(analogRead(knobPin), 0, 1023, 1023, 0);  // invert range of analog readings
  //interval = knobValue * 2;

  if (ring1) {
    // Start/continue the cycle
    ring1Active = true;
    if (currentMillis - previousMillis1 >= interval) {
      previousMillis1 = currentMillis;

      // Toggle state
      relay1State = (relay1State == LOW) ? HIGH : LOW;
      digitalWrite(relay1, relay1State);
      ring1Counter++;
    }
  } else if (ring1Active) {
    // Finish the cycle gracefully
    if (currentMillis - previousMillis1 >= interval) {
      previousMillis1 = currentMillis;

      // Force relay HIGH and stop further cycles
      relay1State = HIGH;
      digitalWrite(relay1, relay1State);
      ring1Active = false;  // finished, no more toggles
    }
  } else {
    // Already inactive
    relay1State = HIGH;
    digitalWrite(relay1, relay1State);
  }

  if (ring2) {
    ring2Active = true;
    if (currentMillis - previousMillis2 >= interval) {
      previousMillis2 = currentMillis;

      relay2State = (relay2State == LOW) ? HIGH : LOW;
      digitalWrite(relay2, relay2State);
      ring2Counter++;
    }
  } else if (ring2Active) {
    if (currentMillis - previousMillis2 >= interval) {
      previousMillis2 = currentMillis;

      relay2State = HIGH;
      digitalWrite(relay2, relay2State);
      ring2Active = false;
    }
  } else {
    relay2State = HIGH;
    digitalWrite(relay2, relay2State);
  }

  if (ring3) {
    ring3Active = true;
    if (currentMillis - previousMillis3 >= interval) {
      previousMillis3 = currentMillis;

      relay3State = (relay3State == LOW) ? HIGH : LOW;
      digitalWrite(relay3, relay3State);
      ring3Counter++;
    }
  } else if (ring3Active) {
    if (currentMillis - previousMillis3 >= interval) {
      previousMillis3 = currentMillis;

      relay3State = HIGH;
      digitalWrite(relay3, relay3State);
      ring3Active = false;
    }
  } else {
    relay3State = HIGH;
    digitalWrite(relay3, relay3State);
  }

  if (scene != lastScene) {
    resetScenes();  // only reset when scene changes
    lastScene = scene;
  }

  // scene control
  switch (scene) {
    case 1: scene1(armedScene1); break;
    case 2: scene2(armedScene2, currentMillis); break;
    case 3: scene3(armedScene3, currentMillis); break;
    case 0: resetScenes(); break;
    default: break;
  }

  // SERIAL PRINTS
  if (Serial.available()) {
    Serial.print("SCENE: ");
    Serial.print(scene);
    Serial.print("  PHONES: ");
    Serial.print(ring1);
    Serial.print("  ");
    Serial.print(ring2);
    Serial.print("  ");
    Serial.print(ring3);
    Serial.print("  R3 COUNTER: ");
    Serial.print(ring3Counter);
    Serial.print("  INTERVALS: ");
    Serial.println(interval);
  }
}
