# RTC on the ATtiny1614

Use the built-in RTC/PIT functionality for clock/alarm functionality. Avoid external RTC chips/modules.

## Background reading

* [ATtiny1614 datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny1614-16-17-DataSheet-DS40002204A.pdf)
* [Technical Brief (TB) 3213](https://www.microchip.com/wwwappnotes/appnotes.aspx?appnote=en609105)
* [megaTinyCore](https://github.com/SpenceKonde/megaTinyCore)

## Arduino setup

ATtiny1614 running with 20 MHz internal RC oscillator and [12.5pF 32.768Khz watch crystal](http://www.farnell.com/datasheets/1883667.pdf), programmed using [megaTinyCore](https://github.com/SpenceKonde/megaTinyCore):

![Schematic](https://user-images.githubusercontent.com/534681/107887522-a0980d00-6efe-11eb-9ff9-68b01d6c64d1.png)

Flashed using UPDI.

## Alarm circuit (post flash)
![Schematic](https://user-images.githubusercontent.com/534681/108268478-7b0c3d00-7164-11eb-9a1d-9236b284c03f.png)

* Baseline power consumption: `1.3uA`
* Serial mode entered via DTR signal on serial monitor connect, power consumption: `1.6mA`
* LED consumption at V<sub>cc</sub> 4.5V, forward voltage 2.9V: `20mA`
* Date, time and alarm programmed over serial (9600 baud)
