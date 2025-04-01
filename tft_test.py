import time
import busio
import digitalio
import subprocess
import displayio
import sys
from PIL import Image, ImageDraw, ImageFont
from adafruit_display_text import label
from board import SCK, MOSI, MISO, CE0, D24, D27

from adafruit_rgb_display import color565
import adafruit_rgb_display.ili9341 as ili9341


# Configuration for CS and DC pins:
CS_PIN = CE0
DC_PIN = D24
RESET_PIN = D27

# Setup SPI bus using hardware SPI:
spi = busio.SPI(clock=SCK, MOSI=MOSI, MISO=MISO)

# Create the ILI9341 display:
display = ili9341.ILI9341(spi, cs=digitalio.DigitalInOut(CS_PIN),
                            dc=digitalio.DigitalInOut(DC_PIN),
                            rst=digitalio.DigitalInOut(RESET_PIN))
image = Image.new("RGB", (display.width, display.height))

# Get drawing object to draw on image.
draw = ImageDraw.Draw(image)

temp = sys.argv[1]
font = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 80)
draw.text((25, 100), temp, font=font, fill="#00FF00")
display.image(image)
