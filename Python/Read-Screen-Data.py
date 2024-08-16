# The purpose of thise code is to read ingame data via my screen because I couldn't find the api for it and I can't look at logs
import numpy as nm
import pytesseract
import cv2 # OpenCV
from PIL import ImageGrab
import pyautogui

def printScreenCoords():
    print(pyautogui.mouseInfo())


def ImgToStr():
    # Path of tesseract executable 
    pytesseract.pytesseract.tesseract_cmd = "C:\Program Files\Tesseract-OCR/tesseract.exe"
    while(True): 
        # ImageGrab-To capture the screen image in a loop.  
        # Bbox used to capture a specific area. 
        cap = ImageGrab.grab(bbox =(645, 98, 1130, 121)) 
        
        # Converted the image to monochrome for it to be easily  
        # read by the OCR and obtained the output String. 
        tesstr = pytesseract.image_to_string( 
                cv2.cvtColor(nm.array(cap), cv2.COLOR_BGR2GRAY),  
                lang ='eng') 
        print(tesstr) 

# Calling the function 
ImgToStr() 