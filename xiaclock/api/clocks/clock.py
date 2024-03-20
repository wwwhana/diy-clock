
from django.http import HttpResponse, HttpRequest
from xiaclock.segment import segment
import datetime

display = segment

def syncTime(request: HttpRequest):
    now = datetime.datetime.now()
    display.setTime(now)
    return HttpResponse()

def setClockMode(request: HttpRequest):
    display.ShowClock()
    return HttpResponse()

def setBlinkMode(request: HttpRequest):
    display.HiddenDisplay()
    return HttpResponse()

def showMsg(request: HttpRequest):
    msg = request.GET.get("msg")

    try:
        duration = int(request.GET.get("duration"))
    except :
        duration = 5

    try:
        dot = int(request.GET.get("dot"))
    except :
        dot = 5

    if msg is None:
        return HttpResponse("Error")
    else:
        length = 8 - len(msg)
        padding = "".join( [" " for v in range(0, length)])
        msg = padding + msg
        msg = msg[0:8]

    display.showMessage(msg, dot, duration)
    return HttpResponse(f"{msg}.{dot} {duration} seconds")
