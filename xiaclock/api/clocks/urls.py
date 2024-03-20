from django.urls import include, path
from . import clock

urlpatterns = [
    path("sync", clock.syncTime, name="sync"),
    path("msg", clock.showMsg, name="showMsgGet"),
    path("blink", clock.setBlinkMode, name="setBlink"),
]
