import subprocess
import serial
import datetime
import time
import atexit
from apscheduler.schedulers.background import BackgroundScheduler
from apscheduler.jobstores.base import JobLookupError

#시리얼포트 객체 ser을 생성
#pc와 스위치 시리얼포트 접속정보
ser = serial.Serial(
    port = '/dev/ttyS3',
    baudrate=9600,
    parity='N',
    stopbits=1,
    bytesize=8,
    timeout=8
    )

i = 0

showAMPM = False
showSec = False
tick = 0.1

def initBright():
  subprocess.run('gpio mode 10 in'.split(" "), stdout=subprocess.DEVNULL)

def readBright():
  r = subprocess.run('gpio read 10'.split(" "), stdout=subprocess.PIPE, text=True).stdout.strip()
  return int(r.strip())

def showTime():
  if showAMPM == True:
      showTimeAMPN()
  else:
    showTimeDefault()

def setBright():
  bright = 0
  now = datetime.datetime.now()

  if readBright() == 0:
    bright = 70
  else:
    if now.hour >= 22 or now.hour < 7:
      bright = 0
    else:
      bright = 30

  print(f'{now.hour}|{bright}')

  ser.write(str.encode(f"[BR]|{bright}\n"))

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

ser.isOpen()
ser.write(str.encode(f"88888888|255\n"))

startup = datetime.datetime.now()

initBright()
setBright()

atexit.register(exit_handler)

sched = BackgroundScheduler()
sched.start()

sched.add_job(showTime, 'interval', seconds=1, id="tick_event")
sched.add_job(setBright, 'interval', seconds=1, id="bright_event")

before  = datetime.datetime.now()


while True:
  now = datetime.datetime.now()
  diff = (now - before).seconds
  before = now

  if diff > 3:
    exit(1)

  time.sleep(1)
