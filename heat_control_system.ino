#include <DHT.h>;

//constants
#define DHTPIN 7     // pin connected to DHT22
#define DHTTYPE DHT22   
#define HEATER_PIN 7
//LED state indicator pins
#define LED_IDLE 6
#define LED_HEATING 5
#define LED_STABILIZING 4
#define LED_TARGET_REACHED 3
#define LED_OVERHEAT 2
DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor

//LED states
const int ledPins[] = {
  LED_IDLE,
  LED_HEATING,
  LED_STABILIZING,
  LED_TARGET_REACHED,
  LED_OVERHEAT
};


// Heater States
enum HeaterStates {
  IDLE,
  HEATING,
  STABILIZING,
  TARGET_REACHED,
  OVERHEAT
};

HeaterStates currentState = STABILIZING;
//variables
float temp;  //temperature value

// temperature preset thresholds
const float MIN_TEMP = 20.0;
const float TARGET_TEMP = 24.0;
const float STABILIZE_RANGE = 0.5;
const float MAX_TEMP = 25.0;
void setup()
{
  cli();//stop interrupts
  Serial.begin(9600);
  dht.begin();
  for (int i = 0; i < sizeof(ledPins)/sizeof(ledPins[0]); i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
  pinMode(HEATER_PIN, OUTPUT);
  digitalWrite(HEATER_PIN, LOW); // Heater initially OFF
  TCCR1A = 0; // Init Timer1A
  TCCR1B = 0;  // Init Timer1B
  TCCR1B |= B00000101; // Prescaler set as 1024
  OCR1A = 31250;// Timer Compare1A Register set for 2 sec interval
  TIMSK1 |= B00000010; // Enable Timer COMPA Interrupt
  sei(); // Enable interrupts
}

void loop()
{

}

void controlHeater(float temp) {
  switch (currentState) {
    case IDLE:
      // digitalWrite(LED_HEATING, LOW);
      if (temp<MIN_TEMP ) {
        currentState = HEATING;
        digitalWrite(HEATER_PIN, HIGH);
        logState(temp, "HEATING");
      }
      break;

    case HEATING:
      if (temp>=TARGET_TEMP-STABILIZE_RANGE) {
        currentState = STABILIZING;
        logState(temp, "STABILIZING");
      }
      break;

    case STABILIZING:
      if (temp>=TARGET_TEMP && temp<=MAX_TEMP) {
        currentState = TARGET_REACHED;
        state_led(currentState);
        digitalWrite(HEATER_PIN, LOW); 
        logState(temp, "TARGET REACHED");
      }
      break;

    case TARGET_REACHED:
      if (temp>MAX_TEMP) {
        currentState = OVERHEAT;
        digitalWrite(HEATER_PIN, LOW);
        logState(temp, "OVERHEAT");
      } else if (temp<MIN_TEMP) {
        currentState = HEATING;
        digitalWrite(HEATER_PIN, HIGH);
        logState(temp, "HEATING");
      }
      break;

    case OVERHEAT:
      if (temp<=TARGET_TEMP) {
        currentState = TARGET_REACHED;
        logState(temp, "BACK TO TARGET");
      }
      break;
  }
}

void logState(float temp, const char* stateName) {
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print(" C,  State: ");
  Serial.println(stateName);
}

// Timer1 ISR
ISR(TIMER1_COMPA_vect) {
  // Serial.println("Interrupt triggered");
  float temp = dht.readTemperature();
  if (isnan(temp)) {
    Serial.println("Failed to read from DHT22!");
    return;
  }
  controlHeater(temp);
  OCR1A += 31250; // Incrementing COMPA Register
}

void state_led(HeaterStates currentState)
{
  for (int i = 0; i < sizeof(ledPins) / sizeof(ledPins[0]); i++) {
    digitalWrite(ledPins[i], LOW);
  }
  digitalWrite(ledPins[currentState], HIGH);
}