from django.urls import include, path
from . import service

urlpatterns = [
    path("shairport", service.restartSharport, name="shairport"),
]
