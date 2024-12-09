# Code to send serial data to pi pico with external 16x2 lcd
from os import _exit
from infi.systray import SysTrayIcon
def onQuit(systray):
    _exit(0)
systray = SysTrayIcon("D:\Programming\Home projects\home-projects\Python\Serial2Pico\icon.ico","Serial2Pico",None, on_quit=onQuit)
systray.start()
# All the stuff above is basically to just make a tray icon lul
import datetime
import serial
import serial.tools.list_ports
import time

def checkPorts(): # Function to check all the ports, returns true if com12 is there (which is the port of my pico)
    ports = (comport.device for comport in serial.tools.list_ports.comports())
    if "COM12" in ports:
        return True
    else:
        return False

def restartProgram(): # if error, retry program lol
    while checkPorts() == False:
        print("COM12 isn't open.")
        time.sleep(1)
    if checkPorts(): # if COM12 open, run main program
        print("COM12 Open")
        mainProgram()

def mainProgram():
    pico = serial.Serial(port="COM12", baudrate=115200,timeout=.1) # set serial to port 12 baud 115200

    def write_read(txt): # function to send and receive input from serial.
        pico.write(bytes(txt, 'UTF-8'))
        time.sleep(0.05)
        data = pico.readline()
        return data

    while True: # main code, try to send data, if error restart program
        try:
            dt = datetime.datetime.now()
            timeNow = dt.strftime("%H:%M") # Get the time now as h:m format
            dateNow = dt.strftime("%d/%m/%y")
            write_read(f"Time: {timeNow}     , Date: {dateNow}   \n") # I'm sending the data in such a weird way cuz random characters would get displayed on my lcd whenever i received them, so my solution was to add spaces lol.
            time.sleep(1)
        except:
            restartProgram()

restartProgram()