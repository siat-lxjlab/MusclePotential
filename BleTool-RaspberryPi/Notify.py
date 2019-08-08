import binascii
import time
import struct
from bluepy.btle import UUID, Peripheral, DefaultDelegate
import numpy as np
import matplotlib.pyplot as plt

import matplotlib
print(matplotlib.get_backend())

# x scale - time 
t = []
# record the current time
t_now = 0
# y scale - muscle current
m = []

sensor_service = UUID(0x180D)

p = Peripheral('DE:5B:BA:87:4C:F7', 'random')

SensorService = p.getServiceByUUID(sensor_service)

ch = SensorService.getCharacteristics(0x2A37)[0]
print(UUID(ch.uuid).getCommonName())

chC = ch.getHandle()

for desc in p.getDescriptors(chC, 0x16):
    if desc.uuid == 0x2902:
        chC = desc.handle

p.writeCharacteristic(chC, struct.pack('<bb', 0x01, 0x00))

class MyDelegate(DefaultDelegate):
    def __init__(self, params):
       DefaultDelegate.__init__(self)

    def handleNotification(self, cHandle, data):
        plt.ion()
        plt.figure(1)
        plt.clf()
        
        global t_now, t, m
        t_now += 0.1
        if t_now > 1000:
            t = 0.1*[range(1, 101)]
            return

        circle = len(t)
        if circle == 100:
            del(t[0])
            del(m[0])

        t.append(t_now)

        value = int.from_bytes(data, byteorder='little', signed=False)
        m.append(value>>8)
        print(circle, value>>8)

        plt.plot(t, m, '-r')
        # plt.draw()
        # time.sleep(0.01)
        plt.pause(0.001)

p.setDelegate(MyDelegate(p))

while True:
    if p.waitForNotifications(0.001):
        continue
    print('Waiting...')
