import time
import adafruit_dht
import board
import sys

dht = adafruit_dht.DHT22(board.D15)

try:
	nhiet_do = format(dht.temperature, ".1f")
	print(nhiet_do)
except RuntimeError as error:
	print (100)
		
dht.exit()


