#include <avr/sleep.h>

typedef struct{
  unsigned char second;
  unsigned char minute;
  unsigned char hour;
  unsigned char date;
  unsigned char month;
  unsigned int year;
} time;

int serialPin = PIN_PB1; // for interrupt into serial mode
volatile bool serialMode = false; // flag for detecting serial mode
bool serialEnabled = false;

volatile time t;

char timestamp[14]; // YYYYMMDDHHMMSS from serial input
char timeOnly[6]; // HHMMSS

void setup() {
  /* Switch off serial pins until we need them */
  pinMode(PIN_PA1, INPUT_PULLUP);
  pinMode(PIN_PA2, INPUT_PULLUP);
  
  /* Switch off unused pins */  
  pinMode(PIN_PA3, INPUT_PULLUP);
  pinMode(PIN_PA4, INPUT_PULLUP);
  pinMode(PIN_PA5, INPUT_PULLUP);
  pinMode(PIN_PA6, INPUT_PULLUP);
  pinMode(PIN_PA7, INPUT_PULLUP);
  pinMode(PIN_PB0, INPUT_PULLUP);
  
  uint8_t temp;

  /* Initialize 32.768kHz Oscillator: */
  /* Disable oscillator: */
  temp = CLKCTRL.XOSC32KCTRLA;
  temp &= ~CLKCTRL_ENABLE_bm;

  /* Writing to protected register */
  _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, temp);

  while(CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm);

  temp = CLKCTRL.XOSC32KCTRLA;
  temp &= ~CLKCTRL_SEL_bm;
  /* Writing to protected register */
  _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, temp);

  /* Enable oscillator: */
  temp = CLKCTRL.XOSC32KCTRLA;
  temp |= CLKCTRL_ENABLE_bm;
  /* Writing to protected register */
  _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, temp);

  /* Initialize RTC: */
  while (RTC.STATUS > 0);
  /* 32.768kHz External Crystal Oscillator (XOSC32K) */
  RTC.CLKSEL = RTC_CLKSEL_TOSC32K_gc;
//  RTC.CLKSEL = RTC_CLKSEL_INT32K_gc; // internal

  RTC.PITINTCTRL = RTC_PI_bm; /* Periodic Interrupt: enabled */

  RTC.PITCTRLA = RTC_PERIOD_CYC32768_gc /* RTC Clock Cycles 32768 */
               | RTC_PITEN_bm; /* Enable: enabled */

  pinMode(serialPin, INPUT_PULLUP);
  attachInterrupt(serialPin,serialISR,FALLING);
  sei(); // enable interrupts

  Serial.swap(1);  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
}

void loop() {
  if (serialMode) {
    if (!serialEnabled) {
      Serial.begin(9600);
      while (!Serial);
      serialEnabled = true;
      Serial.println("Entering serial mode");
    }
    
    if (Serial.available() > 0) {
      // read the incoming byte:
      char incomingByte = Serial.read();
    
      if (incomingByte == 115) { // s(et) timestamp mode
        Serial.println("Enter timestamp: YYYYMMDDHHMMSS");
    
        while (Serial.available() < 14); // wait for serial input
        for (int i=0; i<14; i++) {
          timestamp[i] = Serial.read();
        }
        t.year = 1000 * (timestamp[0] - '0') + 100 * (timestamp[1] - '0') + 10 * (timestamp[2] - '0') + (timestamp[3] - '0');
        t.month = 10 * (timestamp[4] - '0') + (timestamp[5] - '0');
        t.date = 10 * (timestamp[6] - '0') + (timestamp[7] - '0');
        t.hour = 10 * (timestamp[8] - '0') + (timestamp[9] - '0');
        t.minute = 10 * (timestamp[10] - '0') + (timestamp[11] - '0');
        t.second = 10 * (timestamp[12] - '0') + (timestamp[13] - '0');
        printTimestamp();
      } else if (incomingByte == 99) { // c(lock) set mode
        Serial.println("Enter clock time: HHMMSS");
        while (Serial.available() < 6); // wait for serial input
        for (int i=0; i<6; i++) {
          timeOnly[i] = Serial.read();
        }
        t.hour = 10 * (timeOnly[0] - '0') + (timeOnly[1] - '0');
        t.minute = 10 * (timeOnly[2] - '0') + (timeOnly[3] - '0');
        t.second = 10 * (timeOnly[4] - '0') + (timeOnly[5] - '0');
        printTimestamp();
      } else if (incomingByte == 114) { // r(ead) mode
        printTimestamp();
      } else if (incomingByte == 113) { // q(uit) back to sleep
        Serial.println("Going back to sleep");
        Serial.flush();
        Serial.end();
        serialEnabled = false;
        serialMode = false;
        // return serial pins to PULLUP for power saving
        pinMode(PIN_PA1, INPUT_PULLUP);
        pinMode(PIN_PA2, INPUT_PULLUP);
        attachInterrupt(serialPin,serialISR,FALLING);
        sleep_cpu();
      } else {
        Serial.println("Command not recognised.\n\ns(et timestamp), c(lock set), r(read) or q(uit)?");
      } // end checking for one character
    } // end if serial available
  } else { // end serialMode
    sleep_cpu();
  }
} // end loop

void printTimestamp() {
  Serial.print(t.date);
  Serial.print(" ");
  Serial.print(t.month);
  Serial.print(" ");
  Serial.print(t.year);
  Serial.print(" ");
  Serial.print(t.hour);
  Serial.print(":");
  Serial.print(t.minute);
  Serial.print(":");
  Serial.println(t.second);  
} // end printTimestamp

ISR(RTC_PIT_vect)
{  
  /* Clear flag by writing '1': */
  RTC.PITINTFLAGS = RTC_PI_bm;

  if (++t.second==60)        //keep track of time, date, month, and year
  {
    t.second=0;
    if (++t.minute==60)
    {
      t.minute=0;
      if (++t.hour==24)
      {
        t.hour=0;
        if (++t.date==32)
        {
          t.month++;
          t.date=1;
        }
        else if (t.date==31)
        {
          if ((t.month==4) || (t.month==6) || (t.month==9) || (t.month==11))
          {
            t.month++;
            t.date=1;
          }
        }
        else if (t.date==30)
        {
          if(t.month==2)
          {
            t.month++;
            t.date=1;
          }
        }
        else if (t.date==29)
        {
          if((t.month==2) && (not_leap()))
          {
            t.month++;
            t.date=1;
          }
        }
        if (t.month==13)
        {
          t.month=1;
          t.year++;
        }
      }
    }
  }

}

static char not_leap(void)      //check for leap year
{
  if (!(t.year%100))
  {
    return (char)(t.year%400);
  }
  else
  {
    return (char)(t.year%4);
  }
}

void serialISR() {
  detachInterrupt(serialPin);
  serialMode = true;
}
