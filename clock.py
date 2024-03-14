import wiringpi
import subprocess
import serial
import datetime
import time
from apscheduler.schedulers.background import BackgroundScheduler
import dht11

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

lightSensorPin = 10
Dht11Pin = 17
INPUT = 0
OUTPUT = 1

def initClock():
  wiringpi.wiringPiSetup()
  wiringpi.pinMode(lightSensorPin, INPUT)

  cmd = f"SETMODE|DETAILCLOCK\n"
  ser.write(str.encode(cmd))

def readBright():
  return wiringpi.digitalRead(lightSensorPin)

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

def setTempertureHumidity():
  done = False

  while not done:
    result = sensor.read()
    if result.is_valid():
      print(f"{result.humidity}|{result.temperature}")
      ser.write(str.encode(f"SETHUMIDITY|{result.humidity}\n"))
      time.sleep(1)
      ser.write(str.encode(f"SETTEMPERATURE|{result.temperature}\n"))
      print("X")
      done = True

ser.isOpen()
sensor = dht11.DHT11(pin = Dht11Pin)

startup = datetime.datetime.now()

initClock()
setBright()
syncTime()
setTempertureHumidity()

sched = BackgroundScheduler()
sched.start()

sched.add_job(syncTime, 'interval', seconds=10, id="tick_event")
sched.add_job(setBright, 'interval', seconds=5, id="bright_event")
sched.add_job(setTempertureHumidity, 'interval', seconds=5, id="temperture_humidity_event")

before  = datetime.datetime.now()

while True:
  now = datetime.datetime.now()
  diff = (now - before).seconds
  before = now

  if diff > 3:
    syncTime()
    exit(1)

  time.sleep(1)
