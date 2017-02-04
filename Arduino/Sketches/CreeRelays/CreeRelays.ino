//******************************************************************************************
// Pin Definitions
//******************************************************************************************

#define PIN_CREE_PWM   2 // this must be here for interrupt 0
#define PIN_CREE_ONOFF 3 // not in use by this code
#define RELAY_COUNT 8

#define debugging false

//******************************************************************************************
// Global Variables
//******************************************************************************************
volatile int pwm_raw = 0;
volatile int pmw_start = 0;

int relay_pin[RELAY_COUNT] = {4,5,6,7,8,9,10,11};
int relay_timer[RELAY_COUNT] = {0,0,0,0,0,0,0,0};

int pmw_end = 0;
int pwm_percent = 0;
int pwm_binary = 0;
int pwm_binary_previous = 0;
int pwm_raw_previous = -1;

//******************************************************************************************
void setup()
{
  if (debugging) {
    Serial.begin(9600);
  }
  
  for (int pin=0; pin < RELAY_COUNT; pin++) {
    if (debugging) {
      Serial.print("Initializing pin:");
      Serial.println(relay_pin[pin]);
    }
    pinMode(relay_pin[pin], OUTPUT);
    digitalWrite(relay_pin[pin], HIGH);
  }
  
  attachInterrupt(0, falling, FALLING);
  delay(2000); // to allow the readings to stablize
}

//******************************************************************************************
void loop()
{
  // Magic math to convert PWM raw value (840-0) to percent (0-100) with rounding
  pwm_percent = (844 - pwm_raw) / 8.4;
  
  // Magic math to convert PWM percent (0-100) to binary (0-31) with offset and rounding
  if (pwm_percent > 1) {
    pwm_binary = (pwm_percent - 2) / 3;
  }

  if (debugging) {
    Serial.print(pwm_raw); Serial.print("\t");
    Serial.print(pwm_percent); Serial.print("\t");
    Serial.println(pwm_binary);
    delay(250);
  }    
  
  // The Cree bulb does a slow transition between brightness levels,
  // so I wait until I see the same value twice in a row.
  if (pwm_raw == pwm_raw_previous) {
    if (pwm_binary != pwm_binary_previous) {
      if (pwm_binary < 32) {  // Keep the value 0-31, but allow 32 as a parking spot
        // Magic math to pull the relay and action out of the binary value
        int pin = pwm_binary & 7;            // first three bits are the relay 0-7
        int action = (pwm_binary & 24) >> 3; // last two bits are the action 0-3
        if (debugging) {
          Serial.print("Change of state, relay: ");
          Serial.print(pin);
          Serial.print("\taction: ");
          Serial.println(action);    
        }
        switch (action) {
          case 0:
            digitalWrite(relay_pin[pin], HIGH);
            break;
          case 1:
            digitalWrite(relay_pin[pin], LOW);
            break;
          case 2:
            digitalWrite(relay_pin[pin], !digitalRead(relay_pin[pin]));
            break;
          case 3:
            digitalWrite(relay_pin[pin], LOW);
            relay_timer[pin] = 20;
            break;
        }
      }
      pwm_binary_previous = pwm_binary;
    }
  }

  // Check any timers that have been set and turn off any relays that finish their time
  for (int pin=0; pin < RELAY_COUNT; pin++) {
    if (relay_timer[pin] > 0) {
      if (--relay_timer[pin] == 0) {
        digitalWrite(relay_pin[pin], HIGH);       
      }
    }
  }
  
  pwm_raw_previous = pwm_raw;
  delay(50);  // I found this necessary to allow the PWM value time to transition.
}

//******************************************************************************************
void rising()
{
  attachInterrupt(0, falling, FALLING);
  pmw_start = micros();
}

//******************************************************************************************
void falling()
{
  attachInterrupt(0, rising, RISING);
  pmw_end = micros();
  if (pmw_end > pmw_start) { // ...to prevent wild numbers when the integer rolls over.
    pwm_raw = pmw_end - pmw_start;  
  }
}
