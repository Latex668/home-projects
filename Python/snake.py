import pygame
from pygame import locals
import random

pygame.init()
SC_W = 800
SC_H = 800
SC = pygame.display.set_mode((SC_W, SC_H))

pygame.display.set_caption("SNAKE")
clock = pygame.time.Clock()
running = True

gridSize = 80

def drawGrid():
    for x in range(0,SC_W, gridSize):
        for y in range(0,SC_H, gridSize):
            rect = pygame.Rect(x,y,gridSize,gridSize)
            pygame.draw.rect(SC,'#0a0a0a',rect,1)

class Snake:
    def __init__(self):
        self.x,self.y = gridSize, gridSize
        self.xdir = 1
        self.ydir = 0
        self.head = pygame.Rect(self.x, self.y, gridSize, gridSize)
        self.body = [pygame.Rect(self.x-gridSize, self.y, gridSize, gridSize)]
        self.dead = False
    def update(self):
        global apple
        for square in self.body:
            if self.head.x == square.x and self.head.y == square.y:
                self.dead = True
            if self.head.x not in range(0,SC_W) or self.head.y not in range(0,SC_H):
                self.dead = True

        if self.dead:
            self.x, self.y = gridSize, gridSize
            self.head = pygame.Rect(self.x, self.y, gridSize, gridSize)
            self.body = [pygame.Rect(self.x-gridSize, self.y, gridSize, gridSize)]
            self.xdir = 1
            self.ydir = 0
            self.dead = False
            for i in range(len(apple)):
                apple[i] = Apple()

        self.body.append(self.head)
        for i in range(len(self.body)-1):
            self.body[i].x = self.body[i+1].x
            self.body[i].y = self.body[i+1].y
        self.head.x += self.xdir*gridSize
        self.head.y += self.ydir*gridSize
        self.body.remove(self.head)

class Apple:
    def __init__(self):
        self.x = int(random.randint(0,SC_W)/gridSize)*gridSize
        self.y = int(random.randint(0,SC_H)/gridSize)*gridSize
        self.rect = pygame.Rect(self.x, self.y, gridSize, gridSize)

    def update(self):
        pygame.draw.rect(SC,"red",self.rect)
snake = Snake()
apple = [Apple(),Apple(),Apple()]


font = pygame.font.SysFont('Impact',24)
score = font.render("1",True,"GREEN")
score_rect =  score.get_rect(topleft=(10,20))

while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
            pygame.quit()
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_DOWN:
                snake.ydir = 1
                snake.xdir = 0
            elif event.key == pygame.K_UP:
                snake.ydir = -1
                snake.xdir = 0
            elif event.key == pygame.K_LEFT:
                snake.ydir = 0
                snake.xdir = -1
            elif event.key == pygame.K_RIGHT:
                snake.ydir = 0
                snake.xdir = 1


    SC.fill('#0a0a0a')
    drawGrid()

    snake.update()
    for i in range(len(apple)):
        apple[i].update()

    score = font.render(f"Score: {len(snake.body)*10}", True, "green")
    SC.blit(score,score_rect)

    pygame.draw.rect(SC,"green",snake.head)

    for square in snake.body:
        pygame.draw.rect(SC,"green",square)
    for i in range(len(apple)):
        if snake.head.x == apple[i].x and snake.head.y == apple[i].y:
            snake.body.append(pygame.Rect(square.x, square.y, gridSize, gridSize))
            apple[i] = Apple()
        

    pygame.display.flip()
    clock.tick(5)

pygame.quit