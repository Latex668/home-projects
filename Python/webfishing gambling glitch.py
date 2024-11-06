# Webfishing gambling input glitch
import time as t
from pynput.keyboard import Controller, Key, Listener

kb = Controller()
def doTheThing():
  print("Doing the thing")
  x = 0
  while(kb.pressed(Key.ctrl) != True ):
    x += 1
    print(f"Number of passes: {x}")
    kb.press(Key.shift)
    kb.press(Key.space)
    t.sleep(0.1)
    kb.release(Key.shift)
    kb.press("1")
    kb.release(Key.space)
    kb.release("1")
    kb.press("2")
    kb.release("2")
if kb.pressed("alt") == True:
  doTheThing()
  print("Press registered")

