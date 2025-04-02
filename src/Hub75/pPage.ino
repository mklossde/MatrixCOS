
byte matrixPage=0; // 0=off,1=title
unsigned long *matrixPageTime = new unsigned long(0);

byte _effectType=0;

int col_red;
int col_white;
int col_black;
int col_green;
int col_blue;

/* clear from page */
void pageClear() {
  if(eeDisplay.displayBuffer) { display->fillRect(0,0,panelX,panelY,0); } 
  else { displayClear(); }
  _effectType=0; // effect off
}

void pageTitle() {
  pageClear();

  int wh=panelX/2, hh=panelY/2;

  // WIFI  
  long val=0; if(WiFi.RSSI()!=0) { val=(100+WiFi.RSSI())/2; }

  uint16_t w=30,h=30;
  int x=wh,y=50;
  drawArc(x, y, 3, 90, 60, w, h, 3, col_red);
  drawArc(x, y, 3, 90, 60, w-10, h-10, 3, col_red);
  drawArc(x, y, 3, 90, 60, w-20, h-20, 3, col_red);  
  drawArc(x, y ,3, 90, val, w, h, 3, col_green);
  drawArc(x, y, 3, 90, val, w-10, h-10, 3, col_green);
  drawArc(x, y, 3, 90, val, w-20, h-20, 3, col_green);

  fillTriangle(x, y, x-w, y-h, x-w, y, col_black);
  fillTriangle(x, y, x+w, y-h, x+w, y, col_black);
  fillCircle(x, y+10, 3, col_red);

  // title
  int16_t  x1, y1; 
  display->getTextBounds(prgTitle, 0, 0, &x1, &y1, &w, &h);
  x=wh-w/2, y=1;
  drawText(x,y,1,prgTitle,col_red);   
  // version
  drawText(1,panelY-8,1,prgVersion,col_red);   
  // FreeHeap
  drawFull(panelX-5,25,8,20,2,(int)ESP.getFreeHeap(),150000,col_red,col_white);

  if(eeMode!=EE_MODE_OK) {
    fillRect(panelX-13,panelY-9,panelX,panelY,col_red);
    sprintf(buffer, "%d", eeMode); drawText(panelX-12,panelY-8,1,buffer,col_black);  
  }

  if(is(appIP)) {  
    drawText(1,10,1,(char*)appIP.c_str(),col_red);   
  }

  draw();
//  sprintf(buffer, "drawTitle red:%d green:%d blue:%d  white:%d",col_red,col_green,col_blue,col_white); logPrintln(LOG_DEBUG,buffer);
}

void pageStart() {
  pageClear();  
  int wh=panelX/2, wy=panelY/2, hp=panelY/panelX, ii=0;
  int d=1000/wh;
  for(int i=0;i<wh;i++) {
    drawRect(wh-i,wy-ii,(i*2),(ii*2),col_white);
    ii+=hp;  
    draw();
    delay(d);  
  }
}


/*
void pageTest() {
  pageClear();
  int wh=panelX/2, y=panelY/2, hp=panelY/panelX;
  for (int x=wh; x >=0; x--){ 
    int w=panelX-(x*2);
    int h=panelY-(y*2);
//    drawRect(x, y, w, h,col_white); // white rect
    pageClear();drawLine(x,y,w,h,col_white);
    y-=hp; 
    draw();
    delay(25);
  }
}
*/

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
  int max=fsDirSize(".gif");
  int f=random(0,max);
  char* name=fsFile(".gif",f,0);  
  if(!is(name)) { sprintf(buffer,"pageGif missing %d/%d",f,max);logPrintln(LOG_ERROR,buffer); return ; }
  sprintf(paramBuffer,"%s",name);
  sprintf(buffer,"pageGif %d/%d %s",f,max,to(paramBuffer));logPrintln(LOG_INFO,buffer);
  drawFile(paramBuffer,paramBuffer,0,0,false);
  delay(250);
  drawFileClose();
  int rx=random(-1,2)*5;
  int ry=random(-1,2)*5;
  effectStart(1,64,20,rx,ry);
  delay(1300);
}

int pageCmdNr=0;

void pageCmd() {
  pageClear();  
  int max=fsDirSize(".cmd");
  pageCmdNr++; if(pageCmdNr>=max) { pageCmdNr=0; }
  char* name=fsFile(".cmd",pageCmdNr,0);
  cmdFile(name);
}

//-----------------------------------------------------------

void pageSetup() {
  col_red=toColor444(15,0,0);
  col_white=toColor444(15,15,15);
  col_black=toColor444(0,0,0);  
  col_green=toColor444(0,15,0);  
  col_blue=toColor444(0,0,15);  
    
  pageStart();  
  pageTitle(); matrixPage=1;  
}

void matrixStatus() {
  if(eeMode<=EE_MODE_AP) { drawLine(0,panelY,panelX,panelY,col_blue);} // blue => AP Mode
  else if(eeMode!=EE_MODE_OK) { drawLine(0,panelY,panelX,panelY,col_red);} // red => Mode wrong  
}

void pageLoop() {
  if(!displayEnable && !_displaySetup) { return ; }
  if(matrixPage>0) {
    if(isTimer(matrixPageTime, 1000)) { 
      if(matrixPage==1) { pageTitle(); } // draw title again 
      else if(matrixPage==2) { pageEsp(); } 
      else if(matrixPage==3) { pageStart(); } 
      else if(matrixPage==4) { pageTime(); } 
      else if(matrixPage==5) { pageGif(); } 
      else if(matrixPage==6) { pageCmd(); } 
    }
  }else {
    if(isTimer(matrixPageTime, 1000)) { matrixStatus(); } // draw staus
  }  
}
