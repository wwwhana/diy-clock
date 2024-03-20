import serial
import datetime
import time
from apscheduler.schedulers.background import BackgroundScheduler
import json

#시리얼포트 객체 ser을 생성
#pc와 스위치 시리얼포트 접속정보
ser = serial.Serial(
    port = '/dev/ttyS1',
    baudrate=9600,
    parity='N',
    stopbits=1,
    bytesize=8,
    timeout=8
    )

def initClock():

  cmd = f"SETMODE|DETAILCLOCK\n"
  ser.write(str.encode(cmd))

def readSensor():
  while True:
    try:
      line = ser.readline().strip()
      print(line)
      if line == b'':
        continue
      r = json.loads(line)
      with open("/opt/clock/sensors.json", "w") as f:
        f.write(line.decode('utf-8'))
        break
    except Exception as e:
      print(e)
      pass

def isShadow():
  with open("/opt/clock/sensors.json", "r") as f:
    line = f.readline()
    return json.loads(line)["shadow"]

def syncTime():
   now = datetime.datetime.now()
   cmd = f"SETTIME|{now.strftime('%Y.%m.%d %H:%M:%S')}\n"
   ser.write(str.encode(cmd))

def setBright():
  bright = 0
  now = datetime.datetime.now()

  if isShadow() is False:
    if now.hour >= 22 or now.hour < 7:
      bright = 0
    else:
      bright = 70
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

sched.add_job(syncTime, 'interval', seconds=300, id="sync_event")
sched.add_job(readSensor, 'interval', seconds=3, id="tick_event")
sched.add_job(setBright, 'interval', seconds=4, id="bright_event")

before  = datetime.datetime.now()

while True:
  now = datetime.datetime.now()
  diff = (now - before).seconds
  before = now

  if diff > 3:
    syncTime()
    exit(1)

  time.sleep(1)
