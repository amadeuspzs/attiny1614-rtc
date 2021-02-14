// blink an led (PA5) every second

#include <avr/sleep.h>

int led = PIN_PA5;

void setup() {
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
//    RTC.CLKSEL = RTC_CLKSEL_INT32K_gc; // internal

  RTC.PITINTCTRL = RTC_PI_bm; /* Periodic Interrupt: enabled */

  RTC.PITCTRLA = RTC_PERIOD_CYC32768_gc /* RTC Clock Cycles 32768 */
               | RTC_PITEN_bm; /* Enable: enabled */

  pinMode(led, OUTPUT);

  sei(); // enable interrupts
  SLPCTRL.CTRLA |= SLPCTRL_SMODE_PDOWN_gc; // power down mode
  SLPCTRL.CTRLA |= SLPCTRL_SEN_bm;
}

void loop() {
  while(1) {
    sleep_cpu();
  }
}

ISR(RTC_PIT_vect)
{
    /* Clear flag by writing '1': */
    RTC.PITINTFLAGS = RTC_PI_bm;

    // toggle LED
    digitalWrite(led, !digitalRead(led));
}
