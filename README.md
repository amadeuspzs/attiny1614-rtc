# RTC on the ATtiny1614

Use the built-in RTC functionality for clock/alarm functionality. Avoid external RTC chips/modules.

## Background reading

* [ATtiny1614 datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny1614-16-17-DataSheet-DS40002204A.pdf)
* [TB 3213](https://www.microchip.com/wwwappnotes/appnotes.aspx?appnote=en609105)
* [megaTinyCore](https://github.com/SpenceKonde/megaTinyCore)

## Arduino setup

ATtiny1614 running with 20 MHz internal RC oscillator and 32.768 kHz Ultra Low-Power (ULP) internal RC oscillator<!-- [6pF 32.768Khz watch crystal](http://www.farnell.com/datasheets/1564124.pdf)-->, programmed using [megaTinyCore](https://github.com/SpenceKonde/megaTinyCore):

![Schematic](https://user-images.githubusercontent.com/534681/107878934-c5c05780-6ecd-11eb-9c4a-41e587966a21.png)

Flashed using UPDI.
