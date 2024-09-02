# The purpose of this code is to read ingame data via my screen because I couldn't find the api for it and I can't look at logs
import numpy as nm
import pytesseract
import cv2 # OpenCV
from PIL import ImageGrab
import pyautogui
import re


def printScreenCoords():
    print(pyautogui.mouseInfo())
printScreenCoords()
def ImgToStr():
    # Path of tesseract executable 
    pytesseract.pytesseract.tesseract_cmd = "C:\Program Files\Tesseract-OCR/tesseract.exe"
    while(True): 
        # ImageGrab-To capture the screen image in a loop.  
        # Bbox used to capture a specific area. 
        health = ImageGrab.grab(bbox =(420, 870, 550, 940)) 
        killstreak = ImageGrab.grab(bbox =(390, 940, 540, 990))
        # Converted the image to monochrome for it to be easily  
        # read by the OCR and obtained the output String. 
        healthstr = re.sub("\D", "", pytesseract.image_to_string(cv2.cvtColor(nm.array(health), cv2.COLOR_BGR2GRAY), lang ='eng'))
        killstreakstr = re.sub("\D", "", pytesseract.image_to_string( cv2.cvtColor(nm.array(killstreak), cv2.COLOR_BGR2GRAY), lang ='eng'))
        print(healthstr)
        print(killstreakstr)

# Calling the function 
ImgToStr() 