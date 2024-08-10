import pygame
from random import randrange

pygame.init()
SC_WIDTH = 1280
SC_HEIGHT = 840
screen = pygame.display.set_mode((SC_WIDTH, SC_HEIGHT))
running = True
pygame.font.init()

global lastPlaced


isOccupied =   ['','','', # 3x3 list to store if any of the grids are occupied and by which term
                '','','', 
                '','','']
lastPlaced = ''; # stores the last placed element
won = False


while running:
    screen.fill((10,10,10)) 
    mouse_pos = pygame.mouse.get_pos()

    def AI(lastPlaced):
        if lastPlaced == 'X':
            num = randrange(0,8)
            if isOccupied[num] != '':
                num = randrange(8)
            elif (isOccupied[num] == '') and (won == False):
                isOccupied[num] = 'O'
                lastPlaced = 'O'
            return lastPlaced
    lastPlaced = AI(lastPlaced)

    for event in pygame.event.get(): # For any event that happens in game, check any of the possibilities
        if event.type == pygame.QUIT:
            running = False
        if event.type == pygame.MOUSEBUTTONUP:
            for i in range(9):
                if Grid_coords[i].collidepoint(mouse_pos): # If the grid coords collide with the mouse click, check which item was placed last and if the grid is occupied
                    if (lastPlaced != 'X') and isOccupied[i] == '' and won != True:
                        isOccupied[i] = 'X'
                        lastPlaced = 'X'
                    # if (lastPlaced != 'O') and isOccupied[i] == '' and won != True:
                    #     isOccupied[i] = 'O'
                    #     lastPlaced = 'O'

    # def isDraw(list, combinations):
    #     for i in range(9):
    #         if list[i] != "":
    #             if i == 8:
    #                 for x in range(8):
    #                     if(combinations[x] != True):
    #                         if x == 7:
    #                             return True
    
        
    def isWon(isOccupied):
            global won
            combinations =     [isOccupied[0] == isOccupied[1] == isOccupied[2] !='', # Stored all the possible combinations to win this thing
                                isOccupied[6] == isOccupied[7] == isOccupied[8] !='', 
                                isOccupied[0] == isOccupied[3] == isOccupied[6] !='',
                                isOccupied[1] == isOccupied[4] == isOccupied[7] !='',
                                isOccupied[2] == isOccupied[5] == isOccupied[8] !='',
                                isOccupied[3] == isOccupied[4] == isOccupied[5] !='', # If any of these are equal to 1, then X won, if 0 then O
                                isOccupied[0] == isOccupied[4] == isOccupied[8] !='',
                                isOccupied[2] == isOccupied[4] == isOccupied[6] !='']
            for i in range(8): # Checks all possible combinations, this was shit to debug when it didnt work.
                if combinations[i]:
                    drawTXT(str(lastPlaced) + " WON!", 'monospace', 200, "Green", SC_WIDTH/4, 3*SC_HEIGHT/8)
                    won = True
                # elif combinations[i] == False and isDraw(isOccupied, combinations) == True:
                #     drawTXT("IT'S A TIE!", 'monospace', 100, "Green", SC_WIDTH/4, 3*SC_HEIGHT/8)
                #     won = True
            return won 
            

    def drawTXT(txt, fontname, fontSize, txtColor, x, y): # function to render text to screen, starts by taking the font itself and rendering it, then displaying
        font = pygame.font.SysFont(fontname, fontSize)
        img = font.render(txt, True, txtColor)
        screen.blit(img, (x, y))

    for z in range(9): # This basically checks if any of the grids are occupied or if the game is won, so you can't place over things
        if isOccupied[z] == 'X' and won == False:
            x,y = Grid_coords[z].center
            drawTXT("X", 'monospace', 180, "Pink",x-80,y-120)
        elif isOccupied[z] == 'O' and won == False:
            x,y = Grid_coords[z].center
            drawTXT("O", 'monospace', 180, "Pink",x-80,y-120)

    Game_Border = pygame.draw.rect(screen, "pink",((0,0),(SC_WIDTH,SC_HEIGHT)),10)
    Grid = [pygame.draw.rect(screen, "pink",((0,0),(SC_WIDTH,SC_HEIGHT-SC_HEIGHT/3)),10),pygame.draw.rect(screen, "pink",((0,0),(SC_WIDTH,SC_HEIGHT/3)),10),pygame.draw.rect(screen, "pink",((0,0),(SC_WIDTH-SC_WIDTH/3,SC_HEIGHT)),10), pygame.draw.rect(screen, "pink",((0,0),(SC_WIDTH/3,SC_HEIGHT)),10)]
    # This long ass list just stored the coordonates to every single grid, couldn't find a better way at the time.
    Grid_coords =  [pygame.Rect((0,0),(SC_WIDTH-SC_WIDTH*2/3,SC_HEIGHT-SC_HEIGHT*2/3)),pygame.Rect((SC_WIDTH-SC_WIDTH*2/3,0),(SC_WIDTH-SC_WIDTH*2/3,SC_HEIGHT-SC_HEIGHT*2/3)), pygame.Rect((2*(SC_WIDTH-SC_WIDTH*2/3),0),(SC_WIDTH-SC_WIDTH*2/3,SC_HEIGHT-SC_HEIGHT*2/3)),
                    pygame.Rect((0,SC_HEIGHT-SC_HEIGHT*2/3),(SC_WIDTH-SC_WIDTH*2/3,SC_HEIGHT-SC_HEIGHT*2/3)),pygame.Rect((SC_WIDTH-SC_WIDTH*2/3,SC_HEIGHT-SC_HEIGHT*2/3),(SC_WIDTH-SC_WIDTH*2/3,SC_HEIGHT-SC_HEIGHT*2/3)), pygame.Rect((2*(SC_WIDTH-SC_WIDTH*2/3),SC_HEIGHT-SC_HEIGHT*2/3),(SC_WIDTH-SC_WIDTH*2/3,SC_HEIGHT-SC_HEIGHT*2/3)),
                    pygame.Rect((0,2*(SC_HEIGHT-SC_HEIGHT*2/3)),(SC_WIDTH-SC_WIDTH*2/3,SC_HEIGHT-SC_HEIGHT*2/3)),pygame.Rect((SC_WIDTH-SC_WIDTH*2/3,2*(SC_HEIGHT-SC_HEIGHT*2/3)),(SC_WIDTH-SC_WIDTH*2/3,SC_HEIGHT-SC_HEIGHT*2/3)), pygame.Rect((2*(SC_WIDTH-SC_WIDTH*2/3),2*(SC_HEIGHT-SC_HEIGHT*2/3)),(SC_WIDTH-SC_WIDTH*2/3,SC_HEIGHT-SC_HEIGHT*2/3))]
    won = isWon(isOccupied); # Send which areas are occupied to the isWon function.

    pygame.display.flip()


pygame.quit