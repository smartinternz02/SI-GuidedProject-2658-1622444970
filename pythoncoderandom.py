import wiotp.sdk.device
import time
import random
myConfig = { 
    "identity": {
        "orgId": "whkvy8",
        "typeId": "ESP32",
        "deviceId":"88854"
    },
    "auth": {
        "token": "987654321"
    }
}

def myCommandCallback(cmd):
    print("Message received from IBM IoT Platform: %s" % cmd.data['command'])
    m=cmd.data['command']

client = wiotp.sdk.device.DeviceClient(config=myConfig, logHandlers=None)
client.connect()

while True:
    ammonia=random.randint(0,100)
    flame=random.randint(0,1)
    temp=random.randint(0,100)
    dist=random.randint(0,400)
    hum=random.randint(0,100)
    
    myData={'NH3':ammonia, 'flame':flame,'temperature':temp,'humidity':hum,'ultrasonic':dist}
    client.publishEvent(eventId="status", msgFormat="json", data=myData, qos=0, onPublish=None)
    print("Published data Successfully: %s", myData)
    client.commandCallback = myCommandCallback
    time.sleep(4)
client.disconnect()
