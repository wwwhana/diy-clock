import serial
import datetime
import time
import atexit
from apscheduler.schedulers.background import BackgroundScheduler

# 시리얼포트 객체 ser을 생성
# pc와 스위치 시리얼포트 접속정보
ser = serial.Serial(
    port="/dev/ttyS3", baudrate=9600, parity="N", stopbits=1, bytesize=8, timeout=8
)

i = 0

showAMPM = False
showSec = False
tick = 0.1


def setBrightDaily():
    ser.write(str.encode(f"[BR]|70\n"))


def setBrightNightly():
    ser.write(str.encode(f"[BR]|0\n"))


def showTime():
    if showAMPM == True:
        showTimeAMPN()
    else:
        showTimeDefault()


def showTimeDefault():
    now = datetime.datetime.now()
    dot = 0
    output = ""

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
    hourStr = f"{hour}"

    if hour < 10:
        hourStr = f"0{hour}"

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

if startup.hour >= 22 or startup.hour < 6:
    setBrightNightly()
else:
    setBrightDaily()

atexit.register(exit_handler)

sched = BackgroundScheduler()
sched.start()

sched.add_job(showTime, "interval", seconds=1, id="tick_event")

sched.add_job(setBrightNightly, "cron", hour=22, minute="00", second="00", id="bright1")
sched.add_job(setBrightDaily, "cron", hour=6, minute="00", second="00", id="bright2")

while True:
    time.sleep(1)
