
// on a 64x64 LED matrix with HUB75 Connector
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

MatrixPanel_I2S_DMA *dma_display = nullptr;

Adafruit_GFX *display=NULL;

byte _effectType=0;
byte fontSize=1;

//--------------------------------------

int pixelX=64;
int pixelY=64;

int col_red;
int col_white;
int col_black;
int col_green;
int col_blue;

// Matrix store structur
typedef struct {
  int pX=pixelX;
  int pY=pixelY;
  byte panelChain=1;
  char pins[64]="0,15,4,16,27,17,5,18,19,21,12,33,25,22";
  byte brightness=90;
  byte rotation=0;
  boolean dmaBuffer=false;
  boolean displayBuffer=true;
  byte latBlanking=1;
  boolean clkphase=true;  
  char* driver=NULL;
} eeDisplay_t;
eeDisplay_t eeDisplay;    // matrix store object 

HUB75_I2S_CFG::i2s_pins _pins;

boolean _displaySetup=false; // matrix setup ok

//--------------------------------------

char* displayInfo() {
  sprintf(buffer, "Matrix enabled:%d ok:%d pixelX:%d pixelY:%d panelChain:%d brightness:%d rotation:%d pins:%s dmaBuffer:%d disBuffer:%d latBlanking:%d clkphase:%d",
    displayEnable,_displaySetup,eeDisplay.pX,eeDisplay.pY,eeDisplay.panelChain,eeDisplay.brightness,eeDisplay.rotation,eeDisplay.pins,
    eeDisplay.dmaBuffer,eeDisplay.displayBuffer,
    eeDisplay.latBlanking, eeDisplay.clkphase);
    return buffer;
}

void displaySave() {
  if(eeAppPos<=0) { return ; }
  EEPROM.begin(EEBOOTSIZE);

  EEPROM.put(eeAppPos, eeDisplay ); 
  EEPROM.commit();  // Only needed for ESP8266 to get data written
  sprintf(buffer, "displaySave eeAppPos:%d pixelX:%d pixelY:%d panelChain:%d brightness:%d rotation:%d pins:%s",
    eeAppPos,eeDisplay.pX,eeDisplay.pY,eeDisplay.panelChain,eeDisplay.brightness,eeDisplay.rotation,eeDisplay.pins);logPrintln(LOG_INFO,buffer);
}

void displayLoad() {
  if(eeAppPos<=0) { return ; }
  else if(strcmp(eeType,bootType)!=0) { return ; }   // validate
  EEPROM.begin(EEBOOTSIZE);
  EEPROM.get(eeAppPos, eeDisplay); // eeBoot read
  EEPROM.end(); 
  sprintf(buffer, "displayLoad eeAppPos:%d pixelX:%d pixelY:%d panelChain:%d brightness:%d rotation:%d pins:%s",
    eeAppPos,eeDisplay.pX,eeDisplay.pY,eeDisplay.panelChain,eeDisplay.brightness,eeDisplay.rotation,eeDisplay.pins);logPrintln(LOG_INFO,buffer);
}


boolean displayInit() {
    displayLoad(); // load eeDisplay
    if(!is(eeDisplay.pins,10,64)) { logPrintln(LOG_SYSTEM,"matrix init wrong"); return false;}

    pixelX=eeDisplay.pX;
    pixelY=eeDisplay.pY;
    if(pixelX<=0 || pixelY<=0) { sprintf(buffer,"matrix size wrong %d %d",pixelX,pixelY); logPrintln(LOG_ERROR,buffer); return false; }

    char* temp = strdup(eeDisplay.pins);  // Duplicate the string to avoid modifying the original
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

//-----------------------------------------------------------

/* draw actual screen => flip to back buffer */
void displayDraw() {
  if(!_displaySetup) { return ; }
  if(eeDisplay.dmaBuffer) { dma_display->flipDMABuffer(); }// Show the back buffer, set currently output buffer to the back (i.e. no longer being sent to LED panels)
  else if(eeDisplay.displayBuffer) {
    GFXcanvas16 *canvas=(GFXcanvas16*)display;  
    uint16_t *buffer=canvas->getBuffer();
    dma_display->drawRGBBitmap(0,0,buffer,pixelX,pixelY);
  }
}

/* clear display */
void displayClear() { if(!_displaySetup) { return ; } dma_display->clearScreen(); }
void bufferClear() { 
  if(_displaySetup && eeDisplay.displayBuffer && display!=NULL) { 
    display->fillRect(0,0,pixelX,pixelY,0); 
  } 
}

/* set Brightness 0-255 **/
void displayBrightness(int b) {  if(!_displaySetup) { return ; } dma_display->setBrightness8(b); } 
char* displayRotation(int r) {  if(r>=0) { display->setRotation(r); } sprintf(buffer,"%d",display->getRotation()); return buffer; } 

/* color color */
uint16_t toColor444(int r,int g,int b) { if(!_displaySetup) { return -1; } return dma_display->color444(r, g, b); }
uint16_t toColor565(int r,int g,int b) { if(!_displaySetup) { return -1; } return dma_display->color565(r, g, b); }

//-------------------------------------------------------------------


void displaySetup() {  
  if(!displayEnable) { return ; }
  else if(!displayInit()) { displayEnable=false; return ; } // init wrong
  
  // Module configuration
  HUB75_I2S_CFG mxconfig(eeDisplay.pX, eeDisplay.pY, eeDisplay.panelChain,_pins );
  mxconfig.double_buff = eeDisplay.dmaBuffer; // enable/disable buffer
  _displaySetup=true;  

//TODO driver  if(is(eeDisplay.driver)) { mxconfig.driver = eeDisplay.driver; }

  mxconfig.clkphase = eeDisplay.clkphase;

  // Display Setup
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);

  dma_display->setLatBlanking(eeDisplay.latBlanking);

  if(eeDisplay.displayBuffer) { 
    display=new GFXcanvas16(eeDisplay.pX, eeDisplay.pY);  
    if(display==NULL) {  display=dma_display; eeDisplay.displayBuffer=false; } // no ram
  }else { display=dma_display; }

  dma_display->begin();
  dma_display->setBrightness8(eeDisplay.brightness);  //  Brightness 0-255
  dma_display->setRotation(eeDisplay.rotation);      // Rotation, 0-4
  dma_display->clearScreen();

  col_red=toColor444(15,0,0);
  col_white=toColor444(15,15,15);
  col_black=toColor444(0,0,0);  
  col_green=toColor444(0,15,0);  
  col_blue=toColor444(0,0,15); 

}



