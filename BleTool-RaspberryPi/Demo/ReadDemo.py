from bluepy.btle import UUID, Peripheral

uuid_list = [0x2A00, 0x2A01, 0x2A04, 0x2AA6, 0x2A05, 0x2A37, 0x2A38, 0x2A29]
for uuid in uuid_list:
    dev_name_uuid = UUID(uuid)

    p = Peripheral('DE:5B:BA:87:4C:F7', 'random')

    try:
        ch = p.getCharacteristics(uuid=dev_name_uuid)[0]
        if(ch.supportsRead()):
            print(ch.read())

    finally:
        p.disconnect()
