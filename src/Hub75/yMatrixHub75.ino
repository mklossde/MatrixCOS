
// on a 64x64 LED matrix with HUB75 Connector
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

#define DEG2RAD 0.0174532925

MatrixPanel_I2S_DMA *dma_display = nullptr;
Adafruit_GFX *display=NULL;

boolean _matrixSetup=false; // matrix setup ok


byte _effectType=0;

//-----------------------------------------------------------
// Panel Connetons (GPIO)

byte matrixPage=0; // 0=off,1=title
unsigned long *matrixPageTime = new unsigned long(0);

// Matrix store structur
typedef struct {
  int panelX=64;
  int panelY=64;
  byte panelChain=1;
  char pins[64]="0,15,4,16,27,17,5,18,19,21,12,33,25,22";
  byte brightness=90;
  byte rotation=0;
  boolean dmaBuffer=false;
  boolean displayBuffer=true;
  byte latBlanking=1;
  boolean clkphase=true;  
  char* driver=NULL;
} eeMatrix_t;
eeMatrix_t eeMatrix;    // matrix store object 

int panelX=64;
int panelY=64;

HUB75_I2S_CFG::i2s_pins _pins;

uint16_t _color;


//-----------------------------------------------------------

/** draw actual screen => flip to back buffer **/
void draw() {
  if(!_matrixSetup) { return ; }
  if(eeMatrix.dmaBuffer) { dma_display->flipDMABuffer(); }// Show the back buffer, set currently output buffer to the back (i.e. no longer being sent to LED panels)
  else if(eeMatrix.displayBuffer) {
    GFXcanvas16 *canvas=(GFXcanvas16*)display;  
    uint16_t *buffer=canvas->getBuffer();
    dma_display->drawRGBBitmap(0,0,buffer,eeMatrix.panelX,eeMatrix.panelY);
  }
}

/* clear from page */
void pageClear() {
  if(eeMatrix.displayBuffer) { display->fillRect(0,0,eeMatrix.panelX,eeMatrix.panelY,0); } 
  else { dma_display->clearScreen(); }
  _effectType=0; // effect off
}


/* clear display , stop play page */
void drawClear() {   
  if(!_matrixSetup) { return ; }
  if(eeMatrix.displayBuffer) { display->fillRect(0,0,eeMatrix.panelX,eeMatrix.panelY,0); } else { dma_display->clearScreen(); }
  matrixPage=0;
} 

/* color color */
uint16_t toColor444(int r,int g,int b) { if(!_matrixSetup) { return -1; } return dma_display->color444(r, g, b); }
uint16_t toColor565(int r,int g,int b) { if(!_matrixSetup) { return -1; } return dma_display->color565(r, g, b); }

/* set Brightness 0-255 **/
void matrixBrightness(int b) {  if(!_matrixSetup) { return ; } dma_display->setBrightness8(b); } 

//------------------------------------------------------------------------------

void drawColor(uint16_t color) { _color=color; }

/* fill with color (red,green,blue=0..15) */
void fillScreen(int color) { if(!_matrixSetup) { return ; } if(color==-1) { color=_color; } display->fillScreen(color);  } 

/* draw line */
void drawPixel(uint16_t x, uint16_t y, uint16_t color) { 
  if(!_matrixSetup) { return ; } if(color==-1) { color=_color; } display->drawPixel(x,y, color); }  
/* draw line */
void drawLine(int x,int y, int w,int h,int color) { if(!_matrixSetup) { return ; } if(color==-1) { color=_color; } display->drawLine(x,y,w,h, color); }    
/* draw rec */
void drawRect(int x,int y, int w,int h,int color) {if(!_matrixSetup) { return ; } if(color==-1) { color=_color; } display->drawRect(x,y,w,h, color); }   
/* draw fill rec */
void fillRect(int x,int y, int w,int h,int color) { if(!_matrixSetup) { return ; }if(color==-1) { color=_color; } display->fillRect(x,y,w,h, color); }  

void drawLine(int x,int y, int w,int h, int xw,int yw, int color) { 
  if(!_matrixSetup) { return ; } if(color==-1) { color=_color; } 
  for(int i=0;i<xw;i++) {  for(int t=0;t<yw;t++) { display->drawLine(x+i,y+t,w+i,h+t, color); }}
}
 

// drawTriangle x y x2 y2 x3 y3 c - draw a trinagle (from x,y to x2,y2 to x3,y3 to x,y)
void drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
  if(!_matrixSetup) { return ; }
  if(color==-1) { color=_color; } display->drawTriangle(x0,y0,x1,y1,x2,y2,color);
}
// fillTriangle x y x2 y2 x3 y3 c -  draw a filled triangle
void fillTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
  if(!_matrixSetup) { return ; }
  if(color==-1) { color=_color; } display->fillTriangle(x0,y0,x1,y1,x2,y2,color);
}

// drawCircle x y w c - draw circle at x y with radius w 
void drawCircle(int x,int y, int w,int color) { 
  if(!_matrixSetup) { return ; } if(color==-1) { color=_color; } display->drawCircle(x,y,w, color); }   
// draw filled circle at x y with radius w 
void fillCircle(int x,int y, int w,int color) { 
  if(!_matrixSetup) { return ; } if(color==-1) { color=_color; } display->fillCircle(x,y,w, color); }  



void drawRoundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t radius, uint16_t color) { 
  if(!_matrixSetup) { return ; }
  if(color==-1) { color=_color; } display->drawRoundRect(x0,y0,w,h,radius,color); }
void fillRoundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t radius, uint16_t color) { 
  if(!_matrixSetup) { return ; }
  if(color==-1) { color=_color; } display->fillRoundRect(x0,y0,w,h,radius,color);  }

void drawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t size) { 
  if(!_matrixSetup) { return ; }
  if(color<0) { color=_color; } display->drawChar(x,y,c,color,bg,size); }
void drawText(int x,int y, int size, const char *str,int color) {
  if(!_matrixSetup) { return ; }
  if(color<0) { color=_color; }
  display->setTextSize(size);     // size 1 == 8 pixels high
  display->setTextWrap(true); // Don't wrap at end of line - will do ourselves
  display->setCursor(x,y);    // start at top left, with 8 pixel of spacing
  display->setTextColor(color); // set color
  display->print(str);  
}

void drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {
  if(!_matrixSetup) { return ; }
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
      if (bitRead(charColumn, j)) { dma_display->drawPixel(targetX, targetY, color);}
    }
  }
  
}
*/

/* draw 1-bit icon 
 *    convert tool https://javl.github.io/image2cpp/
*/
void drawIcon(int x, int y, int w,int h,int color,char *xbm,int size) {
  if(!_matrixSetup) { return ; }
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
  if(!_matrixSetup) { return ; }
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

int col_red;
int col_white;
int col_black;
int col_green;
int col_blue;

void pageTitle() {
  pageClear();

  // WIFI  
  drawArc(30,50 ,3, 90, 60, 30, 30, 3, col_red);
  drawArc(30, 50, 3, 90,  60, 20, 20, 3, col_red);
  drawArc(30, 50, 3, 90, 60, 10, 10, 3, col_red);

  long val= (100+WiFi.RSSI())/2;
 
  drawArc(30,50 ,3, 90, val, 30, 30, 3, col_green);
  drawArc(30, 50, 3, 90, val, 20, 20, 3, col_green);
  drawArc(30, 50, 3, 90, val, 10, 10, 3, col_green);

  fillTriangle(30, 50, 0, 15, 0, 50, 0);
  fillTriangle(30, 50, 64, 15, 64, 50, 0);
  fillCircle(30, 50, 3, col_red);

  drawText(1,1,1,prgTitle,col_red);   
  drawText(1,panelY-8,1,prgVersion,col_red);   

  drawFull(55,25,8,20,2,(int)ESP.getFreeHeap(),150000,col_red,col_white);
  //if(wifi_image1bit!=NULL && panelY>32) { drawIcon(0,20,0,0, col_red,wifi_image1bit,sizeof(wifi_image1bit));  }

  if(eeMode<=EE_MODE_AP ) {
    fillRect(panelX-13,panelY-9,panelX,panelY,col_green);
    drawText(panelX-12,panelY-8,1,"AP",col_red);
  }else if(eeMode!=EE_MODE_OK && eeMode!=EE_MODE_START) {
    fillRect(panelX-13,panelY-9,panelX,panelY,col_red);
    sprintf(buffer, "%d", eeMode); drawText(panelX-12,panelY-8,1,buffer,col_red);
  }

  if(is(appIP)) {  
    drawText(1,10,1,(char*)appIP.c_str(),col_red);   
  }

  draw();
  sprintf(buffer, "drawTitle red:%d green:%d blue:%d  white:%d",col_red,col_green,col_blue,col_white); logPrintln(LOG_DEBUG,buffer);
}

void pageTest() {
  pageClear();
  for (int i=32; i >=0; i--){ 
    int w=panelX-(i*2);
    int h=panelY-(i*2);
    drawRect(i, i, w, h,col_white); // white rect
    draw();
    delay(25);
  }
}

/* show esp page */
void pageEsp() {
  pageClear();

  uint32_t chipid=espChipId(); // or use WiFi.macAddress() ?
  snprintf(buffer,20, "%08X",chipid);
  drawText(15,1,1,buffer,col_white);
  // Heap
  drawText(1,10,1,"Heap",col_red);
  drawFull(40,10,20,8,2,(int)ESP.getFreeHeap(),150000,col_red,col_white);
  sprintf(buffer,"%d",ESP.getFreeHeap()); 
//  drawText(45,10,col_red,1,buffer);
// sketch
  drawText(1,20,1,"Sketch",col_red);
  drawFull(40,20,20,8,2,(int)ESP.getSketchSize(),(int)ESP.getFreeSketchSpace(),col_red,col_white);
  // bootType
  drawText(1,30,1,"CmdOs",col_red);
  drawText(40,30,1,bootType,col_white);
//  // mac
//  uint8_t baseMac[6]; esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
//  sprintf(buffer,"%02x:%02x:%02x:%02x:%02x:%02x\n",baseMac[0], baseMac[1], baseMac[2],baseMac[3], baseMac[4], baseMac[5]);
//  drawText(1,40,col_red,1,"Mac");
//  drawText(20,40,col_white,1,buffer);

  int a[13];
  // bar chart
  for(int i=0;i<12;i++) {
    int v=random(0,21); a[i]=v;
    drawFull(i*5+1,40,5,21,1,v,21,col_black,col_red);
  }
  drawLine(1,61,60,61,col_red);
  // line chart
  for(int i=1;i<12;i++) {    
    drawLine((i-1)*5+3,40+(a[i-1]),i*5+3,40+(a[i]),col_white);
  }    
  draw();
}

void pageTime() {
  pageClear();
  drawTime(10,5,col_red);
  drawDate(2,20,col_red);
  drawLine(5,15,60,15,col_white);
  draw();
  delay(250);
  effectStart(1,64,20,0,-5);
}  

void pageGif() {
  pageClear();
  int f=random(0,fsDirSize(".gif"));
  char* name=fsFile(".gif",f,0);
  drawFile(name,name,0,0,false);
  delay(250);
  drawFileClose();
  int rx=random(-1,2)*5;
  int ry=random(-1,2)*5;
  effectStart(1,64,20,rx,ry);
}

//-----------------------------------------------------------

char* matrixInfo() {
  sprintf(buffer, "Matrix enabled:%d ok:%d panelX:%d panelY:%d panelChain:%d brightness:%d rotation:%d pins:%s dmaBuffer:%d disBuffer:%d latBlanking:%d clkphase:%d",
    matrixEnable,_matrixSetup,eeMatrix.panelX,eeMatrix.panelY,eeMatrix.panelChain,eeMatrix.brightness,eeMatrix.rotation,eeMatrix.pins,
    eeMatrix.dmaBuffer,eeMatrix.displayBuffer,
    eeMatrix.latBlanking, eeMatrix.clkphase);
    return buffer;
}

void matrixSave() {
  if(eeAppPos<=0) { return ; }
  EEPROM.begin(EEBOOTSIZE);

  EEPROM.put(eeAppPos, eeMatrix ); 
  EEPROM.commit();  // Only needed for ESP8266 to get data written
  sprintf(buffer, "matrixSave eeAppPos:%d panelX:%d panelY:%d panelChain:%d brightness:%d rotation:%d pins:%s",
    eeAppPos,eeMatrix.panelX,eeMatrix.panelY,eeMatrix.panelChain,eeMatrix.brightness,eeMatrix.rotation,eeMatrix.pins);logPrintln(LOG_INFO,buffer);
}

void matrixLoad() {
  if(eeAppPos<=0) { return ; }
  else if(strcmp(eeType,bootType)!=0) { return ; }   // validate
  EEPROM.begin(EEBOOTSIZE);
  EEPROM.get(eeAppPos, eeMatrix); // eeBoot read
  EEPROM.end(); 
  sprintf(buffer, "matrixLoad eeAppPos:%d panelX:%d panelY:%d panelChain:%d brightness:%d rotation:%d pins:%s",
    eeAppPos,eeMatrix.panelX,eeMatrix.panelY,eeMatrix.panelChain,eeMatrix.brightness,eeMatrix.rotation,eeMatrix.pins);logPrintln(LOG_INFO,buffer);
}


boolean matrixInit() {
    matrixLoad(); // load eeMatrix
    if(!is(eeMatrix.pins,10,64)) { logPrintln(LOG_SYSTEM,"matrix init wrong"); return false;}

    panelX=eeMatrix.panelX;
    panelY=eeMatrix.panelY;
    if(panelX<=0 || panelY<=0) { sprintf(buffer,"matrix size wrong %d %d",panelX,panelY); logPrintln(LOG_ERROR,buffer); return false; }

    char* temp = strdup(eeMatrix.pins);  // Duplicate the string to avoid modifying the original
    char* token = strtok(temp, ",");
    int index = 0;

    _pins.r1=(int8_t)atoi(token); token = strtok(NULL, ",");
    _pins.g1=(int8_t)atoi(token); token = strtok(NULL, ",");
    _pins.b1=(int8_t)atoi(token); token = strtok(NULL, ",");
    _pins.r2=(int8_t)atoi(token); token = strtok(NULL, ",");
    _pins.g2=(int8_t)atoi(token); token = strtok(NULL, ",");
    _pins.b2=(int8_t)atoi(token); token = strtok(NULL, ",");
    _pins.a=(int8_t)atoi(token); token = strtok(NULL, ",");
    _pins.b=(int8_t)atoi(token); token = strtok(NULL, ",");
    _pins.c=(int8_t)atoi(token); token = strtok(NULL, ",");
    _pins.d=(int8_t)atoi(token); token = strtok(NULL, ",");
    _pins.e=(int8_t)atoi(token); token = strtok(NULL, ",");
    _pins.lat=(int8_t)atoi(token); token = strtok(NULL, ",");
    _pins.oe=(int8_t)atoi(token); token = strtok(NULL, ",");
    _pins.clk=(int8_t)atoi(token); token = strtok(NULL, ",");

    sprintf(buffer, "pins r1:%d g1:%d b1:%d r2:%d g2:%d b2:%d a:%d b:%d c:%d e:%d lat:%d oe:%d clk:%d",
      _pins.r1, _pins.g1, _pins.b1, _pins.r2, _pins.g2, _pins.b2, _pins.a, _pins.b, _pins.c, _pins.d, _pins.e, _pins.lat, _pins.oe, _pins.clk);
    logPrintln(LOG_INFO,buffer);
    
    free(temp);  // Free the duplicated string
    return true;
}
//-------------------------------------------------------------------

void matrixSetup() {  
  if(!matrixEnable) { return ; }
  else if(!matrixInit()) { matrixEnable=false; return ; } // init wrong
  
  // Module configuration
  HUB75_I2S_CFG mxconfig(eeMatrix.panelX, eeMatrix.panelY, eeMatrix.panelChain,_pins );
  mxconfig.double_buff = eeMatrix.dmaBuffer; // enable/disable buffer
  _matrixSetup=true;  

//TODO driver  if(is(eeMatrix.driver)) { mxconfig.driver = eeMatrix.driver; }

  mxconfig.clkphase = eeMatrix.clkphase;

  // Display Setup
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  col_red=dma_display->color444(15,0,0);
  col_white=dma_display->color444(15,15,15);
  col_black=dma_display->color444(0,0,0);  
  col_green=dma_display->color444(0,15,0);  
  col_blue=dma_display->color444(0,0,15);  

  dma_display->setLatBlanking(eeMatrix.latBlanking);

  if(eeMatrix.displayBuffer) { 
    display=new GFXcanvas16(eeMatrix.panelX, eeMatrix.panelY);  
    if(display==NULL) {  display=dma_display; eeMatrix.displayBuffer=false; } // no ram
  }else { display=dma_display; }

  dma_display->begin();
  dma_display->setBrightness8(eeMatrix.brightness);  //  Brightness 0-255
  dma_display->setRotation(eeMatrix.rotation);      // Rotation, 0-4
  dma_display->clearScreen();

  pageTest();  
  pageTitle(); matrixPage=1;
  

}

void matrixStatus() {
  if(eeMode<=EE_MODE_AP) { drawLine(0,panelY,panelX,panelY,col_blue);} // blue => AP Mode
  else if(eeMode!=EE_MODE_OK) { drawLine(0,panelY,panelX,panelY,col_red);} // red => Mode wrong  
}

void matrixLoop() {
  if(!matrixEnable && !_matrixSetup) { return ; }
  if(matrixPage>0) {
    if(isTimer(matrixPageTime, 1000)) { 
      if(matrixPage==1) { pageTitle(); } // draw title again 
      else if(matrixPage==2) { pageEsp(); } 
      else if(matrixPage==3) { pageTest(); } 
      else if(matrixPage==4) { pageTime(); } 
      else if(matrixPage==5) { pageGif(); } 
    }
  }else {
    if(isTimer(matrixPageTime, 1000)) { matrixStatus(); } // draw staus
  }  
}
