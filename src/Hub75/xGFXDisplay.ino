

class PageFunc { 
private: 
  void (*pageSetup)();
  void (*pageLoop)();
  const char *pageName;

public:
  void doSetup() {  if(pageSetup!=NULL) { pageSetup(); } }
  void doLoop() {  if(pageLoop!=NULL) { pageLoop(); } }
  char* toString() { return (char*)pageName; }

  PageFunc() { pageSetup = NULL; pageLoop = NULL;  pageName=NULL; }
  PageFunc(const char *name,void (*pSetup)(),void (*pLoop)()) {
    pageName=name; pageSetup=pSetup; pageLoop=pLoop;
  }

}; 

PageFunc* pageFunc=NULL;  // actual page
MapList pages;   // list of pages

byte pageIndex=0; // actual page 0=off,1=title
unsigned long *pageRefreshTime = new unsigned long(0); // timer to refresh page

unsigned long *pageLoopTime = new unsigned long(0); // pageLoop timer 
int pageLoopValue=0;

int pageRefresh=60000; // full refresh page (pageSetup) / -1 == no refresh

//--------------------------------

/* clear from page */
void pageClear() {
  if(eeDisplay.displayBuffer) {  bufferClear(); } else { displayClear(); }
  _effectType=0; // effect off
}

/* enabel a page to display */
byte pageSet(int page) {
  if(page>=0) { 
    pageIndex=page;
    *pageRefreshTime=0;
    if(pageIndex==0) { pageFunc=NULL; } else { pageFunc=(PageFunc*)pages.get(pageIndex-1); }
    sprintf(buffer,"set page:%d pageRefreshTime:%d ",pageIndex,*pageRefreshTime);logPrintln(LOG_DEBUG,buffer);
  } 
  return pageIndex;
}

/* enabel a page to display */
byte pageChange(int pageAdd) {
  byte page=pageIndex+pageAdd;
  if(page<1) { page=pages.size(); }
  if(page>pages.size()) { page=1; } 
  return pageSet(page);
}


char* pageList() {
  sprintf(buffer,"");
  for(int i=0;i<pages.size();i++) {  
    pageFunc=(PageFunc*)pages.get(i);
    if(pageFunc!=NULL) {
      sprintf(buffer+strlen(buffer),"page %i %s\n",i,pageFunc->toString());
    }
  }
  return buffer;
}

char* pageDel(int pageIndex) {
  pages.del(pageIndex);
  return EMPTY;
}

//--------------------------------------------------------

void pageSetup() {  }

void pageLoop() {
  if(!displayEnable && !_displaySetup) { return ; }
  if(pageIndex>0 && isTimer(pageRefreshTime, pageRefresh)) {

    if(pageFunc!=NULL) { 
      sprintf(buffer,"page setup %d",pageIndex);logPrintln(LOG_DEBUG,buffer);    
      pageFunc->doSetup(); 
    }else {
      sprintf(buffer,"no page found %d",pageIndex);logPrintln(LOG_ERROR,buffer);   
    }  
  } else {
    if(pageFunc!=NULL) { pageFunc->doLoop(); }
  }
}


//------------------------------------------------------------
// effect

int _effectSpeed=100;
int _effectStep=0;
int _effectIndex=0;

int _effectA=1;
int _effectB=0;

unsigned long *effectTimer = new unsigned long(0);





/* shift image [counter timePerCount plusX plusY] 
   (e.g. effect 1 64 10 -1 0 ) 
   plus>5,<-5 => matrix/random shift e.g. effect 1 64 100 0 -5 
*/
void effectShift() {
    for(int x=0;x<pixelX;x++) {
      for(int y=0;y<pixelY;y++) {
        GFXcanvas16 *canvas=(GFXcanvas16*)display;
        int px=x, py=y; uint16_t color=0;
        if(_effectA<0) { px=pixelX-x-1; }        
        if(_effectB<0) { py=pixelY-y-1; }
        int posX, posY;
        if(_effectA>=5) { posX=px+random(1,_effectA); }        
        else if(_effectA<=-5) { posX=px-random(1,(_effectA*-1)); }    
        else { posX=px+_effectA; }
        if(_effectB>=5) { posY=py+random(1,_effectB); } 
        else if(_effectB<=-5) { posY=py-random(1,(_effectB*-1)); } 
        else { posY=py+_effectB; }
        if(posX>=0 && posX<pixelX && posY>=0 && posY<pixelY) { color= canvas->getPixel(posX,posY); }
        canvas->drawPixel(px,py,color);
      }
    }
    draw();
}

void effectLoop() {
  if(eeDisplay.displayBuffer && _effectType!=0 && _effectIndex<_effectStep && isTimer(effectTimer, _effectSpeed)) { 
    if(_effectType==1) { effectShift();  }
//    else if(_effectType==2) { effectDim();  }
    _effectIndex++;
    if(_effectIndex==_effectStep) { logPrintln(LOG_DEBUG, "effect end"); }
  }
}

/* start effect */
void effectStart(byte effectType,int effectStep,int effectSpeed,int effectA,int effectB) {
  sprintf(buffer,"effect start %d %d %d %d %d",effectType,effectStep,effectSpeed,effectA,effectB); logPrintln(LOG_DEBUG,buffer);
  _effectType=effectType;
  _effectStep=effectStep; _effectSpeed=effectSpeed;
  _effectA=effectA; _effectB=effectB;
  _effectIndex=0;
//  *effectTimer=0;
}


AnimatedGIF gif;
File f;
int x_offset, y_offset;

boolean _playGif=false;
boolean _playStart=false;
int frames=0;

void GIFDraw(GIFDRAW *pDraw) {
//    if(!_playStart || _playGif) { return ; } // do not end after stop
    uint8_t *s;
    uint16_t *d, *usPalette, usTemp[320];
    int x, y, iWidth;

    iWidth = pDraw->iWidth;
//    if (iWidth > MATRIX_WIDTH) { iWidth = MATRIX_WIDTH; }      
    if (iWidth > pixelX) { iWidth = pixelX; }   

    usPalette = pDraw->pPalette;
    y = pDraw->iY + pDraw->y; // current line
    
    s = pDraw->pPixels;
    if (pDraw->ucDisposalMethod == 2) {// restore to background color    
      for (x=0; x<iWidth; x++) {
        if (s[x] == pDraw->ucTransparent)
           s[x] = pDraw->ucBackground;
      }
      pDraw->ucHasTransparency = 0;
    }

    if (pDraw->ucHasTransparency) {// if transparency used
      uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
      int x, iCount;
      pEnd = s + pDraw->iWidth;
      x = 0;
      iCount = 0; // count non-transparent pixels
      while(x < pDraw->iWidth) {
        c = ucTransparent-1;
        d = usTemp;
        while (c != ucTransparent && s < pEnd) {
          c = *s++;
          if (c == ucTransparent) { // done, stop          
            s--; // back up to treat it like transparent
          } else {// opaque          
             *d++ = usPalette[c];
             iCount++;
          }
        } // while looking for opaque pixels
        if (iCount) {// any opaque pixels?        
          for(int xOffset = 0; xOffset < iCount; xOffset++ ){
            drawPixel(x + xOffset+x_offset, y+y_offset, usTemp[xOffset]); // 565 Color Format
          }
          x += iCount;
          iCount = 0;
        }
        // no, look for a run of transparent pixels
        c = ucTransparent;
        while (c == ucTransparent && s < pEnd) {        
          c = *s++;
          if (c == ucTransparent) { iCount++;}
          else { s--;  }
        }
        if (iCount) {
          x += iCount; // skip these
          iCount = 0;
        }
      }
    } else { // does not have transparency    
      s = pDraw->pPixels;
      // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
      for (x=0; x<pDraw->iWidth; x++) { drawPixel(x+x_offset, y+y_offset, usPalette[*s++]); }  // color 565
    }
    if(y==gif.getCanvasHeight()-1) { draw(); }
//Serial.print("y:");Serial.println(y);    
 } /* GIFDraw() */


/* open gif file */
void * GIFOpenFile(const char *fname, int32_t *pSize) {  
//TODO  if(!name.startsWith("/")) { name="/"+name; }
  f = FILESYSTEM.open(fname);
  if (!f) { sprintf(buffer,"GIF file missing '%s'",fname);logPrintln(LOG_INFO,buffer); return NULL; }  

  //sprintf(buffer,"GIF open %s",fname);logPrintln(LOG_INFO,buffer);
  *pSize = f.size();
  return (void *)&f;

 } 


void GIFCloseFile(void *pHandle) {
  File *f = static_cast<File *>(pHandle);
  if (f != NULL)
     f->close();
 } /* GIFCloseFile() */

 int32_t GIFReadFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen) {
    int32_t iBytesRead;
    iBytesRead = iLen;
    File *f = static_cast<File *>(pFile->fHandle);
    // Note: If you read a file all the way to the last byte, seek() stops working
    if ((pFile->iSize - pFile->iPos) < iLen)
       iBytesRead = pFile->iSize - pFile->iPos - 1; // <-- ugly work-around
    if (iBytesRead <= 0)
       return 0;
    iBytesRead = (int32_t)f->read(pBuf, iBytesRead);
    pFile->iPos = f->position();
    return iBytesRead;
 } /* GIFReadFile() */

 int32_t GIFSeekFile(GIFFILE *pFile, int32_t iPosition) { 
  int i = micros();
  File *f = static_cast<File *>(pFile->fHandle);
  f->seek(iPosition);
  pFile->iPos = (int32_t)f->position();
  i = micros() - i;
  return pFile->iPos;
 } /* GIFSeekFile() */

 unsigned long start_tick = 0;

//------------------------------------------------------

boolean drawIcon(int x, int y, int w, int h, int color,char *name) {
  size_t dataSize = 0; // gif data size
  uint8_t *data = fsReadBin(name, dataSize); 
  if(dataSize<=0) { return false; }
  drawIcon(x,y,w,h, color,(char*)data,dataSize); // dataSize 
  delete[] data; data=NULL;
  return true;   
}

/*
void saveIcon1(char *name,int x,int y,int w,int h,int colorRange) {
  File ff = FILESYSTEM.open(name, FILE_WRITE); 
  ff.write(1);
  ff.write(w);
  ff.write(h);
  ff.close();
}
*/

//------------------------------------------------------

void drawFile(char *name,char *suffix,int x,int y,boolean direct) {
  if(!is(name)) { return ;  }
  sprintf(buffer,"drawFile %s %s %d %d %d",name,suffix,x,y,direct); logPrintln(LOG_DEBUG,buffer); 
  if(endsWith(suffix,".bm1")) {
    int col=-1; if(direct) { col=col_white;}
    drawIcon(0,0,0,0,col,name);
    if(direct) { draw(); }
    return ;

  }else if(endsWith(suffix,".gif")) {
//    drawClear();
    _playGif=false;

    x_offset=x; y_offset=y;
    if (x_offset < 0) { x_offset = 0;}
    if (y_offset < 0) { y_offset = 0; }

    if(!startWith(name,"/")) { sprintf(buffer,"/%s",name); name=buffer; }
    if (gif.open(name, GIFOpenFile, GIFCloseFile, GIFReadFile, GIFSeekFile, GIFDraw)) {
      sprintf(buffer,"GIF draw %s; (%d x %d)", name, gif.getCanvasWidth(), gif.getCanvasHeight());logPrintln(LOG_DEBUG,buffer);
      _playStart=true;
      _playGif=false; 
      frames=0;   
    }
    return ;

  }else if(endsWith(suffix,".cmd")) {
    cmdFile(name);

  }else { sprintf(buffer,"drawFile suffix unkown '%s'", suffix);logPrintln(LOG_ERROR,buffer); }
} 

//String uploadFile=String("upload");
char* uploadFile="/upload_file";
String URL_FILE="/upload_url";

void drawUrl(String url,int x,int y,boolean direct) {
  boolean ok=fsDownload(url,URL_FILE);
  if(ok) { 
    String name=url.substring(url.lastIndexOf('/'));
    drawFile((char*)URL_FILE.c_str(),(char*)name.c_str(),0,0,direct);
  }
}

/* draw upload */
void drawUpload(AsyncWebServerRequest *request, String file, size_t index, uint8_t *data, size_t len, bool final) {
  if(!isWebAccess(ACCESS_READ)) { request->send(403, "text/html"); }
  sprintf(buffer, "upload %s %d index:%d", file.c_str(), len, index);logPrintln(LOG_DEBUG,buffer);

  File ff;
  if (!index) {     
    FILESYSTEM.remove(rootDir + uploadFile); // remove old file 
    ff = SPIFFS.open(rootDir + uploadFile, FILE_WRITE); 
  }else { ff = SPIFFS.open(rootDir + uploadFile, FILE_APPEND); }

  for (size_t i = 0; i < len; i++) { ff.write(data[i]); }
  ff.close();
  if (final) {
    sprintf(buffer, "uploaded %s => %s %d", file.c_str(),uploadFile, (index + len)); logPrintln(LOG_DEBUG,buffer);
    char* name=copy(file); 
    drawFile(uploadFile,name,0,0,true);
    delete name;
    request->redirect("/app");
  }
}

/* stop gif play */
void drawFileClose() {
  _playStart=false;
  _playGif=false;
  gif.close();  
}

/* stop all prg /drawings */
void drawStop() {
//  pageSet(0);
  drawFileClose();
  prgStop();
}

/* clear matrix */
void drawClose() {
  drawStop();
  drawClear();
  draw(); 
}

/* stop and clear matrix */
void drawOff() {
  pageSet(0);
  drawClose();
}


//------------------------------------------

void drawLoop() {
    // play GifImage 
    if(_playStart) {
      frames++;  
      if(!gif.playFrame(true, NULL)) {
        _playStart=false;
        _playGif=frames>1; // animate gif if > 1 frame
      }
    }else if(_playGif ) {  gif.playFrame(true, NULL); }
}

      


//------------------------------------------------------------
// web

char* cmdSetMatrix(char* p0,char* p1,char* p2,char* p3,char* p4,char* p5) {  
  if(is(p0)) { 
    if(!isAccess(ACCESS_ADMIN))  { return "ACCESS DENIED setMatrix"; }
    if(is(p0,1,3)) { eeDisplay.pX=toInt(p0); }
    if(is(p1,1,3)) { eeDisplay.pY=toInt(p1); }
    if(is(p2,1,2)) { eeDisplay.panelChain=toInt(p2); }  
    if(is(p3,1,3)) { eeDisplay.brightness=toInt(p3);}
    if(is(p4,1,2)) { eeDisplay.rotation=toInt(p4);}
    if(is(p5,1,64)) { strcpy(eeDisplay.pins,p5); }
    displaySave();
  }
  return displayInfo();
}

char* cmdSetMatrix2(boolean dmaBuffer, boolean displayBuffer,int latBlanking,boolean clkphase,char *driver) {
  eeDisplay.dmaBuffer=dmaBuffer;
  eeDisplay.displayBuffer=displayBuffer;
//  eeDisplay.latBlanking=latBlanking;
//  eeDisplay.clkphase=clkphase;
//  if(is(driver)) { eeDisplay.driver=copy(driver); }
  displaySave();
  return displayInfo();
}

void matrixWebSetupSet(AsyncWebServerRequest *request) {
  String v;
  v=webParam(request,"pixelX"); if(is(v,1,5)) {  eeDisplay.pX=v.toInt(); }
  v=webParam(request,"pixelY"); if(is(v,1,5)) {  eeDisplay.pY=v.toInt(); }
  v=webParam(request,"chain"); if(is(v,1,2)) {  eeDisplay.panelChain=v.toInt(); }
  v=webParam(request,"pins"); if(is(v,1,64)) {  v.toCharArray(eeDisplay.pins, sizeof(eeDisplay.pins)); }
  v=webParam(request,"brightness"); if(is(v,1,4)) {  eeDisplay.brightness=v.toInt(); }
  v=webParam(request,"rotation"); if(is(v,1,2)) {  eeDisplay.rotation=v.toInt(); }
  displaySave();
} 

void matrixWebSetup(AsyncWebServerRequest *request) {
  String message; if (request->hasParam("ok")) { matrixWebSetupSet(request); message="set"; }

  String html = ""; html = pageHead(html, "MatrixHup");
  html+= "[<a href='/config'>network</a>][<a href='/appSetup'>app</a>]";
  html = pageForm(html, "MatrixHub75 config");
  html = pageInput(html, "pixelX", to(eeDisplay.pX));
  html = pageInput(html, "pixelY", to(eeDisplay.pY));
  html = pageInput(html, "chain", to(eeDisplay.panelChain));
  html = pageInput(html, "pins", eeDisplay.pins);
  html = pageInput(html, "brightness", to(eeDisplay.brightness));
  html = pageInput(html, "rotation", to(eeDisplay.rotation));
  html = pageButton(html, "ok", "ok");
  html = pageFormEnd(html);
  html = pageEnd(html,message);
  request->send(200, "text/html", html);
}



void matrixWeb(AsyncWebServerRequest *request) {
  String message;
  if (request->hasParam("drawOff")) {  drawOff();
  }else if (request->hasParam("page")) { 
    String nr=webParam(request,"nr"); 
    pageSet(nr.toInt());
  }else if (request->hasParam("pageNext")) { pageChange(+1);  
  }else if (request->hasParam("pagePriv")) { pageChange(-1);  
  }else if (request->hasParam("drawFile")) { 
    String name=webParam(request,"name");  
    char *file=(char*)name.c_str();
    drawFile(file,file,0,0,true);
  }else if (request->hasParam("drawCmd")) {
    String name=webParam(request,"name");  
    String ret=cmdFile((char*)name.c_str());
  }else if (request->hasParam("drawUrl")) {
    drawUrl(webParam(request,"url"),0,0,true);
  }

  String html = ""; html = pageHead(html, "MatrixHup");
  File root = FILESYSTEM.open(rootDir);
  File foundfile = root.openNextFile();
  html+="[<a href=?page=1&nr=1>Title</a>][<a href=?pageNext=1>NextPage</a>][<a href=?pagePriv=1>PrivPage</a>][<a href=?drawOff=1>OFF</a>]";
  html+="<table><tr>";
  int cols=0;
  while (foundfile) { 
    String name = String(foundfile.name());
    if(name.endsWith(".gif") || name.endsWith(".bm1")) { 
      html += "<td><a href='?drawFile=1&name=" + rootDir + name + "'><img src='/res?name="+ rootDir +name+"' width='64' height='64'/><br>" + name + "</a></td>";
      if(cols++>10) { cols=0; html+="</tr><tr>";}
    }else if(name.endsWith(".cmd")) {
      html += "<td><a href='?drawCmd=1&name=" + rootDir + name + "'>CMD<br>" + name + "</a></td>";
      if(cols++>10) { cols=0; html+="</tr><tr>";}
    }
    foundfile = root.openNextFile();
    
  }
  html+="</tr></table><hr>";
//  html = pageUpload(html, "Draw gif/bm1/cmd", "/drawUpload");
//  html += "<form method='GET'>Draw URL gif/bm1/cmd<input type='text' size='64' name='url'/><input type='submit' name='drawUrl' value='ok'/></form>";
  root.close();
  foundfile.close();


  html = pageEnd(html,message);
  request->send(200, "text/html", html);
}


//------------------------------------------------------------
// cmd

char* matrixCmd(char *cmd, char **param) {
    // drawOff => switch display off by clear and stop all
    if(equals(cmd, "drawOff")) { drawOff(); return EMPTY; }
    // drawClear => clear display
    else if(equals(cmd, "drawClear")) { drawClear(); return EMPTY; } 
    // drawStop => stop all (not clear)
    else if(equals(cmd, "drawStop")) { drawFileClose(); return EMPTY; } 
    // draw => draw buffer, draw content from buffer on display or flip dma buffer
    else if(equals(cmd, "draw")) { draw(); return EMPTY; } 

    // brightness n - set up brightness of dislpay
    else if(equals(cmd, "brightness")) { int b=toInt(cmdParam(param)); displayBrightness(b); return EMPTY; }
    else if(equals(cmd, "rotation")) { int b=toInt(cmdParam(param)); return displayRotation(b); }

    //drawColor r g b - calculate 444 color and set as default color    
    // default color for all draw-commands (use by draw command if no or -1 is given as color)
    else if(equals(cmd, "drawColor")) { uint16_t col=toColor444(toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)));drawColor(col); sprintf(buffer,"%d",col); return buffer; }
    // drawColor565 r g b - calculate 565 color and set as default color 
    else if(equals(cmd, "drawColor565")) { uint16_t col=toColor565(toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param))); drawColor(col); sprintf(buffer,"%d",col); return buffer; }

    //fillScreen c - fill screen with color	
    else if(equals(cmd, "fillScreen")) { fillScreen(toInt(cmdParam(param))); return EMPTY; }
    // drawPixel x y c - draw a pixel at x y
    else if(equals(cmd, "drawPixel")) { drawPixel(toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param))); return EMPTY; }
    // drawLine x y x2 y2 c - draw a line (from x,y to x2,y2)
    else if(equals(cmd, "drawLine")) { drawLine(toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param))); return EMPTY; }
    // draw line with thick 
    else if(equals(cmd, "drawWLine")) { drawLine(toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param))); return EMPTY; }
    // rawRect x y w h c - draw rect (rect from x,y to x+w,y+h)
    else if(equals(cmd, "drawRect")) { drawRect(toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param))); return EMPTY; }
    // fillRect x y w h c - draw a filled rect
    else if(equals(cmd, "fillRect")) { fillRect(toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param))); return EMPTY; }
    // drawTriangle x y x2 y2 x3 y3 c - draw a trinagle (from x,y to x2,y2 to x3,y3 to x,y)
    else if(equals(cmd, "drawTriangle")) { drawTriangle(toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param))); return EMPTY; }
    // fillTriangle x y x2 y2 x3 y3 c -  draw a filled triangle
    else if(equals(cmd, "fillTriangle")) { fillTriangle(toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param))); return EMPTY; }

   // rawRect x y w h c - draw rect (rect from x,y to x+w,y+h)
    else if(equals(cmd, "drawRoundRect")) { drawRoundRect(toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param))); return EMPTY; }
    // fillRect x y w h c - draw a filled rect
    else if(equals(cmd, "fillRoundRect")) { fillRoundRect(toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param))); return EMPTY; }
 


    // drawCircle x y w c - draw circle at x y with radius w 
    else if(equals(cmd, "drawCircle")) { drawCircle(toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param))); return EMPTY; }
    // fillCircle x y w c - draw filled circle at x y with radius w 
    else if(equals(cmd, "fillCircle")) { fillCircle(toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param))); return EMPTY; }
    // drawArc x y angel seg rx ry w c - draw a segment arc at x,y start at angel (0=top), end after seg, with radiusX and radiusY and thikness w 
    else if(equals(cmd, "drawArc")) { drawArc(toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param))); return EMPTY; }
    // drawSegment s i - set segemnt step (segmentStep=6,segmentInc=6) segmentStep=6 => full arc have 60 segments, 3=120
//    else if(equals(cmd, "drawSegment")) { drawSegment(toInt(cmdParam(param)),toInt(cmdParam(param))); return EMPTY; }
    
    // drawFull x y w h p value max, c1,c2 - draw a full-element at x,y with w,h. full=100/max*value will be in color c2 and offset p
    else if(equals(cmd, "drawFull")) { drawFull(toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param))); return EMPTY; }
    else if(equals(cmd, "drawOn")) { drawOn(toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toBoolean(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param))); return EMPTY; }
    else if(equals(cmd, "drawGauge")) { drawGauge(toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param))); return EMPTY; }

    // valueFull x y value max c1 c2 - show value with name at full
    else if(equals(cmd, "valueFull")) { valueFull(toInt(cmdParam(param)),toInt(cmdParam(param)),cmdParam(param),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param))); return EMPTY; }
    else if(equals(cmd, "valueOn")) { valueOn(toInt(cmdParam(param)),toInt(cmdParam(param)),cmdParam(param),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param))); return EMPTY; }
    else if(equals(cmd, "valueGauge")) { valueGauge(toInt(cmdParam(param)),toInt(cmdParam(param)),cmdParam(param),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param))); return EMPTY; }


    // drawText x y c size text - draw text at x y with size 
    else if(equals(cmd, "drawText")) { drawText(toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),cmdParam(param),toInt(cmdParam(param))); return EMPTY; }

    // set page
    else if(equals(cmd, "page")) { int p=pageSet(toInt(cmdParam(param))); sprintf(buffer,"%d",p); return buffer; }
    // next page
    else if(equals(cmd, "pagePriv")) { int p=pageChange(-1);sprintf(buffer,"%d",p); return buffer; }
    // next page
    else if(equals(cmd, "pageNext")) {  int p=pageChange(1);sprintf(buffer,"%d",p); return buffer; }

    // set page
    else if(equals(cmd, "pages")) { return pageList();  }
//    else if(equals(cmd, "pageAdd")) { return pageAdd(cmdParam(param));  }
    else if(equals(cmd, "pageDel")) { return pageDel(toInt(cmdParam(param)));  }

    // drawFile file type x y - draw a gif/icon at x,y
    else if(equals(cmd, "drawFile")) { char *f=cmdParam(param); drawFile(f,f,toInt(cmdParam(param)),toInt(cmdParam(param)),false); return EMPTY; }    
    // drawUrl url x y - draw content of url (gif/icon) at a x 
    else if(equals(cmd, "drawUrl")) { drawUrl(toString(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),false); return EMPTY; }

    /* write wifi icon as wifi.bm1 file 
    else if(equals(cmd, "writeIcon")) { 
      uint8_t* uint8Ptr = (uint8_t*)reinterpret_cast<const uint8_t*>(wifi_image1bit);
      fsWriteBin("wifi.bm1",uint8Ptr,sizeof(wifi_image1bit)); return EMPTY; 
    } 
    */   
    // drawIcon x y w h c file - draw icon (bm1) at x,y with w,h of color 
    else if(equals(cmd, "drawIcon")) { drawIcon(toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),cmdParam(param)); return EMPTY; }
  
    // drawTime x y c - draw time (format hh:mm:ss) at x,y of color c 
    else if(equals(cmd, "drawTime")) { drawTime(toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param))); return EMPTY; }
    // drawDate x y c - draw date (format dd.mm.yyyy) at x,y of color c
    else if(equals(cmd, "drawDate")) { drawDate(toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param))); return EMPTY; }

    // effect type step speed a b - start effect type with n-steps with speed in ms between steps
    else if(equals(cmd, "effect")) { effectStart(toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param)),toInt(cmdParam(param))); return EMPTY;  } // start effect 

    // matrix sizeX sizeY chain brightness rotation pins - config hub75 dislpay/connection
	  // e.g. matrix 64 64 1 90 0 0,15,4,16,27,17,5,18,19,21,12,33,25,22
    else if(equals(cmd, "matrix")) { return cmdSetMatrix(cmdParam(param),cmdParam(param),cmdParam(param),cmdParam(param),cmdParam(param),cmdParam(param));  }
    // buffer dmaBuffer displayBuffer - (0=off/1=on) enable dmsBuffer or displayBuffer 
    else if(equals(cmd, "matrix2")) { return cmdSetMatrix2(toBoolean(cmdParam(param)),toBoolean(cmdParam(param)),toInt(cmdParam(param)),toBoolean(cmdParam(param)),cmdParam(param));  }

    else { return cmd; }
}



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
  if(eeDisplay.displayBuffer) { bufferClear(); } else { displayClear(); }
//  pageSet(0);
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
void drawTime(int x,int y,int size,int color) {  
  drawText(x,y,size,getTime(),color);
}

/* draw net-date at x,y with color */
//TODO do not work
void drawDate(int x,int y,int size,int color) {  
  drawText(x,y,size,getDate(),color);
}







