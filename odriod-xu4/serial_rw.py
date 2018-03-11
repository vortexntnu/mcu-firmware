#!/usr/bin/python
import wiringpi2 as wpi
import time

device = '/dev/ttySAC0'
baud = 38400

serial = wpi.serialOpen(device, baud)
while True:
        wpi.serialPuts(serial, 'OleErSjefen')
        time.sleep(0.001)
        output_str = 'Device ' + device + ' at baudrate ' + str(baud) +  ' output:  '
        if wpi.serialDataAvail(serial):
                output_str += chr(wpi.serialGetchar(serial))
                print(output_str)
        else:
                print("seriaDataAvail(serial)")

wpi.serialClose(serial)
