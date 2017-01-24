//******************************************************************************************
// Pin Definitions
//******************************************************************************************
#define PIN_RELAY_1    4
#define PIN_RELAY_2    5

#define PIN_CREE_PWM   2 // this must be here for interrupt 0
#define PIN_CREE_ONOFF 3

#define debugging false

//******************************************************************************************
// Global Variables
//******************************************************************************************
volatile int pwm_value = 0;
volatile int prev_time = 0;

int val = 0;
int level = 0;
int prev_value = -1;

//******************************************************************************************
void setup()
{
  Serial.begin(9600);
  
  pinMode(PIN_CREE_ONOFF, INPUT); // not really in use right now.
  
  pinMode(PIN_RELAY_1, OUTPUT);
  pinMode(PIN_RELAY_2, OUTPUT);
  
  digitalWrite(PIN_RELAY_1, HIGH);
  digitalWrite(PIN_RELAY_2, HIGH);

  attachInterrupt(0, falling, FALLING);
}

//******************************************************************************************
void loop()
{
  // Magic math to convert PWM value (840-0) to relay number (0-10)
  level = (882 - pwm_value) / 84;
  
  if (debugging)
  {
    Serial.println(level);
    delay(250);
  }

  // The Cree bulb does a slow transition between brightness levels.
  // I wait until the value settles down.
  if (pwm_value == prev_value) 
  {
    digitalWrite(PIN_RELAY_1, level == 1?LOW:HIGH);
    digitalWrite(PIN_RELAY_2, level == 2?LOW:HIGH);
  } 

  prev_value = pwm_value;
  delay(25);  // I found this necessary to allow the PWM value time to transition.
}

//******************************************************************************************
void rising()
{
  attachInterrupt(0, falling, FALLING);
  prev_time = micros();
}

//******************************************************************************************
void falling()
{
  attachInterrupt(0, rising, RISING);
  val = micros();
  if(val > prev_time) // ...to prevent wild numbers when the integer rolls over.
  {
    pwm_value = val - prev_time;  
  }
}
