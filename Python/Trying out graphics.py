import pygame
import pygame.gfxdraw
import math
import numpy as np
pygame.init()
screen = pygame.display.set_mode((800, 800))
clock = pygame.time.Clock()
running = True



def drawPoint(x,y, color):
    pygame.gfxdraw.filled_circle(screen,x,y,10,pygame.Color(color))


def rotatedX(angle, posMatrix):
    rotationMatrix = ([1,0,0],
                      [0,math.cos(angle), -math.sin((angle))],
                      [0, math.sin(angle), math.cos((angle))])
    res = np.dot(rotationMatrix, posMatrix)
    x = res[0][0].item()
    y = res[1][0].item()
    pos = (x,y)
    return pos

def rotatedY(angle, posMatrix):
    rotationMatrix = ([math.cos(angle), 0, math.sin(angle)],
                      [0, 1, 0],
                      [-math.sin(angle), 0, math.cos((angle))])
    res = np.dot(rotationMatrix, posMatrix)
    x = res[0][0].item()
    y = res[1][0].item()
    pos = (x,y)
    return pos

def rotatedZ(angle, posMatrix):
    rotationMatrix = ([math.cos(angle), -math.sin(angle),0],
                      [math.sin(angle), math.cos((angle)),0],
                      [0, 0, 1])
    res = np.dot(rotationMatrix, posMatrix)
    x = res[0][0].item()
    y = res[1][0].item()
    pos = (x,y)
    return pos

def createPosMatrix(x,y,z):
    posMatrix = (
        [x],
        [y],
        [z]
    )
    return posMatrix

class Points:
    def __init__(self, x, y, z):
        self.posMat = (
            [x],
            [y],
            [z]
        )
        self.x = x
        self.y = y
pcenter = Points(0,0,0)
p0 = Points(1,1,-1)
p1 = Points(-1,1,-1)
p2 = Points(1,-1,-1)
p3 = Points(-1,-1,-1)
p4 = Points(1,1,1)
p5 = Points(-1,1,1)
p6 = Points(1,-1,1)
p7 = Points(-1,-1,1)
scale = 100
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False


    for angle in range(0, 360, 30):
        screen.fill("purple")

        pos0 = (rotatedX(math.radians(angle),p0.posMat))
        pos1 = (rotatedX(math.radians(angle),p1.posMat))
        pos2 = (rotatedX(math.radians(angle),p2.posMat))
        pos3 = (rotatedX(math.radians(angle),p3.posMat))
        pos4 = (rotatedX(math.radians(angle),p4.posMat))
        pos5 = (rotatedX(math.radians(angle),p5.posMat))
        pos6 = (rotatedX(math.radians(angle),p6.posMat))
        pos7 = (rotatedX(math.radians(angle),p7.posMat))
        
        pos0 = (rotatedY(math.radians(angle),createPosMatrix(pos0[0],pos0[1],0)))
        pos1 = (rotatedY(math.radians(angle),createPosMatrix(pos1[0],pos1[1],0)))
        pos2 = (rotatedY(math.radians(angle),createPosMatrix(pos2[0],pos2[1],0)))
        pos3 = (rotatedY(math.radians(angle),createPosMatrix(pos3[0],pos3[1],0)))
        pos4 = (rotatedY(math.radians(angle),createPosMatrix(pos4[0],pos4[1],0)))
        pos5 = (rotatedY(math.radians(angle),createPosMatrix(pos5[0],pos5[1],0)))
        pos6 = (rotatedY(math.radians(angle),createPosMatrix(pos6[0],pos6[1],0)))
        pos7 = (rotatedY(math.radians(angle),createPosMatrix(pos7[0],pos7[1],0)))
        
        # pos0 = (rotatedZ(math.radians(angle),createPosMatrix(pos0[0],pos0[1],0)))
        # pos1 = (rotatedZ(math.radians(angle),createPosMatrix(pos1[0],pos1[1],0)))
        # pos2 = (rotatedZ(math.radians(angle),createPosMatrix(pos2[0],pos2[1],0)))
        # pos3 = (rotatedZ(math.radians(angle),createPosMatrix(pos3[0],pos3[1],0)))
        # pos4 = (rotatedZ(math.radians(angle),createPosMatrix(pos4[0],pos4[1],0)))
        # pos5 = (rotatedZ(math.radians(angle),createPosMatrix(pos5[0],pos5[1],0)))
        # pos6 = (rotatedZ(math.radians(angle),createPosMatrix(pos6[0],pos6[1],0)))
        # pos7 = (rotatedZ(math.radians(angle),createPosMatrix(pos7[0],pos7[1],0)))

        drawPoint(scale*round(pos0[0])+400,scale*round(pos0[1])+400, "black")
        drawPoint(scale*round(pos1[0])+400,scale*round(pos1[1])+400, "black")
        drawPoint(scale*round(pos2[0])+400,scale*round(pos2[1])+400, "black")
        drawPoint(scale*round(pos3[0])+400,scale*round(pos3[1])+400, "black")
        drawPoint(scale*round(pos4[0])+400,scale*round(pos4[1])+400, "black")
        drawPoint(scale*round(pos5[0])+400,scale*round(pos5[1])+400, "black")
        drawPoint(scale*round(pos6[0])+400,scale*round(pos6[1])+400, "black")
        drawPoint(scale*round(pos7[0])+400,scale*round(pos7[1])+400, "black")
        # Drawing the lines that connect the vertices
        pygame.draw.line(screen,pygame.Color("Black"),((scale*round(pos0[0])+400,scale*round(pos0[1])+400)),((scale*round(pos1[0])+400,scale*round(pos1[1])+400)),5)
        pygame.draw.line(screen,pygame.Color("Black"),((scale*round(pos2[0])+400,scale*round(pos2[1])+400)),((scale*round(pos3[0])+400,scale*round(pos3[1])+400)),5)
        pygame.draw.line(screen,pygame.Color("Black"),((scale*round(pos0[0])+400,scale*round(pos0[1])+400)),((scale*round(pos2[0])+400,scale*round(pos2[1])+400)),5)
        pygame.draw.line(screen,pygame.Color("Black"),((scale*round(pos1[0])+400,scale*round(pos1[1])+400)),((scale*round(pos3[0])+400,scale*round(pos3[1])+400)),5)
        
        pygame.draw.line(screen,pygame.Color("Black"),((scale*round(pos4[0])+400,scale*round(pos4[1])+400)),((scale*round(pos5[0])+400,scale*round(pos5[1])+400)),5)
        pygame.draw.line(screen,pygame.Color("Black"),((scale*round(pos6[0])+400,scale*round(pos6[1])+400)),((scale*round(pos7[0])+400,scale*round(pos7[1])+400)),5)
        pygame.draw.line(screen,pygame.Color("Black"),((scale*round(pos4[0])+400,scale*round(pos4[1])+400)),((scale*round(pos6[0])+400,scale*round(pos6[1])+400)),5)
        pygame.draw.line(screen,pygame.Color("Black"),((scale*round(pos5[0])+400,scale*round(pos5[1])+400)),((scale*round(pos7[0])+400,scale*round(pos7[1])+400)),5)
       
        pygame.draw.line(screen,pygame.Color("Black"),((scale*round(pos0[0])+400,scale*round(pos0[1])+400)),((scale*round(pos4[0])+400,scale*round(pos4[1])+400)),5)
        pygame.draw.line(screen,pygame.Color("Black"),((scale*round(pos1[0])+400,scale*round(pos1[1])+400)),((scale*round(pos5[0])+400,scale*round(pos5[1])+400)),5)
        pygame.draw.line(screen,pygame.Color("Black"),((scale*round(pos2[0])+400,scale*round(pos2[1])+400)),((scale*round(pos6[0])+400,scale*round(pos6[1])+400)),5)
        pygame.draw.line(screen,pygame.Color("Black"),((scale*round(pos3[0])+400,scale*round(pos3[1])+400)),((scale*round(pos7[0])+400,scale*round(pos7[1])+400)),5)

        pygame.display.flip()
        clock.tick(5)  # limits FPS to 60

        
    # # Draw center point and 4 vertices
    # drawPoint((scale*p0.x)+400,((scale*p0.y)+400), "white")
    # drawPoint((scale*p1.x)+400,((scale*p1.y)+400), "white")
    # drawPoint((scale*p2.x)+400,((scale*p2.y)+400), "white")
    # drawPoint((scale*p3.x)+400,((scale*p3.y)+400), "white")


        # Draw lines connecting vertices
        
        # pygame.draw.line(screen,pygame.Color("Black"),(scale*pos2+400),(scale*pos3+400),5)
        # pygame.draw.line(screen,pygame.Color("Black"),(scale*pos0+400),(scale*pos2+400),5)
        # pygame.draw.line(screen,pygame.Color("Black"),(scale*pos1+400),(scale*pos3+400),5)



pygame.quit()