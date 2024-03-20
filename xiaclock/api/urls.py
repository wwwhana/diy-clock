from django.urls import include, path

urlpatterns = [
    path("clock/", include("xiaclock.api.clocks.urls")),
    path("sensors/", include("xiaclock.api.sensors.urls")),
    path("service/", include("xiaclock.api.service.urls")),
]
