from django.urls import include, path
from . import sensors

urlpatterns = [
    path("", sensors.index, name="sensors"),
]
