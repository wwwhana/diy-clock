from serial import Serial
import datetime
from .config import config
from enum import Enum
import json

class SegmentCommand(Enum):
    SETTIME = "SETTIME"
    SETBRIGHT = "SETBR"
    SETMODE = "SETMODE"
    SETSTRING = "SETSTR"
    GETTIME= "GETTIME"

class SegmentMode(Enum):
    SHOWSTRING = "STR"
    BLINK = "BLINK"
    SHOWCLOCK = "CLOKC"

class _Segment:
    _serial: Serial
    _isConnected = False

    def __new__(cls, *args, **kwargs):
        if not hasattr(cls, "_instance"):         # Foo 클래스 객체에 _instance 속성이 없다면
            cls._instance = super().__new__(cls)  # Foo 클래스의 객체를 생성하고 Foo._instance로 바인딩
        
        return cls._instance   

    def __init__(self, port, baudrate=19200, parity="N", stopbits=1, bytesize=8, timeout= 8):
        cls = type(self)
        if not hasattr(cls, "_init"):             # Foo 클래스 객체에 _init 속성이 없다면
            if port is not None:
                self.open(port, baudrate, parity, stopbits, bytesize, timeout)

        cls._init = True

    def isConnected(self):
        return self._isConnected

    def open(self, port, baudrate=19200, parity="N", stopbits=1, bytesize=8, timeout= 8):
        if self._isConnected:
            return
        print(f"{port}:{baudrate}")
        try:
            self._serial = Serial(
                port = port,
                baudrate=baudrate,
                parity=parity,
                stopbits=stopbits,
                bytesize=bytesize,
                timeout=timeout
            )
            self._serial.isOpen()
            self._isConnected = True
        except:
           self._isConnected = False

    def close(self):
        self._serial.close()
        self._isConnected = False

    def _write(self, cmd: SegmentCommand, msg: str):
        str = f"{cmd._value_}|{msg}\n"
        if self._isConnected:
            self._serial.write(str.encode())
        else:
            print(str)

    def _setMode(self, mode: SegmentMode):
        self._write(SegmentCommand.SETMODE, mode._value_)

    def setBright(self, brightness = 70):
        if brightness < - 200:
            brightness = - 200
        elif brightness > 200:
            brightness = 200
            
        msg = f"{brightness}"
        self._write(SegmentCommand.SETBRIGHT, msg)

    def setTime(self, value: datetime.datetime):
        msg = f"{value.year}.{value.month}.{value.day} {value.hour}:{value.minute}:{value.second}"
        self._write(SegmentCommand.SETTIME, msg)

    def showMessage(self, msg, dot: int, duration = 5):
        msg = f"{msg}|{dot}|{duration}"
        self._setMode(SegmentMode.SHOWSTRING)
        self._write(SegmentCommand.SETSTRING, msg)

    def ShowClock(self):
        self._setMode(SegmentMode.SHOWCLOCK)

    def HiddenDisplay(self):
        self._setMode(SegmentMode.BLINK)
    
    def readSensor(self):
        str = ""

        while not str.startswith("{") and not str.endswith("}"):
            str = self._serial.readline().strip()
            try:
                return json.loads(str)
            except:
                pass




segment = _Segment(config["port"], config["baudrate"], config["parity"], config["stopbits"], config["bytesize"], config["timeout"])


