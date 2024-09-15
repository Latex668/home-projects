# The purpose of this code is to read ingame data via my screen because I couldn't find the api for it and I can't look at logs
import numpy as nm
import pytesseract
import cv2 # OpenCV
from PIL import ImageGrab
import pyautogui
import re
import serial
import time

esp = serial.Serial(port='COM4', baudrate=115200,bytesize=8,parity='N',stopbits=1,timeout=.1)

def write_read(txt):
    esp.write(bytes(txt, 'UTF-8'))
    time.sleep(0.05)
    data = esp.readline()
    return data

def printScreenCoords(): # Small function to get my screen coords to make the box
    print(pyautogui.mouseInfo())
def ImgToStr():
    # Path of tesseract executable 
    pytesseract.pytesseract.tesseract_cmd = "C:\Program Files\Tesseract-OCR/tesseract.exe"
    while(True): 
        # ImageGrab-To capture the screen image in a loop.  
        # Bbox used to capture a specific area. 
        health = ImageGrab.grab(bbox =(388, 857, 566, 940)) 
        killstreak = ImageGrab.grab(bbox =(390, 940, 540, 990))
        # Converted the image to monochrome for it to be easily  
        # read by the OCR and obtained the output String. 
        healthstr = re.sub("\D", "", pytesseract.image_to_string(health, lang ='eng')) # Grabbing the health info whilst removing anything that's not a digit, same thing with line below
        killstreakstr = re.sub("\D", "", pytesseract.image_to_string( cv2.cvtColor(nm.array(killstreak), cv2.COLOR_BGR2GRAY), lang ='eng'))
        if(healthstr):
            time.sleep(0.02)
            write_read("h" + healthstr)
        elif(healthstr == ""):
            time.sleep(0.02)
            write_read("h0")
        if(killstreakstr):
            time.sleep(0.02)
            write_read("k" + killstreakstr)
        elif(killstreakstr == ""):
            time.sleep(0.02)
            write_read("k0")
ImgToStr() 