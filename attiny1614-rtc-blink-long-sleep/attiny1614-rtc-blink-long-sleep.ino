/*

  Use PIT with counter for long sleep

  LED blinks every time it wakes up

*/

#include <avr/sleep.h>
#define led 7                           // positive lead of LED

volatile int current_sleeps = 0;        // counter for how many times PIT has woken up
int target_sleeps = 2;                  // target for total sleep time. Increment of the RTC_PERIOD setting below
                                        // e.g. RTC_PERIOD_CYC32768_gc = 32s, target_sleeps = 2 -> 64s sleep
// setup for the RTC
void RTC_init(void)
{
  // initialise RTC:
  while (RTC.STATUS > 0)
  {
    ;                                   // wait for all registers to be synchronised
  }
  RTC.CLKSEL = RTC_CLKSEL_INT1K_gc;     // 1.024kHz Internal Ultra-Low-Power Oscillator (from OSCULP32K)

  RTC.PITINTCTRL = RTC_PI_bm;           // PIT Interrupt: enabled

  RTC.PITCTRLA = RTC_PERIOD_CYC32768_gc // RTC Clock Cycles 32768, resulting in 32768/1024Hz = every 32 seconds
  | RTC_PITEN_bm;                       // enable PIT counter
}

// interrupt subroutine, runs when PIT wakes up
ISR(RTC_PIT_vect)
{
  current_sleeps += 1;                  // increment sleep counter
  RTC.PITINTFLAGS = RTC_PI_bm;          // clear interrupt flag by writing '1' (required)
}

// function to blink led
void blink(int times = 1, int interval = 100) {
  for (int n=0; n < times; n++) {
    digitalWrite(led, HIGH);
    delay(interval);
    digitalWrite(led, LOW);
    delay(interval);
  }
}

void setup() {
  pinMode(led, OUTPUT);                 // blinker
  digitalWrite(led, LOW);               // turn off
  
  RTC_init();                           // initialise the RTC timer
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // set sleep mode to POWER DOWN mode
  sleep_enable();                       // enable sleep mode, but not going to sleep yet
  sei();                                // enable interrupts

}

void loop() {
  if (current_sleeps == 0 || current_sleeps == target_sleeps ) { // just switched on, or we've hit our target
    // main code to run here
    blink(5);
    // end main code
    current_sleeps = 0;                           // reset sleep counter to 0
  }
  sleep_cpu();                                    // sleep the device and wait for an interrupt to continue
}
