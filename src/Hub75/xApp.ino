

void appSetup() {
  if(serialEnable) { 
    delay(1); Serial.begin(115200); 
    delay(1); Serial.println("----------------------------------------------------------------------------------------------------------------------");
  }
  eeSetup();
  ledSetup();  
  swSetup(); 
  bootSetup();
  
  fsSetup();

  if(eeMode<EE_MODE_SYSERROR) {
    wifiSetup();  
    otaSetup();
  }

  if(eeMode>EE_MODE_AP && eeMode<EE_MODE_ERROR) {
    mqttSetup();  
    matrixSetup();
    timeSteup();  
  }
}

void appLoop() {
  eeLoop();
  ledLoop(); 
  swLoop(); 
  cmdLoop();

  timeLoop();

  if(eeMode<EE_MODE_SYSERROR) {
    wifiLoop();
    otaLoop();
  }

  if(eeMode>EE_MODE_AP && eeMode<EE_MODE_ERROR) {
    mqttLoop();
    webLoop();

    drawLoop();
    matrixLoop();
    effectLoop();
    timeLoop();
//    eventLoop(); 
  }
  delay(0);
}