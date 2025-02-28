
// MatrixCOS - Esp32 HUB75 Matrix 64x64 based on CmdOS
// CmdOS - http://github.com/mklossde/CmdOs

#include <ESPAsyncWebServer.h>
#include <AnimatedGIF.h>
 
const char *prgTitle = "MatrixCOS";
const char *prgVersion = "V1.1.1";

const char* user_admin = "admin"; // default user
char user_pas[]="";   // default espPas

const char *wifi_ssid_default = ""; // PRIVAT_WIFI_SSID; // define in privatdata.h 
const char *wifi_pas_default = ""; //PRIVAT_WIFI_PAS;   // define in privatdata.h 
const char *mqtt_default = ""; //PRIVAT_MQTTSERVER;     // define in privatdata.h 

byte MODE_DEFAULT=21; // MODE_WIFI_CL_TRY=21 / MODE_PRIVAT


boolean serialEnable=true; // enable/disbale serial log 
boolean cmdEnable=true; // enable/disbale serial log 

boolean wifiEnable=true;  // enable/disbale wifi
boolean ntpEnable=true; // enable time server
boolean webEnable=true;    // enable/disbale http server
boolean mdnsEnable=true;   // enable/disable mDNS detection 
boolean bootSafe=false;    // enable/disbale boot safe

#define enableFs true         // enable fs / SPIFFS

#define netEnable true       // enable/disbale network ping/dns/HttpCLient 
#define webSerialEnable false // enable/disbale web serial
#define mqttEnable true      // enable/disbale mqtt

#define otaEnable false        // enabled/disbale ota update 
#define updateEnable false     // enabled/disbale update firmware via web 

#define ledEnable false       // enable/disbale serial
int ledGpio=15;            // io of led
boolean ledOnTrue=true;           // gpio false=led-on

#define swEnable false        // enable/disbale switch
int swGpio=0;                // io pin of sw 
int swTimeBase=100;       // prell and lonePress Timebase (e.g. 100ms)
boolean swOnTrue=false;      // gpio false=sw-pressed
boolean swPullUp=true;      // enable pullUp
int swSetupTime=5000;     // 0=sw is init during setzp, >0 sw is init after swSetupTime

// app
boolean matrixEnable=true; // enable matrix


int _webPort = 80;
AsyncWebServer server(_webPort);

//--------------------------------------------------------------


char* appCmd(char *cmd, char **param) {
    return matrixCmd(cmd,param); 
}

//--------------------------------------------------------------


/* callback to add app web pages */
void webApp() {
  server.on("/app", HTTP_GET, [](AsyncWebServerRequest *request) { matrixWeb(request); });
  server.on("/appSetup", HTTP_GET, [](AsyncWebServerRequest *request) { matrixWebSetup(request); });

  server.on(
        "/drawUpload", HTTP_POST,
        [](AsyncWebServerRequest *request) {},[](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data,size_t len, bool final) {
          drawUpload(request, filename, index, data, len, final);
        })
//TODO  .addMiddleware(&basicAuth)
    ;
}

//--------------------------------------------------------------

void setup() {
  cmdOSSetup();
  if(isModeNoError()) { 
    matrixSetup();
    swCmd(1,"pageNext"); // nextPage on one sw click
    swCmd(2,"pageCmd"); // nextCmd on two sw click
    swCmd(10,"drawOff"); // drawOff on two sw click
  }  
}

void loop() {
  cmdOSLoop();
  if(isModeNoError()) { 
    drawLoop();
    matrixLoop();
    effectLoop();
  }  
}
