

#define DEG2RAD 0.0174532925



//-----------------------------------------------------------
// Panel Connetons (GPIO)

uint16_t _color;

void draw() {
  displayDraw();
}


/* clear display , stop play page */
void drawClear() {   
  if(!_displaySetup) { return ; }
  if(eeDisplay.displayBuffer) { display->fillRect(0,0,panelX,panelY,0); } else { displayClear(); }
  matrixPage=0;
} 

//------------------------------------------------------------------------------

void drawColor(uint16_t color) { _color=color; }

/* fill with color (red,green,blue=0..15) */
void fillScreen(int color) { if(!_displaySetup) { return ; } if(color==-1) { color=_color; } display->fillScreen(color);  } 

/* draw line */
void drawPixel(uint16_t x, uint16_t y, uint16_t color) { 
  if(!_displaySetup) { return ; } if(color==-1) { color=_color; } display->drawPixel(x,y, color); }  
/* draw line */
void drawLine(int x,int y, int w,int h,int color) { if(!_displaySetup) { return ; } if(color==-1) { color=_color; } display->drawLine(x,y,w,h, color); }    
/* draw rec (x,y,x+w,y+h)*/
void drawRect(int x,int y, int w,int h,int color) {if(!_displaySetup) { return ; } if(color==-1) { color=_color; } display->drawRect(x,y,w,h, color); }   
/* draw fill rec */
void fillRect(int x,int y, int w,int h,int color) { if(!_displaySetup) { return ; }if(color==-1) { color=_color; } display->fillRect(x,y,w,h, color); }  

void drawLine(int x,int y, int w,int h, int xw,int yw, int color) { 
  if(!_displaySetup) { return ; } if(color==-1) { color=_color; } 
  for(int i=0;i<xw;i++) {  for(int t=0;t<yw;t++) { display->drawLine(x+i,y+t,w+i,h+t, color); }}
}
 

// drawTriangle x y x2 y2 x3 y3 c - draw a trinagle (from x,y to x2,y2 to x3,y3 to x,y)
void drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
  if(!_displaySetup) { return ; }
  if(color==-1) { color=_color; } display->drawTriangle(x0,y0,x1,y1,x2,y2,color);
}
// fillTriangle x y x2 y2 x3 y3 c -  draw a filled triangle
void fillTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
  if(!_displaySetup) { return ; }
  if(color==-1) { color=_color; } display->fillTriangle(x0,y0,x1,y1,x2,y2,color);
}

// drawCircle x y w c - draw circle at x y with radius w 
void drawCircle(int x,int y, int w,int color) { 
  if(!_displaySetup) { return ; } if(color==-1) { color=_color; } display->drawCircle(x,y,w, color); }   
// draw filled circle at x y with radius w 
void fillCircle(int x,int y, int w,int color) { 
  if(!_displaySetup) { return ; } if(color==-1) { color=_color; } display->fillCircle(x,y,w, color); }  



void drawRoundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t radius, uint16_t color) { 
  if(!_displaySetup) { return ; }
  if(color==-1) { color=_color; } display->drawRoundRect(x0,y0,w,h,radius,color); }
void fillRoundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t radius, uint16_t color) { 
  if(!_displaySetup) { return ; }
  if(color==-1) { color=_color; } display->fillRoundRect(x0,y0,w,h,radius,color);  }

void drawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t size) { 
  if(!_displaySetup) { return ; }
  if(color<0) { color=_color; } display->drawChar(x,y,c,color,bg,size); }
void drawText(int x,int y, int size, const char *str,int color) {
  if(!_displaySetup) { return ; }
  if(color<0) { color=_color; }
  display->setTextSize(size);     // size 1 == 8 pixels high
  display->setTextWrap(true); // Don't wrap at end of line - will do ourselves
  display->setCursor(x,y);    // start at top left, with 8 pixel of spacing
  display->setTextColor(color); // set color
  display->print(str);  
}

void getTextBounds(const char *text,uint16_t *w,uint16_t *h) {
  int16_t  x1, y1; 
  display->getTextBounds(text, 0, 0, &x1, &y1, w, h);
}

void drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {
  if(!_displaySetup) { return ; }
  if(color<0) { color=_color; } display->drawBitmap(x,y,bitmap,w,h,color);
}


/* draw 1-bit icon 
 *    convert tool https://javl.github.io/image2cpp/
void matrixHub75_icon(int x, int y, int color,char *xbm,int size) {
  if(xbm==NULL || size<3) { return ; } // empty
  int index=0;
  int colors=pgm_read_byte(xbm+index++); // firet color-deep/type
  int width=pgm_read_byte(xbm+index++);  // width in pixel
  int height=pgm_read_byte(xbm+index++); // height in pixel
  int max=((width/8)*height)+index;
//sprintf(buffer,"c:%d w:%d h:%d s:%d max:%d",colors,width,height,size,max);logPrintln(LOG_INFO,buffer);
  if(max>size-index) { max=size-index; } // wrong icon size   
  if(color==-1) { color=_color; }

  if (width % 8 != 0) { width =  ((width / 8) + 1) * 8; }
  int i=0;
  while(i++<max) {
    unsigned char charColumn = pgm_read_byte(xbm + i+ index);
    for (int j = 0; j < 8; j++) {
      int targetX = (i * 8 + j) % width + x;
      int targetY = (8 * i / (width)) + y;
      if (bitRead(charColumn, j)) { drawPixel(targetX, targetY, color);}
    }
  }  
}
*/

/* draw 1-bit icon 
 *    convert tool https://javl.github.io/image2cpp/
*/
void drawIcon(int x, int y, int w,int h,int color,char *xbm,int size) {
  if(!_displaySetup) { return ; }
  if(xbm==NULL || size<3) { return ; } // empty
  int index=0;
  int type=pgm_read_byte(xbm+index++); // type
  int width=pgm_read_byte(xbm+index++);  // width in pixel
  int height=pgm_read_byte(xbm+index++); // height in pixel
  if(type!=1 || width<1 || height<1) { return ; } // wonrg
  int max=((width/8)*height)+index;
  
  sprintf(buffer,"icon draw size:%d type:%d (%d x %d) to (%d x %d)", size,type,w, h,width,height);logPrintln(LOG_DEBUG,buffer);
//sprintf(buffer,"c:%d w:%d h:%d s:%d max:%d",colors,width,height,size,max);logPrintln(LOG_INFO,buffer);
//  if(max>size-index) { max=size-index; } // correct size   
  if(color==-1) { color=_color; }
  if(w<=0) { w=width; }
  if(h<=0) { h=height; }

  float wx=(float)width/(float)w; float wy=(float)height/(float)h; // pixel size depening on w,h
  for(int targetX=0;targetX<w;targetX++) {
    for(int targetY=0;targetY<h;targetY++) {      
      int tx=((float)targetX*wx)/8;
      int pos=tx+((int)((float)targetY*wy)*(width/8));
      int bit=((float)targetX*wx)-(tx*8);
      unsigned char charColumn = 0;
      if(index+pos<size) { charColumn = pgm_read_byte(xbm + index+ pos); }
      if (bitRead(charColumn, bit)) { display->drawPixel(targetX+x, targetY+y, color);}
    }
  }


/*  
  if (width % 8 != 0) { width =  ((width / 8) + 1) * 8; }
  int i=0;
  while(i++<max) {
    unsigned char charColumn = pgm_read_byte(xbm + i+ index);
    for (int j = 0; j < 8; j++) {
      int targetX = (i * 8 + j) % width + x;
      int targetY = (8 * i / (width)) + y;
      if (bitRead(charColumn, j)) { display->drawPixel(targetX, targetY, color);}
    }
  }
  */
  
}

//-------------------------------------------------------------------

//byte _segmentStep=6; // Segments are 3 degrees wide = 120 segments for 360 degrees
//byte _segmentInc=6; // Draw segments every 3 degrees, increase to 6 for segmented ring

/* set _segmentStep and _segmentInc */
//void drawSegment(int segmentStep, int segmentInc) { _segmentStep=segmentStep; _segmentInc=segmentInc; }

// Draw an segment-arc with a defined thickness
// x,y == coords of centre of arc
// segmentStep (1=360, 3=120, 6=60)
// start_segemnt = 0 - n (e.g. segmentSep 6, start_segemnt=15 => 90 grad)
// seg_count = number of segments to draw (segmentStep=3, seg_count=120 => halb circle)
// rx = x axis radius, yx = y axis radius
// w  = width (thickness) of arc in pixels
void drawArc(int x, int y, int segmentStep, int seg_start, int seg_count, int rx, int ry, int w, int color) {
  int segmentInc=segmentStep;
  int start_angle=seg_start*segmentInc;
  if(!_displaySetup) { return ; }
  if(color==-1) { color=_color; }  
  float sx = cos((start_angle - 90) * DEG2RAD);
  float sy = sin((start_angle - 90) * DEG2RAD);
  uint16_t x0 = sx * (rx - w) + x;
  uint16_t y0 = sy * (ry - w) + y;
  uint16_t x1 = sx * rx + x;
  uint16_t y1 = sy * ry + y;

  for (int i = start_angle; i < start_angle + segmentStep * seg_count; i += segmentInc) {
    float sx2 = cos((i + segmentStep - 90) * DEG2RAD);
    float sy2 = sin((i + segmentStep - 90) * DEG2RAD);
    int x2 = sx2 * (rx - w) + x;
    int y2 = sy2 * (ry - w) + y;
    int x3 = sx2 * rx + x;
    int y3 = sy2 * ry + y;
    display->fillTriangle(x0, y0, x1, y1, x2, y2, color);
    display->fillTriangle(x1, y1, x2, y2, x3, y3, color);
    x0 = x2;
    y0 = y2;
    x1 = x3;
    y1 = y3;  
  }
}

/* draw full-bar of value% of max% (e.g. 5 10 => half) e.g. drawFull 10 40 5 20 1 10 50 888 2016 */
void drawFull(int x,int y,int w,int h,int p,int value,int max,int col1,int col2) {
  if(col1==-1) { col1=_color; }  if(col2==-1) { col2=_color; }  
  drawRect(x,y,w,h,col1);  
  if(w>h) { 
    float step=(float)w/(float)max; int w2=(int)((float)value*step);
    if(w2>p*2) {fillRect(x+p,y+p,w2-(p*2),h-(p*2),col2); }
  } else { 
    float step=(float)h/(float)max; int w2=(int)((float)value*step);
    if(w2>p*2) { fillRect(x+p,y+p+h-w2, w-(p*2),w2-(p*2),col2); }
  } 
}

/* drawGauge 10 10 5 1 5 10 888 2016 */
void drawGauge(int x,int y,int w,int p,int value,int max,int col1,int col2) {
  if(col1==-1) { col1=_color; } 
  drawArc(x,y, 1, 270, 180, w,w, 0, col1);
  drawLine(x-w,y,x+w,y,col1);
  int v=(180/max)*value;
  drawArc(x,y,1,270+v,2,w-p,w-p,w,col2);
}
/* draw on button e.g. drawOn 10 10 5 2 1 -1 2016 */
void drawOn(int x,int y,int w,int p,boolean on,int col1,int col2) {
  drawCircle(x,y,w,col1);
  if(on) { fillCircle(x,y,w-p,col2); }
}

//-------------------------------------------

/* draw value with name+value */
void drawValue(int x,int y,char *text,int value,int max,int col1,int col2) {
  drawText(x,y,1,text,_color);
  drawText(x,y+8,1,to(value),col1);
  drawLine(x,y+16,x+63,y+16,_color);
}

/* draw name+value+full */
void valueFull(int x,int y,char *text,int value,int max,int col1,int col2) {
  drawValue(x,y,text,value,max,col1,col2);
  if(value<0) { col1=col2; value=value*-1; } 
  drawFull(x+40,y,20,8,1,value,max,_color,col1);
}

/* draw name+value+on (on=vlaue>max) */
void valueOn(int x,int y,char *text,int value,int max,int col1,int col2) {
  drawValue(x,y,text,value,max,col1,col2);
  if(value>max) { col1=col2; }
  drawOn(x+53,y+7,5,1,true,_color,col1);
}

/* draw name+value+guage */
void valueGauge(int x,int y,char *text,int value,int max,int col1,int col2) {
  drawValue(x,y,text,value,max,col1,col2);
  if(value>max) { col1=col2; }
  drawGauge(x+50,y+10,10,1,value,max,_color,col1);
}

//-------------------------------------------------------------------

/* draw net-time at x,y with color */
//TODO do not work
void drawTime(int x,int y,int color) {  
  drawText(x,y,1,getTime(),color);
}

/* draw net-date at x,y with color */
//TODO do not work
void drawDate(int x,int y,int color) {  
  drawText(x,y,1,getDate(),color);
}



