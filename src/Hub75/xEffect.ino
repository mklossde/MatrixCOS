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
    for(int x=0;x<eeMatrix.panelX;x++) {
      for(int y=0;y<eeMatrix.panelY;y++) {
        GFXcanvas16 *canvas=(GFXcanvas16*)display;
        int px=x, py=y; uint16_t color=0;
        if(_effectA<0) { px=eeMatrix.panelX-x-1; }        
        if(_effectB<0) { py=eeMatrix.panelY-y-1; }
        int posX, posY;
        if(_effectA>=5) { posX=px+random(1,_effectA); }        
        else if(_effectA<=-5) { posX=px-random(1,(_effectA*-1)); }    
        else { posX=px+_effectA; }
        if(_effectB>=5) { posY=py+random(1,_effectB); } 
        else if(_effectB<=-5) { posY=py-random(1,(_effectB*-1)); } 
        else { posY=py+_effectB; }
        if(posX>=0 && posX<eeMatrix.panelX && posY>=0 && posY<eeMatrix.panelY) { color= canvas->getPixel(posX,posY); }
        canvas->drawPixel(px,py,color);
      }
    }
    draw();
}

void effectLoop() {
  if(eeMatrix.displayBuffer && _effectType!=0 && _effectIndex<_effectStep && isTimer(effectTimer, _effectSpeed)) { 
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
