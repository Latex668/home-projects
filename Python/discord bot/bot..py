import discord
from discord.ext import commands
from os import getenv
from dotenv import load_dotenv
load_dotenv()
BOT_TOKEN = getenv("BOT_TOKEN")
import requests
import matplotlib.colors as mcolors

def lightsPWR(pwrVal,brightnessVal):
  r = requests.post("http://192.168.100.50/json/state", json={"on":pwrVal,"bri":int(brightnessVal/100*255)})
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

#dictionary for light effects
effectDict = {
"solid":0,
"blink":1,
"breathe":2,
"wipe":3,
"wiperandom":4,
"randomcolors":5,
"sweep":6,
"dynamic":7,
"colorloop":8,
"rainbow":9,
"scan":10,
"dualscan":11,
"fade":12,
"theater":13,
"theaterrainbow":14,
"running":15,
"saw":16,
"twinkle":17,
"dissolve":18,
"dissolvernd":19,
"sparkle":20,
"sparkledark":21,
"sparkle+":22,
"strobe":23,
"stroberainbow":24,
"strobemega":25,
"blinkrainbow":26,
"android":27,
"chase":28,
"chaserandom":29,
"chaserainbow":30,
"chaseflash":31,
"chaseflashrnd":32,
"rainbowrunner":33,
"colorful":34,
"trafficlight":35,
"sweeprandom":36,
"running2":37,
"aurora":38,
"stream":39,
"scanner":40,
"lighthouse":41,
"fireworks":42,
"rain":43,
"merrychristmas":44,
"fireflicker":45,
"gradient":46,
"loading":47,
"police":48,
"policeall":49,
"twodots":50,
"twoareas":51,
"circus":52,
"halloween":53,
"trichase":54,
"triwipe":55,
"trifade":56,
"lightning":57,
"icu":58,
"multicomet":59,
"scannerdual":60,
"stream2":61,
"oscillate":62,
"pride2015":63,
"juggle":64,
"palette":65,
"fire2012":66,
"colorwaves":67,
"bpm":68,
"fillnoise":69,
"noise1":70,
"noise2":71,
"noise3":72,
"noise4":73,
"colortwinkles":74,
"lake":75,
"meteor":76,
"meteorsmooth":77,
"railway":78,
"ripple":79,
"twinklefox":80,
"twinklecat":81,
"halloweeneyes":82,
"solidpattern":83,
"solidpatterntri":84,
"spots":85,
"spotsfade":86,
"glitter":87,
"candle":88,
"fireworksstarburst":89,
"fireworks1d":90,
"bouncingballs":91,
"sinelon":92,
"sinelondual":93,
"sinelonrainbow":94,
"popcorn":95,
"drip":96,
"plasma":97,
"percent":98,
"ripplerainbow":99,
"heartbeat":100,
"pacifica":101,
"candlemulti":102,
"solidglitter":103,
"sunrise":104,
"phased":105,
"twinkleup":106,
"noisepal":107,
"sine":108,
"phasednoise":109,
"flow":110,
"chunchun":111,
"dancingshadows":112,
"washingmachine":113,
"candycane":114,
"blends":115,
"tvsimulator":116,
"dynamicsmooth":117,
}

#commands
@bot.command(name="lights", description="Turns the lights on/off and changes brightness")
async def lights(ctx, pwrVal:bool = commands.parameter(default=True, description="accepts on/off"), brightness:float = commands.parameter(default=50, description="Sets the lights' brightness value, between 0-100")):
  if((brightness > 100) or (brightness < 0)):
    await ctx.send("Please input a value between 0-100")
  else:
    lightsPWR(bool(pwrVal), brightness)
    whatToReturn = lambda arg: "on" if(bool(arg)==True) else "off"
    await ctx.send(f"Lights are {whatToReturn(pwrVal)} at brightness: {brightness}%!")

@bot.command(name="color", description="Changes the lights color")
async def color(ctx, colorSTR:str):
    colorRGB = []
    for color in mcolors.to_rgb(colorSTR):
      colorRGB.append(color*255)
    await ctx.send(f"Color is set to {colorSTR}!")
    lightsColor(colorRGB)


@bot.command(name="effect", description="Changes the lights effect to said ID, additionally set to r for random")
async def effect(ctx, effectID: str):
   if(effectID != "r"):
    lightEffectsF(effectDict[effectID])
    await ctx.send(f"Effect ID is set to: {effectID}")
   else:
     lightEffectsF("r")
     await ctx.send("Effect ID is set to: random!")
@bot.command(name="effectSpeed", description="Changes the brightness of the effect.")
async def effectSpeed(ctx, effectSpeed:int):
  if(effectSpeed > 100  or effectSpeed < 0 ):
    await ctx.send("Please input a value between 0-100")
  else:
    changeEffectSpeed(effectSpeed)
    await ctx.send(f"Effect speed is: {effectSpeed}%!")
@bot.command(name="effectIntensity", description="Changes the intensity of the effect.")
async def effectIntensity(ctx, effectIntensity:int):
  if(effectIntensity > 100  or effectIntensity < 0):
    await ctx.send("Please input a value between 0-100")
  else:
    changeEffectIntensity(effectIntensity)
    await ctx.send(f"Effect intensity is: {effectIntensity}%!")
@bot.command(name="helpEffects", description="Sends a dm with all effect names and a link to see what they look like")
async def helpEffects(ctx):
  await ctx.message.author.send(f"Effect id's are: \n {list(effectDict.keys())} \n https://github.com/Aircoookie/WLED/wiki/List-of-effects-and-palettes")
@bot.command(name="giveburger", description="Sends bismarck a burger :D")
async def giveburger(ctx):
  await ctx.send("<@600023802809679894> https://www.reddit.com/media?url=https%3A%2F%2Fpreview.redd.it%2Fa-giant-enormous-hamburger-dripping-with-cheese-on-a-v0-betcdk4ia54a1.png%3Fauto%3Dwebp%26s%3De945e87b35c82938ff6fe9f1ac0c0c6f25418d6e")
@bot.event
async def on_command_error(ctx, error):
  await ctx.send(f"An error has occured: {str(error)}!")


bot.run(BOT_TOKEN)