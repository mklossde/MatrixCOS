
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
    drawClear();
    _playGif=false;

    x_offset=x; y_offset=y;
    if (x_offset < 0) { x_offset = 0;}
    if (y_offset < 0) { y_offset = 0; }

    if(!startWith(name,"/")) { sprintf(buffer,"/%s",name); name=buffer; }
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
  matrixPage=0;
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

