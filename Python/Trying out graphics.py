from numba import jit, cuda
import math
import pygame
import numpy as np

# TO-DO:
# switch from coords to vectors

pygame.init()

# Set up the game window
screen = pygame.display.set_mode((800, 800))
pygame.display.set_caption("Hello Pygame")
clock = pygame.time.Clock()
running = True

def drawPixel(color, posVector): # pixel size is 10x bigger, rgba optional, posVector = [x,y]
    colorRGB = (pygame.Color(color).premul_alpha())
    # calculate opacity
    screen.fill(colorRGB, (posVector, (10,10)))

def drawLine(color, posVector): # bresenham's line generation algorithm, posVector = [[x0,x1],[y0,y1]]
    if abs(posVector[0][1]-posVector[0][0]) > abs(posVector[1][1]-posVector[1][0]): # draw horizontal line
        if posVector[0][0] > posVector[0][1]:
            posVector[0][0], posVector[0][1] = posVector[0][1], posVector[0][0]
            posVector[1][0], posVector[1][1] = posVector[1][1], posVector[1][0]
        dx = posVector[0][1] - posVector[0][0]
        dy = posVector[1][1] - posVector[1][0]
        dir = -1 if dy < 0 else 1
        dy *= dir
        if dx != 0:
            y = posVector[1][0]
            p = 2*dy - dx
            for i in range(dx+1):
                drawPixel(color, [posVector[0][0]+i, y])
                if p >= 0:
                    y += dir
                    p = p - 2*dx
                p = p + 2*dy
    else: # draw vertical line
        if posVector[1][0] > posVector[1][1]:
            posVector[1][0], posVector[1][1] = posVector[1][1], posVector[1][0]
            posVector[0][0], posVector[0][1] = posVector[0][1], posVector[0][0]
        dx = posVector[0][1] - posVector[0][0]
        dy = posVector[1][1] - posVector[1][0]
        dir = -1 if dx < 0 else 1
        dx *= dir
        if dy != 0:
            x = posVector[0][0]
            p = 2*dx - dy
            for i2 in range(dy+1):
                drawPixel(color, [x, posVector[1][0]+i2])
                if p >= 0:
                    x += dir
                    p = p - 2*dy
                p = p + 2*dx


def drawRect(color, posVector): # posVector = [[x,width],[y,height]]
    drawLine(color, [[posVector[0][0]                , posVector[0][0]+posVector[0][1]], [posVector[1][0]                , posVector[1][0]                ]]) # top line
    drawLine(color, [[posVector[0][0]                , posVector[0][0]+posVector[0][1]], [posVector[1][0]+posVector[1][1], posVector[1][0]+posVector[1][1]]]) # bottom line
    drawLine(color, [[posVector[0][0]                , posVector[0][0]                ], [posVector[1][0]                , posVector[1][0]+posVector[1][1]]]) # left line
    drawLine(color, [[posVector[0][0]+posVector[0][1], posVector[0][0]+posVector[0][1]], [posVector[1][0]                , posVector[1][0]+posVector[1][1]]]) # right line

def drawFilledRect(color, posVector): # posVector = [[x,width],[y,height]]
    for i in range(posVector[1][1]):
        drawLine(color, [[posVector[0][0], posVector[0][0]+posVector[0][1]], [posVector[1][0]+i, posVector[1][0]+i]])

def drawTriangle(color, posVector): # using lines to draw triangle, posVector = [[x0,x1,x2],[y0,y1,y2]]
    drawLine(color, [[posVector[0][0], posVector[0][1]],[posVector[1][0], posVector[1][1]]])
    drawLine(color, [[posVector[0][0], posVector[0][2]],[posVector[1][0], posVector[1][2]]])
    drawLine(color, [[posVector[0][1], posVector[0][2]],[posVector[1][1], posVector[1][2]]])

def drawCircle(color, posVector): # midpoint circle algorithm, posVector= [cx,ry,radius]
    x = 0
    y = -posVector[2]
    p = 1-posVector[2]
    while x < -y:
        if p > 0:
            y += 1
            p += 2*(x+y) + 1
        else: 
            p += 2*x + 1
        drawPixel(color, [posVector[0]+x, posVector[1]+y])
        drawPixel(color, [posVector[0]-x, posVector[1]+y])
        drawPixel(color, [posVector[0]+x, posVector[1]-y])
        drawPixel(color, [posVector[0]-x, posVector[1]-y])
        drawPixel(color, [posVector[0]+y, posVector[1]+x])
        drawPixel(color, [posVector[0]+y, posVector[1]-x])
        drawPixel(color, [posVector[0]-y, posVector[1]+x])
        drawPixel(color, [posVector[0]-y, posVector[1]-x])
        
        x += 1
        
def drawFilledCircle(color, posVector): # midpoint circle algorithm as well, but using lines to fill in the circle
                                             # posVector = [cx,cy,radius] 
    x = 0
    y = -posVector[2]
    p = 1-posVector[2]
    while x < -y:
        if p > 0:
            y += 1
            p += 2*(x+y) + 1
        else: 
            p += 2*x + 1
        drawLine(color, [[posVector[0]-x, posVector[0]+x], [posVector[1]+y, posVector[1]+y]])
        drawLine(color, [[posVector[0]-x, posVector[0]+x], [posVector[1]-y, posVector[1]-y]])
        drawLine(color, [[posVector[0]+y, posVector[0]-y], [posVector[1]+x, posVector[1]+x]])
        drawLine(color, [[posVector[0]+y, posVector[0]-y], [posVector[1]-x, posVector[1]-x]])
        x += 1

def drawGrid(boxSize): # draw a colored grid with dimensions boxSize x BoxSize
    screenSizeX = 800
    screenSizeY = 800
    for y in range(0, screenSizeY, boxSize): # draw the horizontal lines
        drawLine("black", [[0, screenSizeX], [y, y]])
    for x in range(0, screenSizeX, boxSize): # draw the vertical lines
        drawLine("black", [[x, x], [0, screenSizeY]])

Vector2D = [[100,300,200],[100,200,300]] # [x0,x1,xn],[y0,y1,yn]
Triangle1 = [[300,450,365],[300,365,450]]
Vector1D = [200,200,100]
Vector3D = [[70, 240, 140, 240, 140, 240, 140, 240],
            [140, 140, 240, 240, 140, 140, 240, 240],
            [10 , 10 , 10 , 10 , 100, 100, 100, 100]] # weird, [2][0] gives me z0

def vectorMultiplication(Vector1, Vector2):
    
    if np.shape(Vector1) == np.shape(Vector2): 
        Vector1 = np.transpose(Vector1)
        return np.dot(Vector2,Vector1) 
    else:
        print("Vectors not equal!")
        
angle = 30
decreasing = True
def main():
    # Game loop
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            screen.fill("black")
            drawTriangle("red",Vector2D)
            angleRadians = math.radians(angle)
            Triangle1[0][0] = int(Triangle1[0][0]*math.cos(angleRadians)-Triangle1[1][0]*math.sin(angleRadians))  
            Triangle1[0][1] = int(Triangle1[0][1]*math.cos(angleRadians)-Triangle1[1][1]*math.sin(angleRadians))  
            Triangle1[0][2] = int(Triangle1[0][2]*math.cos(angleRadians)-Triangle1[1][2]*math.sin(angleRadians))  
            drawTriangle("green",Triangle1)
            print(angle)
            angle+=1
            pygame.display.update()
    # Quit Pygame
    pygame.quit()

if __name__=="__main__":
    main()