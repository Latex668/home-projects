import pygame
import pygame.gfxdraw
import math
import numpy as np
pygame.init()
screen = pygame.display.set_mode((800, 800))
clock = pygame.time.Clock()
running = True
 
def drawPixel(color, pos): # pixel size is 10x bigger
    screen.fill(color, (pos, (10,10)))

def drawLine(x1, y1, x2, y2, color): # bresenham's line generation algorithm
    m_new = 2 * (y2-y1)
    slope_err_new = m_new - (x2-x1)
    y = y1
    for x in range(x1, x2+1):
        drawPixel(color, (x,y))
        slope_err_new = slope_err_new + m_new
        if (slope_err_new >= 0):
            y = y+1
            slope_err_new = slope_err_new - 2 * (x2-x1)

def drawCircle(x, y, radius, color):
    numberOfLines = 512
    for i in range(0,numberOfLines):
        angle1 = i * (2*math.pi/numberOfLines)
        angle2 = (i+1) * (2*math.pi/numberOfLines)
        a1 = x + radius*math.cos(angle1)
        b1 = y + radius*math.sin(angle1)
        a2 = x + radius*math.cos(angle2)
        b2 = y + radius*math.sin(angle2)
        drawLine(int(a1),int(b1),int(a2),int(b2),color)

while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
            pygame.quit()

    if running:
        screen.fill("purple")
        drawCircle(400,400,50,"white")
        drawCircle(500,400,50,"white")
        drawCircle(400,400,10,"white")
        drawCircle(500,400,10,"white")

        pygame.display.flip()
        clock.tick(60)
