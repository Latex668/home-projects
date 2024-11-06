import discord
from discord.ext import commands
from os import getenv
from dotenv import load_dotenv
load_dotenv()
BOT_TOKEN = getenv("BOT_TOKEN")
import requests

def lightsPWR(pwrVal,brightnessVal):
  r = requests.post("http://192.168.100.50/json/state", json={"on":pwrVal,"bri":str(brightnessVal/100*255)})
  print(f"Status Code: {r.status_code}, Response: {r.json()}")
def lightsColor(colorsHex):
  r = requests.post("http://192.168.100.50/json/state", json={"seg":[{"pal":3,"col":[[colorsHex[0],colorsHex[1],colorsHex[2]]]}]})
  print(f"Status Code: {r.status_code}, Response: {r.json()}")
def lightEffectsF(effectID):
  r = requests.post("http://192.168.100.50/json/state", json={"seg":[{"pal":3,"fx":effectID}]})
  print(f"Status Code: {r.status_code}, Response: {r.json()}")
def changeEffectSpeed(effectSpeed):
  r = requests.post("http://192.168.100.50/json/state", json={"seg":[{"v":"true","id":0,"pal":3,"sx":str(effectSpeed/100*255)}]})
  print(f"Status Code: {r.status_code}, Response: {r.json()}")
def changeEffectIntensity(effectIntensity):
  r = requests.post("http://192.168.100.50/json/state", json={"seg":[{"v":"true","id":0,"pal":3,"ix":str(effectIntensity/100*255)}]})
  print(f"Status Code: {r.status_code}, Response: {r.json()}")



intents = discord.Intents.default()
intents.message_content = True

bot = commands.Bot(command_prefix="/", intents=intents)

#commands
@bot.command(name="lights", description="Turns the lights on/off and changes brightness")
async def lights(ctx, pwrVal:bool = commands.parameter(description="accepts on/off"), brightness:float = commands.parameter(default=50, description="Sets the lights' brightness value, between 0-100")):
  lightsPWR(bool(pwrVal), brightness)
  await ctx.send(f"Lights are {bool(pwrVal)} at brightness: {brightness}%!")

@bot.command(name="color", description="Changes the lights color to any RGB value, example: 255 0 0 = RED; 255 255 255 - White")
async def color(ctx, *colorRGB:int):
    lightsColor(colorRGB)
    await ctx.send(f"Colors are set to: {colorRGB}!")

@bot.command(name="effects", description="Changes the lights effect to said ID, additionally set to r for random")
async def effects(ctx, effectID: str):
   if(effectID != "r"):
    lightEffectsF(int(effectID))
    await ctx.send(f"Effect ID is set to: {effectID}")
   else:
     lightEffectsF(effectID)
     await ctx.send("Effect ID is set to: random!")
@bot.command(name="effectSpeed", description="Changes the brightness of the effect.")
async def effectSpeed(ctx, effectSpeed:int):
  changeEffectSpeed(effectSpeed)
  await ctx.send(f"Effect speed is: {effectSpeed}%!")
@bot.command(name="effectIntensity", description="Changes the intensity of the effect.")
async def effectSpeed(ctx, effectIntensity:int):
  changeEffectIntensity(effectIntensity)
  await ctx.send(f"Effect intensity is: {effectIntensity}%!")

bot.run(BOT_TOKEN)