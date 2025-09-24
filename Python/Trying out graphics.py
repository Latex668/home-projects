import pygame
import math
pygame.init()
screen = pygame.display.set_mode((800, 800))
clock = pygame.time.Clock()
running = True
 
def drawPixel(color, x, y): # pixel size is 10x bigger
    screen.fill(color, ((x, y), (10,10)))

def drawLine(color, x1, y1, x2, y2): # bresenham's line generation algorithm
    if abs(x2-x1) > abs(y2-y1): # draw horizontal line
        if x1 > x2:
            x1, x2 = x2, x1
            y1, y2 = y2, y1
        dx = x2 - x1
        dy = y2 - y1
        dir = -1 if dy < 0 else 1
        dy *= dir
        if dx != 0:
            y = y1
            p = 2*dy - dx
            for i in range(dx+1):
                drawPixel(color, x1+i, y)
                if p >= 0:
                    y += dir
                    p = p - 2*dx
                p = p + 2*dy
    else: # draw vertical line
        if y1 > y2:
            y1, y2 = y2, y1
            x1, x2 = x2, x1
        dx = x2 - x1
        dy = y2 - y1
        dir = -1 if dx < 0 else 1
        dx *= dir
        if dy != 0:
            x = x1
            p = 2*dx - dy
            for i2 in range(dy+1):
                drawPixel(color, x, y1+i2)
                if p >= 0:
                    x += dir
                    p = p - 2*dy
                p = p + 2*dx

def drawRect(color, x, y, width, height):
    drawLine(color, x, y, x+width, y) # top line
    drawLine(color, x, y+height, x+width, y+height) # bottom line
    drawLine(color, x, y, x, y+height) # left line
    drawLine(color, x+width, y, x+width, y+height) # right line

def drawFilledRect(color, x, y, width, height):
    for i in range(height):
        drawLine(color, x, y+i, x+width, y+i)

def drawTriangle(color, x1, y1, x2, y2, x3, y3): # using lines to draw triangle
    drawLine(color, x1, y1, x2, y2)
    drawLine(color, x1, y1, x3, y3)
    drawLine(color, x2, y2, x3, y3)

def drawCircle(color, cx, cy, radius): # midpoint circle algorithm
    x = 0
    y = -radius
    p = 1-radius
    while x < -y:
        if p > 0:
            y += 1
            p += 2*(x+y) + 1
        else: 
            p += 2*x + 1
        drawPixel(color, cx+x, cy+y)
        drawPixel(color, cx-x, cy+y)
        drawPixel(color, cx+x, cy-y)
        drawPixel(color, cx-x, cy-y)
        drawPixel(color, cx+y, cy+x)
        drawPixel(color, cx+y, cy-x)
        drawPixel(color, cx-y, cy+x)
        drawPixel(color, cx-y, cy-x)
        
        x += 1
        
def drawFilledCircle(color, cx, cy, radius): # midpoint circle algorithm as well, but using lines to fill in the circle
    x = 0
    y = -radius
    p = 1-radius
    while x < -y:
        if p > 0:
            y += 1
            p += 2*(x+y) + 1
        else: 
            p += 2*x + 1
        drawLine(color, cx-x, cy+y, cx+x, cy+y)
        drawLine(color, cx-x, cy-y, cx+x, cy-y)
        drawLine(color, cx+y, cy+x, cx-y, cy+x)
        drawLine(color, cx+y, cy-x, cx-y, cy-x)
        x += 1
      
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
            pygame.quit()

    if running:
        screen.fill("purple")
        drawTriangle("white", 100, 300, 300, 300, 200, 100) 
        drawPixel("green", 100, 300)
        drawPixel("green", 300, 300)
        drawPixel("green", 200, 100)
        
        pygame.display.flip()
        clock.tick(60)
