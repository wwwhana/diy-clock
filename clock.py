import subprocess
import serial
import datetime
import time
from apscheduler.schedulers.background import BackgroundScheduler

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


def initClock():
  subprocess.run('gpio mode 10 in'.split(" "), stdout=subprocess.DEVNULL)
  cmd = f"SETMODE|CLOCK\n"
  ser.write(str.encode(cmd))
def readBright():
  r = subprocess.run('gpio read 10'.split(" "), stdout=subprocess.PIPE, text=True).stdout.strip()
  return int(r.strip())

def syncTime():
   now = datetime.datetime.now()
   cmd = f"SETTIME|{now.strftime('%Y.%m.%d %H:%M:%S')}\n"
   ser.write(str.encode(cmd))

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

  ser.write(str.encode(f"SETBR|{bright}\n"))


ser.isOpen()
startup = datetime.datetime.now()

initClock()
setBright()
syncTime()


sched = BackgroundScheduler()
sched.start()

sched.add_job(syncTime, 'interval', seconds=10, id="tick_event")
sched.add_job(setBright, 'interval', seconds=5, id="bright_event")

before  = datetime.datetime.now()

while True:
  now = datetime.datetime.now()
  diff = (now - before).seconds
  before = now

  if diff > 3:
    exit(1)

  time.sleep(1)
