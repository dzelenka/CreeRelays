//******************************************************************************************
// Pin Definitions
//******************************************************************************************
#define PIN_RELAY_1    2
#define PIN_RELAY_2    3

#define PIN_CREE_PWM   11 //NOTE:  This PIN is hard-coded in the ISR routine below.  Defined here for informational purposes only.
#define PIN_CREE_ONOFF 12

//******************************************************************************************
// Global Variables
//******************************************************************************************
volatile unsigned long count = 0;
volatile unsigned long numInterrupts = 0;
byte lastCreeOnOffValue = LOW;

//******************************************************************************************
// Arduino setup()
//******************************************************************************************
void setup()
{
  Serial.begin(9600);
  
  pinMode(PIN_CREE_ONOFF, INPUT);
  
  pinMode(PIN_RELAY_1, OUTPUT);
  pinMode(PIN_RELAY_2, OUTPUT);
  
  digitalWrite(PIN_RELAY_1, HIGH);
  digitalWrite(PIN_RELAY_2, HIGH);
  
  setupTimingInterrupt();
}

//******************************************************************************************
// Arduino loop()
//******************************************************************************************
void loop()
{  
  static int lastLevel = -1;
  
  //Get the value of the Cree Zigbee board's PWM digital ouput
  int level = getDimLevel();
  if (level != lastLevel)
  {
    lastLevel = level;
    
    Serial.print(F("Cree Level = "));
    Serial.println(level);
    
     //TODO: Add any code needed to handle the value of the PWM input 0 through 99
    
    digitalWrite(PIN_RELAY_1, level == 1?LOW:HIGH);
    digitalWrite(PIN_RELAY_2, level == 2?LOW:HIGH);

    delay(250);

    digitalWrite(PIN_RELAY_1, HIGH);
    digitalWrite(PIN_RELAY_2, HIGH);
  }
  
  //Get the value of the Cree Zigbee board's On/Off digital ouput
  byte CreeOnOffValue = digitalRead(PIN_CREE_ONOFF);
  if(CreeOnOffValue != lastCreeOnOffValue)
  {
    Serial.print(F("Cree On/Off Value = "));
    Serial.println(CreeOnOffValue);
    lastCreeOnOffValue = CreeOnOffValue;
    
    //TODO: Add any code needed to handle the ON or OFF event
    
  }
}

//******************************************************************************************
// getDimLevel() - Routine to get the Cree Zigbee board's dim level and 
//                 scale it from 0 to 99
//******************************************************************************************
int getDimLevel()
{
  static int prevdimlevel = -1;
  static byte numdebounce = 0;
  unsigned long tmpCount; 
  
  if (numInterrupts >= 79208)  // If 0.5 second has elapsed, time to calculate the new dim level
    {
      noInterrupts();
      tmpCount = count;
      count=0;
      numInterrupts=0;
      interrupts();
    
      int dimlevel = map(tmpCount, 14500, 78150, 1, 99); //map counts to dim levels 1 though 99
      if (dimlevel < 0) dimlevel = 0;
      if (dimlevel > 99) dimlevel = 99;
      if (dimlevel != prevdimlevel)
      {
        numdebounce++;
        if (numdebounce > 2)  //need to have the value stable for a few interations to prevent transitional pwm values
        {
          int returnValue= prevdimlevel;
          prevdimlevel = dimlevel;
          //Serial.println(returnValue);
          return returnValue;
        }
        else
        {
          //Serial.println(prevdimlevel);
          return prevdimlevel;
        }
      }
      else
      {
        numdebounce = 0;
        //Serial.println(dimlevel);
        return dimlevel;
      }
    }
    else
    {
      return prevdimlevel; //Not time to calculate
    }
}

//******************************************************************************************
// Interrupt Service Routine
//******************************************************************************************
ISR(TIMER2_COMPA_vect)
{
    numInterrupts++;  //keep count of the number of times we've interrupted, so we can figure out when 0.5 second has elapsed
    if (!(PINB & (1<<PB3))) count++;  //Read value of Pin 11, if low, increment counter (PIN_CREE_PWM) - direct read of I/O register is much faster than a digitalRead() command
}

//******************************************************************************************
// Configuration for the TIMER2 counter - comparison mode
//******************************************************************************************
void setupTimingInterrupt()
{
  //set timer2 interrupt at 158.416kHz
  noInterrupts();  // disable global interrupts
  TCCR2A = 0;      // set entire TCCR2A register to 0
  TCCR2B = 0;      // same for TCCR2B
  TCNT2  = 0;      //initialize counter value to 0
  
  OCR2A = 100;// = (16*10^6) / (158416*1) - 1 (must be <256) // set compare match register for 158.416kHz increments
  TCCR2A |= (1 << WGM21);  // turn on CTC mode
  TCCR2B |= (1 << CS10);  // Set CS10 bit for no prescaler   
  TIMSK2 |= (1 << OCIE2A);// enable timer compare interrupt
  interrupts();
}
