from bluepy.btle import UUID, Peripheral


p = Peripheral('DE:5B:BA:87:4C:F7', 'random')

services = p.getServices()

for service in services:
    print(service)
