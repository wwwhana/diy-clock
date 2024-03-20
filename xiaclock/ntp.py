import ntplib
from time import localtime

def getTime():
    # NTP_Server = 'time.windows.com'
    NTP_Server = 'time-a.nist.gov'
    ntp_client = ntplib.NTPClient()
    response = ntp_client.request(NTP_Server)

    return localtime(response.tx_time)


if __name__ == "__main__":
    getTime()