from django.http import HttpResponse, JsonResponse, HttpRequest
from xiaclock.segment import segment
import logging

logger = logging.getLogger(__name__)

display = segment

def index(request: HttpRequest):
    with open("/opt/clock/sensors.json", "r") as f:
        str = f.readline()
        return HttpResponse(str, content_type="application/json")
