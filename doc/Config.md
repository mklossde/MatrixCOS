
# MatrixCOS - Config

MatrixCOS use <a href='https://github.com/mrcodetastic/ESP32-HUB75-MatrixPanel-DMA'>ESP32-HUB75-MatrixPanel-DMA</a>

## hardware config
The config requires some basic entrys
- sizeX - number of pixel in one row (0..n)
- sizeY - number of pixel in one coloumn (0..n)
- chain - chaind disdplay (1..n)
- brightness - startup brightness (0..255)
- rotation - startup rotation (0,1,2,3)
- pins  - sepaerated by semikolon (r1,g1,b1,r2,g2,b2,a,b,c,d,e,lat,oe,clk)

Config via command
	e.g. matrix 64 64 1 90 0 0,15,4,16,27,17,5,18,19,21,12,33,25,22
	
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