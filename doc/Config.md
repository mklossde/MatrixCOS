
# MatrixCOS - Config


MatrixCOS based on <a href='https://eely.eu/'>HoDi</a> (<a href='https://github.com/JPlenert/eely-hodi'>eely-HoDi on github</a>)
and use Lib <a href='https://github.com/mrcodetastic/ESP32-HUB75-MatrixPanel-DMA'>ESP32-HUB75-MatrixPanel-DMA</a>
which based on <a href='https://github.com/adafruit/Adafruit-GFX-Library'>adafruit-gfx-library</a>

## Hardware

- ESP32-DevKitC-Board (38 Pins) (e.g. 2,29 Euro at https://de.aliexpress.com/item/1005006900641832.html)
- HUB75 LED-Matrix mit 64 × 64 (e.g.  10,79 Euro at https://de.aliexpress.com/item/32959774837.html)
- <a hrefg='https://github.com/JPlenert/eely-hodi/tree/main/eely-hodi-pcb/HoDi11P4_V.08.zip'>HoDi-Platine</a> (0,90 Euro orderd at via <a href='https://jlcpcb.com/'>jlcpcb.com</a>)

I build 5 items with 13,98 Euro each :-)

## 3Dprint 

See <a href='../3d/rdprint.md>3D Print Modell</a>

## Installtion

Easy was to install the bin via <a href='https://web.esphome.io/'>esp-web-tools</a>
- Connect to ESP
- Install (seclect <a href='../bin/'>MatrixCOS firmware</a> from bin directory)

## CmdOS Config 

Config via Web Serial Terminal (e.g. <a href='https://serial.huhn.me/'>serial.huhn.me</a>)

with the commamd "setup"

	setup wifi_ssid wifi_pas espName espPas mqttServer
	
	wifi_ssid = your local wifi 
	wifi_pas = your local wifi password 	
    espName = a uniqe good name for the esp (not required)
    espPas	= to secure the esp (not required)
	mqttServer = to use mqtt (e.g. mqtt://user:pas@mqttserver.local:1833) (not required)
	
	
## HUB75 config
The config requires some basic entrys
- sizeX - number of pixel in one row (0..n)
- sizeY - number of pixel in one coloumn (0..n)
- chain - chaind disdplay (1..n)
- brightness - startup brightness (0..255)
- rotation - startup rotation (0,1,2,3)
- pins  - sepaerated by semikolon (r1,g1,b1,r2,g2,b2,a,b,c,d,e,lat,oe,clk)

Config via matrix command (for the ESP32-DevKitC-Board with HoDi-Platine)

	e.g. matrix 64 64 1 90 0 0,15,4,16,27,17,5,18,19,21,12,33,25,22
		matrix2 0 1 1 0
		
Setup extended via matrix2
- DmaBuffer
- ImageBuffer 	
- 
	matrix2 0 1 1 0
	
Config via web with 
	http://ESPNAME.local/appSetup
	
DO not forget to save the changes. 
The new config will be used after restart. 

	
## buffer config
MatrixCOS use two buffers, which are enabeld via "buffer"

- dmaBuffer - the drawing is bufferd to enhanced display quality 
and the dmsBuffer is switched via "draw".

- displayBuffer - all drawing are on a readable buffer to add on copy and effects 
the buffe is copied to dislpay with "draw"

 
![LOGO](../images/Hub75_logo_32x32.gif) a OpenOn.org project - develop by mk@almi.de 