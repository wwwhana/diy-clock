import serial
import datetime
import sched, time
import atexit

#시리얼포트 객체 ser을 생성
#pc와 스위치 시리얼포트 접속정보
ser = serial.Serial(
    port = '/dev/ttyACM0',
    baudrate=19200,
    parity='N',
    stopbits=1,
    bytesize=8,
    timeout=8
    )

i = 0

showAMPM = False
showSec = False

#시리얼포트 접속
ser.isOpen()

ser.write(str.encode(f"88888888|255\n"))

s = sched.scheduler(time.time, time.sleep)

tick = 0.1


def showTime(sc):
  if showAMPM == True:
      showTimeAMPN()
  else:
      showTimeDefault()

  s.enter(tick, 1, showTime, (sc,))

def showTimeDefault():
  now = datetime.datetime.now()
  dot = 0
  output = ''

  if showSec == True:
    output = f'{now.strftime("%H%M")} {now.strftime("%S")}S'
    if now.second % 2 == 0:
      dot = 33
    else:
      dot = 1
  else:
    output = f'{now.strftime("%H%M%m%d")}'
    if now.second % 2 == 1:
      dot = 2
    else:
      dot = 34

  ser.write(str.encode(f"{output}|{dot}\n"))

def showTimeAMPN():
  now = datetime.datetime.now()
  output = ""
  dot = 0

  hour = now.hour % 13
  hourStr = f'{hour}'

  if hour < 10:
      hourStr = f'0{hour}'

  if now.second % 2 == 1:
    dot = 0
  else:
    dot = 32

  if showSec == True:
    output = f'{hourStr}{now.strftime("%M")} {now.strftime("%S")}S'
  else:
    AP = "ANN"
    if now.hour / 12 > 0:
        AP = "PNN"

    output = f'{hourStr}{now.strftime("%M")} {AP}'

  ser.write(str.encode(f"{output}|{dot}\n"))



def exit_handler():
  ser.write(str.encode(f"nready__|0\n"))


atexit.register(exit_handler)


s.enter(tick, 1, showTime, (s,))
s.run()
