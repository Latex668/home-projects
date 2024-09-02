import serial
import time

esp = serial.Serial(port='COM4', baudrate=115200,timeout=.1)

def write_read(txt):
    esp.write(bytes(txt, 'UTF-8'))
    time.sleep(0.05)
    data = esp.readline()
    return data

while True:
    print(write_read('Ping!'))
    time.sleep(0.1)