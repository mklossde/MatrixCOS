
// on a 64x64 LED matrix with HUB75 Connector
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

MatrixPanel_I2S_DMA *dma_display = nullptr;

Adafruit_GFX *display=NULL;

//--------------------------------------

int panelX=64;
int panelY=64;

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

HUB75_I2S_CFG::i2s_pins _pins;

boolean _displaySetup=false; // matrix setup ok

//--------------------------------------

char* displayInfo() {
  sprintf(buffer, "Matrix enabled:%d ok:%d panelX:%d panelY:%d panelChain:%d brightness:%d rotation:%d pins:%s dmaBuffer:%d disBuffer:%d latBlanking:%d clkphase:%d",
    matrixEnable,_displaySetup,eeMatrix.panelX,eeMatrix.panelY,eeMatrix.panelChain,eeMatrix.brightness,eeMatrix.rotation,eeMatrix.pins,
    eeMatrix.dmaBuffer,eeMatrix.displayBuffer,
    eeMatrix.latBlanking, eeMatrix.clkphase);
    return buffer;
}

void displaySave() {
  if(eeAppPos<=0) { return ; }
  EEPROM.begin(EEBOOTSIZE);

  EEPROM.put(eeAppPos, eeMatrix ); 
  EEPROM.commit();  // Only needed for ESP8266 to get data written
  sprintf(buffer, "displaySave eeAppPos:%d panelX:%d panelY:%d panelChain:%d brightness:%d rotation:%d pins:%s",
    eeAppPos,eeMatrix.panelX,eeMatrix.panelY,eeMatrix.panelChain,eeMatrix.brightness,eeMatrix.rotation,eeMatrix.pins);logPrintln(LOG_INFO,buffer);
}

void displayLoad() {
  if(eeAppPos<=0) { return ; }
  else if(strcmp(eeType,bootType)!=0) { return ; }   // validate
  EEPROM.begin(EEBOOTSIZE);
  EEPROM.get(eeAppPos, eeMatrix); // eeBoot read
  EEPROM.end(); 
  sprintf(buffer, "displayLoad eeAppPos:%d panelX:%d panelY:%d panelChain:%d brightness:%d rotation:%d pins:%s",
    eeAppPos,eeMatrix.panelX,eeMatrix.panelY,eeMatrix.panelChain,eeMatrix.brightness,eeMatrix.rotation,eeMatrix.pins);logPrintln(LOG_INFO,buffer);
}


boolean displayInit() {
    displayLoad(); // load eeMatrix
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

//-----------------------------------------------------------

/* draw actual screen => flip to back buffer */
void displayDraw() {
  if(!_displaySetup) { return ; }
  if(eeMatrix.dmaBuffer) { dma_display->flipDMABuffer(); }// Show the back buffer, set currently output buffer to the back (i.e. no longer being sent to LED panels)
  else if(eeMatrix.displayBuffer) {
    GFXcanvas16 *canvas=(GFXcanvas16*)display;  
    uint16_t *buffer=canvas->getBuffer();
    dma_display->drawRGBBitmap(0,0,buffer,eeMatrix.panelX,eeMatrix.panelY);
  }
}

/* clear display */
void displayClear() {
  dma_display->clearScreen();
}

/* set Brightness 0-255 **/
void displayBrightness(int b) {  if(!_displaySetup) { return ; } dma_display->setBrightness8(b); } 

/* color color */
uint16_t toColor444(int r,int g,int b) { if(!_displaySetup) { return -1; } return dma_display->color444(r, g, b); }
uint16_t toColor565(int r,int g,int b) { if(!_displaySetup) { return -1; } return dma_display->color565(r, g, b); }

//-------------------------------------------------------------------

int col_red;
int col_white;
int col_black;
int col_green;
int col_blue;

void displaySetup() {  
  if(!matrixEnable) { return ; }
  else if(!displayInit()) { matrixEnable=false; return ; } // init wrong
  
  // Module configuration
  HUB75_I2S_CFG mxconfig(eeMatrix.panelX, eeMatrix.panelY, eeMatrix.panelChain,_pins );
  mxconfig.double_buff = eeMatrix.dmaBuffer; // enable/disable buffer
  _displaySetup=true;  

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
}



