from django.http import HttpResponse, HttpRequest
import os

def restartSharport(request: HttpRequest):
  os.popen("/opt/bt.sh")
  line = os.popen("systemctl restart shairport-sync.service")
  os.popen("/opt/bt-disconnect.sh")
  return HttpResponse("OK")