
# MatrixCOS - Commands

- matrix sizeX sizeY chain brightness rotation pins - config hub75 dislpay/connection
	e.g. matrix 64 64 1 90 0 0,15,4,16,27,17,5,18,19,21,12,33,25,22
- matrix2 dmaBuffer displayBuffer latBlanking clkphase 
	(0=off/1=on) enable dmsBuffer or displayBuffer 

 ## draw control
- drawOff => switch display off by clear and stop all
- drawClear => clear display
- drawStop => stop all (not clear)
- draw => draw buffer, draw content from dma/local buffer on display

- brightness n - set up brightness of dislpay

## draw color
- drawColor r g b - calculate 444 color and set as default color    
    default color for all draw-commands (use by draw command if no or -1 is given as color)
- drawColor565 r g b - calculate 565 color and set as default color 

## draw 
- fillScreen c - fill screen with color	
- drawPixel x y c - draw a pixel at x y
- drawLine x y x2 y2 c - draw a line (from x,y to x2,y2)
- drawRect x y w h c - draw rect (rect from x,y to x+w,y+h)
- fillRect x y w h c - draw a filled rect
- drawTriangle x y x2 y2 x3 y3 c - draw a trinagle (from x,y to x2,y2 to x3,y3 to x,y)
- fillTriangle x y x2 y2 x3 y3 c -  draw a filled triangle

- drawText x y c size text - draw text at x y with size 

- drawCircle x y w c - draw circle at x y with radius w 
- fillCircle x y w c - draw filled circle at x y with radius w 
- drawArc x y seg seg-start seg rx ry w c - draw a segment arc at x,y start at angel (0=top), end after seg, with radiusX and radiusY and thikness w 

## draw  

- drawFull x y w h p value max, c1,c2 - draw a full-element at x,y with w,h. full=100/max*value will be in color c2 and offset p
- drawOn  x y r p on col1 col2 e.g. drawOn 10 10 5 2 1 -1 2016
- drawGauge x y w p value max col1 col2 e.g.  drawGauge 10 10 5 1 5 10 888 2016

## values

-valueFull x y name value max col1 col2 - e.g.  valueFull 1 12 "Power0" $shellies/shellyem3/emeter/0/power 1000 888 444
-valueOn x y name value max col1 col2 e.g. valueOn 1 30 "Power1" $shellies/shellyem3/emeter/1/power 1000 2016 444
-valueGauge x y name value max col1 col2 e.g. valueGauge 1 47 "Power2" $shellies/shellyem3/emeter/2/power 100 2016 444  
  
## page 

- pageTest - draw test page - simple dislpay test
- pageTitle - draw title page - matrixCOS title 
- pageEsp - draw esp page - show esp informations 

## image 
- drawIcon x y w h c file - draw icon (bm1) at x,y with w,h of color 
- drawFile file type x y - draw a gif/icon at x,y
- drawUrl url x y - draw content of url (gif/icon) at a x 

## formated 
- drawTime x y c - draw time (format hh:mm:ss) at x,y of color c
- drawDate x y c - draw date (format dd.mm.yyyy) at x,y of color c

## effect 
 
- effect type step speed a b - start effect type with n-steps with speed in ms between steps
 
![LOGO](images/Hub75_logo_32x32.gif) a OpenOn.org project - develop by mk@almi.de 