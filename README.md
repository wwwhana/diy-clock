# Diy Clock

Arduino + Orange pi base 7Segment Clock.

## hardware
 * Orange Pi Zero 2w (os: ubuntu 22.04.4)
 * Arduino Pro Micro * 2
  * * Arduino A (for Segment Control. TX/RX <-> Arduino B RX/TX, [7digit.ino](arduino/7digit.ino))
  * * Arduino B (for Sensor and Segment Control. GPIO 8/9 <-> Orange Pi UART 2, TX/RX <-> Arduino A RX/TX, [sensor.ino](arduino/sensor.ino))
 * 8-digit segment display

 * DS3231 -> orange Pi 2w
 * CDS cell(조도 센서) (Arduino B GPIO 5 pin) 
 * ATH10 (Arduino B GPIO 2(SDA), 3(SDL) pin)

### 7segment control command

The command must end with a newline character (\\n).

| command | argument detail |  | example |
|---------|-----------------|--| ------- |
| SETMODE\|{mode} | `mode`: CLOCK(defualt), DETAILCLOCK, BLINK, STR | |SETMODE\|CLOKCK|
| SETTIME\|{time} | `time`: yyyy.MM.dd HH:mm:ss |  |SETTIME\|2024.01.01 16:30:00|
| SETSTR\|{msg}\|{dp}\|{duration} | `dp`: Set whether to display the DP position of the segment. Set by substituting each digit into 8 bits (0-255)<sup>[1](#footnote_1)</sup>.<br />`duration`: second by second. If it is less than 0, it is displayed as unlimited.  | First need to set STR mode | SETSTR\|SHOWCLOC\|16\|-1 <sup>[2](#footnote_2)</sup>
| SETBR\|{brightness} | `brightnes`s: -200 to 200 | set segment brightness | SETBR\|70 |
| SETHUMIDITY\|{humidity} | `humidity`: float || SETHUMIDITY\|61.4|
| SETTEMPERATURE\|{temperature} | `temperature`: float| | SETTEMPERATURE\|25.3 |


<a name="footnote_1">[1]</a>examples, 128(10000000) : `.8888 8888`, 34(00100010): `88:88 88.88` <br />
<a name="footnote_2">[2]</a> Display as  `SHOW.CLOC`

#### Display Mode
| mode | |
|------|-|
|CLOCK| default. Display as `%H:%M %m.%d` |
|DETAILCLOCK| There are three screens that change every 5 seconds. <br /> Display Clock: `%I:%M:%S %p`  <br /> Display Date: `%Y.%m.%d`<br />Display Humidity & Temperature: `H00.0 00.0°C`.<br />Both temperature and humidity are displayed as single-digit decimals. If the temperature is below 0 degrees, it is displayed as `.00.0°C`.|
|STR|Displays an 8-character string. Characters that cannot be displayed in 7 segments are not displayed. If no string is set, `NREADY__` is displayed.|
|BLINK| Turn off the screen. |

## API
 * /api/sensors/ : Display sensor information in json format
 * /api/clock/sync : Forced time synchronization on Segment
 * /api/clock/msg?msg={msg}&dot={dot}&duration={duration} : Displays 8 characters on segment
  * /api/clock/blink: turn off Display


### clock.py

Synchronizes the clock, adjusts brightness, and reads sensors. Must register as a service