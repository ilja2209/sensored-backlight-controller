#include <Ultrasonic.h>

#define SENSOR_POWER_PIN 9
#define SENSOR_SIG 7
#define LED_PWM_PIN 5
#define PWM_INTERVAL 5
#define SWITCHING_DISTANCE 75
#define TIME_BEFORE_SWITCHING_OFF 300000 //5 min
#define MAIN_LOOP_DELAY 150
#define NUMBERS_OF_SWITCH_OFF_DELAYS TIME_BEFORE_SWITCHING_OFF/MAIN_LOOP_DELAY
#define MAX_BRIGHT 255
#define MIN_BRIGHT 0

Ultrasonic ultrasonic(SENSOR_SIG);
volatile bool isSwitchedOn = false;
volatile int filterOneSwitchOnCounter = 0;
volatile int filterOneSwitchOffCounter = 0;
volatile long switchOffDelaysCounter = 0;
 
void setup() {
  pinMode(SENSOR_POWER_PIN, OUTPUT);
  digitalWrite(SENSOR_POWER_PIN, HIGH);
  analogWrite(LED_PWM_PIN, 0);
  isSwitchedOn = false;
  Serial.begin(9600);
}

void switchOn() {
  for (int i = MIN_BRIGHT; i <= MAX_BRIGHT; i++) {
    delay(PWM_INTERVAL);
    analogWrite(LED_PWM_PIN, i);
  }
}

void switchOff() {
  for (int i = MAX_BRIGHT; i >= MIN_BRIGHT; i--) {
    delay(PWM_INTERVAL);
    analogWrite(LED_PWM_PIN, i);
  }
}

long measureDistance() {
    long distance = ultrasonic.MeasureInCentimeters();
    Serial.print(distance);
    Serial.println(" cm");
    return distance;
}

bool filterOne(int *counter, bool flag, int numberOfCycles) {
  if (flag) {
    if (++*counter < numberOfCycles) {
      return false;
    } else {
      return true;
    }
  } else {
    *counter = 0;
    return false;
  }
}

void loop() {
  
  if (filterOne(&filterOneSwitchOnCounter, measureDistance() < SWITCHING_DISTANCE, 5)) {
    if (!isSwitchedOn) {
      Serial.println("Switch on");
      switchOn();
      isSwitchedOn = true;
    }
    switchOffDelaysCounter = 0;
  } 
  
  if (filterOne(&filterOneSwitchOffCounter, measureDistance() >= SWITCHING_DISTANCE, 5)) {
    if (isSwitchedOn) {
      if (++switchOffDelaysCounter == NUMBERS_OF_SWITCH_OFF_DELAYS) {
        Serial.print("Switch off");
        switchOff();
        isSwitchedOn = false;
        switchOffDelaysCounter = 0;
      }
    }
  }  
  delay(MAIN_LOOP_DELAY);
}
