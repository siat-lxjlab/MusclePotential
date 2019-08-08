from bluepy.btle import Peripheral, UUID

p = Peripheral('DE:5B:BA:87:4C:F7', 'random')

try:
    chList = p.getCharacteristics()
    for ch in chList:
        print('0x' + format(ch.getHandle(), '02X') + "  " + str(ch.uuid) + "  " + ch.propertiesToString())
finally:
    p.disconnect()

