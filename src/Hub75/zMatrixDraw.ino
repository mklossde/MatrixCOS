
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
    if (iWidth > MATRIX_WIDTH) { iWidth = MATRIX_WIDTH; }      

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
//            dma_display->drawPixel(x + xOffset+x_offset, y+y_offset, usTemp[xOffset]); // 565 Color Format
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
//      for (x=0; x<pDraw->iWidth; x++) { dma_display->drawPixel(x+x_offset, y+y_offset, usPalette[*s++]); }  // color 565
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
    drawShowTitle=false;  
    _playGif=false;

    x_offset=x; y_offset=y;
    if (x_offset < 0) { x_offset = 0;}
    if (y_offset < 0) { y_offset = 0; }

    if (gif.open(name, GIFOpenFile, GIFCloseFile, GIFReadFile, GIFSeekFile, GIFDraw)) {
      sprintf(buffer,"GIF draw %s; (%d x %d)", name, gif.getCanvasWidth(), gif.getCanvasHeight());logPrintln(LOG_INFO,buffer);
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
    char* name=to(file); 
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
  drawClose();
}



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

/* draw net-time at x,y with color */
//TODO do not work
void drawTime(int x,int y,int color) {  
  drawText(x,y,color,1,getTime());
}

/* draw net-date at x,y with color */
//TODO do not work
void drawDate(int x,int y,int color) {  
  drawText(x,y,color,1,getDate());
}
        
//------------------------------------------------------------
// effect

int _effectSpeed=100;
int _effectStep=0;
int _effectIndex=0;

unsigned long *effectTimer = new unsigned long(0);

byte _effectA=1;
byte _effectB=0;
byte _effectType=0;

/* shift image [counter timePerCount plusX plusY] (e.g. effect 1 64 10 1 0 )*/
void effectShift() {
    for(int x=0;x<eeMatrix.panelX;x++) {
      for(int y=0;y<eeMatrix.panelY;y++) {
        GFXcanvas16 *canvas=(GFXcanvas16*)display;
        int px=x, py=y; uint16_t color=0;
        if(_effectA<0) { px=eeMatrix.panelX-x-1; }
        if(_effectB<0) { px=eeMatrix.panelY-y-1; }
        int posX=px+_effectA; int posY=py+_effectB;
//Serial.print(" posX:");Serial.print(posX)+Serial.print(" a:");Serial.print(_effectA);        
//Serial.print(" posY:");Serial.print(posY)+Serial.print(" b:");Serial.print(_effectB);Serial.println("");        
        if(posX>=0 && posX<eeMatrix.panelX && posY>=0 && posY<eeMatrix.panelY) { color= canvas->getPixel(posX,posY); }
        canvas->drawPixel(px,py,color);
      }
    }
    draw();
}

/* dim up/down display (e.g. effect 2 10 100) */
void effectDim() {
  if(_effectIndex==0) { _effectA=eeMatrix.brightness/(_effectStep-1); }
  int v=eeMatrix.brightness-(_effectA*_effectIndex);
  matrixBrightness(v);
  if(_effectIndex==_effectStep-1) { matrixBrightness(eeMatrix.brightness); }
} 

void effectLoop() {
  if(eeMatrix.displayBuffer && _effectIndex<_effectStep && isTimer(effectTimer, _effectSpeed)) { 
    if(_effectType==1) { effectShift();  }
    else if(_effectType==2) { effectDim();  }
    _effectIndex++;
  }
}

/* start effect */
void effectStart(byte effectType,int effectStep,int effectSpeed,int effectA,int effectB) {
  _effectType=effectType;
  _effectStep=effectStep; _effectSpeed=effectSpeed;
  _effectA=effectA; _effectB=effectB;
  _effectIndex=0;
//  *effectTimer=0;
}

//------------------------------------------------------------
// web

char* cmdSetMatrix(char* p0,char* p1,char* p2,char* p3,char* p4,char* p5) {  
  if(is(p0)) { 
    if(!isAccess(ACCESS_ADMIN))  { return "ACCESS DENIED setMatrix"; }
    if(is(p0,1,3)) { eeMatrix.panelX=toInt(p0); }
    if(is(p1,1,3)) { eeMatrix.panelY=toInt(p1); }
    if(is(p2,1,2)) { eeMatrix.panelChain=toInt(p2); }  
    if(is(p3,1,3)) { eeMatrix.brightness=toInt(p3);}
    if(is(p4,1,2)) { eeMatrix.rotation=toInt(p4);}
    if(is(p5,1,64)) { strcpy(eeMatrix.pins,p5); }
    matrixSave();
  }
  return matrixInfo();
}

char* cmdSetBuffer(boolean dmaBuffer, boolean displayBuffer) {
  eeMatrix.dmaBuffer=dmaBuffer;
  eeMatrix.displayBuffer=displayBuffer;
  matrixSave();
  return matrixInfo();
}

void matrixWebSetupSet(AsyncWebServerRequest *request) {
  String v;
  v=webParam(request,"panelX"); if(is(v,1,5)) {  eeMatrix.panelX=v.toInt(); }
  v=webParam(request,"panelY"); if(is(v,1,5)) {  eeMatrix.panelY=v.toInt(); }
  v=webParam(request,"chain"); if(is(v,1,2)) {  eeMatrix.panelChain=v.toInt(); }
  v=webParam(request,"pins"); if(is(v,1,64)) {  v.toCharArray(eeMatrix.pins, sizeof(eeMatrix.pins)); }
  v=webParam(request,"brightness"); if(is(v,1,4)) {  eeMatrix.brightness=v.toInt(); }
  v=webParam(request,"rotation"); if(is(v,1,2)) {  eeMatrix.rotation=v.toInt(); }
  matrixSave();
} 

void matrixWebSetup(AsyncWebServerRequest *request) {
  String message; if (request->hasParam("ok")) { matrixWebSetupSet(request); message="set"; }

  String html = ""; html = pageHead(html, "MatrixHup");
  html+= "[<a href='/config'>network</a>][<a href='/appSetup'>app</a>]";
  html = pageForm(html, "MatrixHub75 config");
  html = pageInput(html, "panelX", to(eeMatrix.panelX));
  html = pageInput(html, "panelY", to(eeMatrix.panelY));
  html = pageInput(html, "chain", to(eeMatrix.panelChain));
  html = pageInput(html, "pins", eeMatrix.pins);
  html = pageInput(html, "brightness", to(eeMatrix.brightness));
  html = pageInput(html, "rotation", to(eeMatrix.rotation));
  html = pageButton(html, "ok", "ok");
  html = pageFormEnd(html);
  html = pageEnd(html,message);
  request->send(200, "text/html", html);
}



void matrixWeb(AsyncWebServerRequest *request) {
  String message;
  if (request->hasParam("drawClear")) {  drawClose();
  }else if (request->hasParam("drawTitle")) {  pageTitle();
  }else if (request->hasParam("drawEsp")) {  pageEsp();
  }else if (request->hasParam("drawTest")) {  pageTest();
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
  html+="[<a href=?drawTitle=1>Title</a>][<a href=?drawEsp=1>Esp</a>][<a href=?drawTest=1>Test</a>][<a href=?drawClear=1>OFF</a>]";
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
  html = pageUpload(html, "Draw gif/bm1/cmd", "/drawUpload");
  html += "<form method='GET'>Draw URL gif/bm1/cmd<input type='text' size='64' name='url'/><input type='submit' name='drawUrl' value='ok'/></form>";
  root.close();
  foundfile.close();


  html = pageEnd(html,message);
  request->send(200, "text/html", html);
}


//------------------------------------------------------------
// cmd

char* matrixCmd(char *cmd, char *p0, char *p1,char *p2,char *p3,char *p4,char *p5,char *p6,char *p7,char *p8,char *p9) {
    // drawOff => switch display off by clear and stop all
    if(strcmp(cmd, "drawOff")==0) { drawOff(); return EMPTY; }
    // drawClear => clear display
    else if(strcmp(cmd, "drawClear")==0) { drawClear(); return EMPTY; } 
    // drawStop => stop all (not clear)
    else if(strcmp(cmd, "drawStop")==0) { drawFileClose(); return EMPTY; } 
    // draw => draw buffer, draw content from buffer on display or flip dma buffer
    else if(strcmp(cmd, "draw")==0) { draw(); return EMPTY; } 

    // brightness n - set up brightness of dislpay
    else if(strcmp(cmd, "brightness")==0) { int b=toInt(p0); matrixBrightness(b); return EMPTY; }
    
    //drawColor r g b - calculate 444 color and set as default color    
    // default color for all draw-commands (use by draw command if no or -1 is given as color)
    else if(strcmp(cmd, "drawColor")==0) { uint16_t col=toColor444(toInt(p0),toInt(p1),toInt(p2));drawColor(col); sprintf(buffer,"%d",col); return buffer; }
    // drawColor565 r g b - calculate 565 color and set as default color 
    else if(strcmp(cmd, "drawColor565")==0) { uint16_t col=toColor565(toInt(p0),toInt(p1),toInt(p2)); drawColor(col); sprintf(buffer,"%d",col); return buffer; }

    //fillScreen c - fill screen with color	
    else if(strcmp(cmd, "fillScreen")==0) { fillScreen(toInt(p0)); return EMPTY; }
    // drawPixel x y c - draw a pixel at x y
    else if(strcmp(cmd, "drawPixel")==0) { drawPixel(toInt(p0),toInt(p1),toInt(p2)); return EMPTY; }
    // drawLine x y x2 y2 c - draw a line (from x,y to x2,y2)
    else if(strcmp(cmd, "drawLine")==0) { drawLine(toInt(p0),toInt(p1),toInt(p2),toInt(p3),toInt(p4)); return EMPTY; }
    // rawRect x y w h c - draw rect (rect from x,y to x+w,y+h)
    else if(strcmp(cmd, "drawRect")==0) { drawRect(toInt(p0),toInt(p1),toInt(p2),toInt(p3),toInt(p4)); return EMPTY; }
    // fillRect x y w h c - draw a filled rect
    else if(strcmp(cmd, "fillRect")==0) { fillRect(toInt(p0),toInt(p1),toInt(p2),toInt(p3),toInt(p4)); return EMPTY; }
    // drawTriangle x y x2 y2 x3 y3 c - draw a trinagle (from x,y to x2,y2 to x3,y3 to x,y)
    else if(strcmp(cmd, "drawTriangle")==0) { drawTriangle(toInt(p0),toInt(p1),toInt(p2),toInt(p3),toInt(p4),toInt(p5),toInt(p6)); return EMPTY; }
    // fillTriangle x y x2 y2 x3 y3 c -  draw a filled triangle
    else if(strcmp(cmd, "fillTriangle")==0) { fillTriangle(toInt(p0),toInt(p1),toInt(p2),toInt(p3),toInt(p4),toInt(p5),toInt(p6)); return EMPTY; }

    // drawCircle x y w c - draw circle at x y with radius w 
    else if(strcmp(cmd, "drawCircle")==0) { drawCircle(toInt(p0),toInt(p1),toInt(p2),toInt(p3)); return EMPTY; }
    // fillCircle x y w c - draw filled circle at x y with radius w 
    else if(strcmp(cmd, "fillCircle")==0) { fillCircle(toInt(p0),toInt(p1),toInt(p2),toInt(p3)); return EMPTY; }
    // drawArc x y angel seg rx ry w c - draw a segment arc at x,y start at angel (0=top), end after seg, with radiusX and radiusY and thikness w 
    else if(strcmp(cmd, "drawArc")==0) { drawArc(toInt(p0),toInt(p1),toInt(p2),toInt(p3),toInt(p4),toInt(p5),toInt(p6),toInt(p7)); return EMPTY; }
    // drawSegment s i - set segemnt step (segmentStep=6,segmentInc=6) segmentStep=6 => full arc have 60 segments, 3=120
    else if(strcmp(cmd, "drawSegment")==0) { drawSegment(toInt(p0),toInt(p1)); return EMPTY; }
    
    // drawFull x y w h p value max, c1,c2 - draw a full-element at x,y with w,h. full=100/max*value will be in color c2 and offset p
    else if(strcmp(cmd, "drawFull")==0) { drawFull(toInt(p0),toInt(p1),toInt(p2),toInt(p3),toInt(p4),toInt(p5),toInt(p6),toInt(p7),toInt(p8)); return EMPTY; }
    else if(strcmp(cmd, "drawOn")==0) { drawOn(toInt(p0),toInt(p1),toInt(p2),toInt(p3),toBoolean(p4),toInt(p5),toInt(p6)); return EMPTY; }

    // valueFull x y value max c1 c2 - show value with name at full
    else if(strcmp(cmd, "valueFull")==0) { valueFull(toInt(p0),toInt(p1),p2,toInt(p3),toInt(p4),toInt(p5),toInt(p6)); return EMPTY; }
    else if(strcmp(cmd, "valueOn")==0) { valueOn(toInt(p0),toInt(p1),p2,toInt(p3),toInt(p4),toInt(p5),toInt(p6)); return EMPTY; }

    // drawText x y c size text - draw text at x y with size 
    else if(strcmp(cmd, "drawText")==0) { drawText(toInt(p0),toInt(p1),toInt(p2),toInt(p3),p4); return EMPTY; }

    // pageTest - draw test page - simple dislpay test
    else if(strcmp(cmd, "pageTest")==0) { pageTest(); return EMPTY; }
    // pageTitle - draw title page - matrixCOS title
    else if(strcmp(cmd, "pageTitle")==0) { pageTitle(); return EMPTY; }
    // pageEsp - draw esp page - show esp informations 
    else if(strcmp(cmd, "pageEsp")==0) { pageEsp(); return EMPTY; }

    // drawFile file type x y - draw a gif/icon at x,y
    else if(strcmp(cmd, "drawFile")==0) { drawFile(p0,p0,toInt(p1),toInt(p2),false); return EMPTY; }    
    // drawUrl url x y - draw content of url (gif/icon) at a x 
    else if(strcmp(cmd, "drawUrl")==0) { drawUrl(toString(p0),toInt(p1),toInt(p2),false); return EMPTY; }

    /* write wifi icon as wifi.bm1 file 
    else if(strcmp(cmd, "writeIcon")==0) { 
      uint8_t* uint8Ptr = (uint8_t*)reinterpret_cast<const uint8_t*>(wifi_image1bit);
      fsWriteBin("wifi.bm1",uint8Ptr,sizeof(wifi_image1bit)); return EMPTY; 
    } 
    */   
    // drawIcon x y w h c file - draw icon (bm1) at x,y with w,h of color 
    else if(strcmp(cmd, "drawIcon")==0) { drawIcon(toInt(p0),toInt(p1),toInt(p2),toInt(p3),toInt(p4),p5); return EMPTY; }
  
    // drawTime x y c - draw time (format hh:mm:ss) at x,y of color c 
    else if(strcmp(cmd, "drawTime")==0) { drawTime(toInt(p0),toInt(p1),toInt(p2)); return EMPTY; }
    // drawDate x y c - draw date (format dd.mm.yyyy) at x,y of color c
    else if(strcmp(cmd, "drawDate")==0) { drawDate(toInt(p0),toInt(p1),toInt(p2)); return EMPTY; }

    // effect type step speed a b - start effect type with n-steps with speed in ms between steps
    else if(equals(cmd, "effect")) { effectStart(toInt(p0),toInt(p1),toInt(p2),toInt(p3),toInt(p4)); return EMPTY;  } // start effect 

    // matrix sizeX sizeY chain brightness rotation pins - config hub75 dislpay/connection
	  // e.g. matrix 64 64 1 90 0 0,15,4,16,27,17,5,18,19,21,12,33,25,22
    else if(strcmp(cmd, "matrix")==0) { return cmdSetMatrix(p0,p1,p2,p3,p4,p5);  }
    // buffer dmaBuffer displayBuffer - (0=off/1=on) enable dmsBuffer or displayBuffer 
    else if(strcmp(cmd, "buffer")==0) { return cmdSetBuffer(toBoolean(p0),toBoolean(p1));  }
//    else if(strcmp(cmd, "drawEffect")==0) { drawEffect(); return "drawEffect"; }

//    else { return EMPTYSTRING; }
    else { sprintf(buffer,"unkown cmd %s",cmd); return buffer; }
}


