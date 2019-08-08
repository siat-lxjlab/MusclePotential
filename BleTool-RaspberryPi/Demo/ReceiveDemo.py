from bluepy.btle import UUID, Peripheral, DefaultDelegate

class MyDelegate(DefaultDelegate):
    def __init__(self, params):
       DefaultDelegate.__init__(self)

    def handleNotification(self, cHandle, data):
        print('Notification from Handle: 0x' + format(cHandle, '02X') +" Value: "+ format(ord(data[0])))

sensor_service = UUID(0x2A37)

p = Peripheral('DE:5B:BA:87:4C:F7', 'random')

p.setDelegate(MyDelegate(p))

SensorService = p.getServiceByUUID(sensor_service)

print(SensorService.getCharacteristics())

while True:
    if p.waitForNotifications(1.0):
        continue
    print('Waiting...')
