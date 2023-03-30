#include "deviceState.h"
#include "captivePortal.h"
#include "hardwareDefs.h"
#include "sensorRead.h"
#include "utils.h"
#include <Wire.h>
#include "WiFiOTA.h"
#include <rom/rtc.h>
#include <esp_wifi.h>
#include "oledState.h"
#include <BytebeamArduino.h>

DeviceState state;
DeviceState &deviceState = state;
ESPCaptivePortal captivePortal(deviceState);

// sntp credentials
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 3600;
const char* ntpServer = "pool.ntp.org";

// on board led pin number
#define BOARD_LED 2

// led state variable
int ledState = 0;


void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin();
  if (!EEPROM.begin(EEPROM_STORE_SIZE))
  {
    DEBUG_PRINTLN("Problem loading EEPROM");
  }

  if (!SPIFFS.begin(true)) {
    Serial.println("spiffs mount failed");
    return;
  } else {
    Serial.println("spiffs mount success");
  }

  bool rc = deviceState.load();
  if (!rc)
  {
    DEBUG_PRINTLN("EEPROM Values not loaded");
  }
  else
  {
    DEBUG_PRINTLN("Values Loaded");
  }
#ifdef OLED_DISPLAY
  initDisplay();
  clearDisplay();
#endif
  DEBUG_PRINTF("The reset reason is %d\n", (int)rtc_get_reset_reason(0));
  if ( ((int)rtc_get_reset_reason(0) == 12) || ((int)rtc_get_reset_reason(0) == 1))  { // =  SW_CPU_RESET
    RSTATE.isPortalActive  = true;
    if (!APConnection(AP_MODE_SSID)) {
      DEBUG_PRINTLN("Error Setting Up AP Connection");
      return;
    }
    delay(100);
    captivePortal.servePortal(true);
    captivePortal.beginServer();
    delay(100);
  }

  shtInit();
  ccsInit();


}

// function for ToggleLED action
int toggleLed(char* args, char* actionId) {
  Serial.printf("*** args : %s , actionId : %s ***\n", args, actionId);
  if(RSTATE.isBlindsOpen ? Serial.println("Blinds are open"): Serial.println("Blinds are closed"));
  RSTATE.isBlindsOpen = !RSTATE.isBlindsOpen;
  
  Bytebeam.publishActionCompleted(actionId);
  return 0;
}

// function to setup the predefined led 
void setupLED() {
  pinMode(BOARD_LED, OUTPUT);
  digitalWrite(BOARD_LED, ledState);
}



void loop()
{
  if (!RSTATE.isPortalActive)
  {
    if (!reconnectWiFi((PSTATE.apSSID).c_str(), (PSTATE.apPass).c_str(), 300))
    {
      DEBUG_PRINTLN("Error connecting to WiFi, Trying again");
    }
    if(WiFi.isConnected() && !RSTATE.isBytebeamBegin){
        syncTimeFromNtp();
         Bytebeam.begin();
         Bytebeam.addActionHandler(toggleLed, "toggle_blind");
         RSTATE.isBytebeamBegin = true;
      }
    Bytebeam.loop();
    publishToDeviceShadow();
    delay(2000);
}

  

  if (millis() - RSTATE.startPortal >= SECS_PORTAL_WAIT * MILLI_SECS_MULTIPLIER && RSTATE.isPortalActive)
  {
    reconnectWiFi((PSTATE.apSSID).c_str(), (PSTATE.apPass).c_str(), 300); 
    RSTATE.isPortalActive = false;
  }
}

unsigned long long getEpochTime() {
  const long  gmtOffset_sec = 19800;
  const int   daylightOffset_sec = 3600;
  const char* ntpServer = "pool.ntp.org";

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return (0);
  }
  time(&now);

  unsigned long long time = ((unsigned long long)now * 1000) + (millis() % 1000);
  return time;
}

void syncTimeFromNtp() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);                    // set the ntp server and offset

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {                                              // sync the current time from ntp server
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.println();
}

void publishToDeviceShadow() {
  static int sequence = 0;
  const char* payload = "";
  String deviceShadowStr = "";
  StaticJsonDocument<1024> doc;

  char deviceShadowStream[] = "device_shadow";

  sequence++;
  unsigned long long milliseconds = getEpochTime();

  JsonArray deviceShadowJsonArray = doc.to<JsonArray>();
  JsonObject deviceShadowJsonObj_1 = deviceShadowJsonArray.createNestedObject();
  DEBUG_PRINTLN(milliseconds);

  deviceShadowJsonObj_1["timestamp"] = milliseconds;
  deviceShadowJsonObj_1["sequence"]  = sequence;
  if (RSTATE.isBlindsOpen) {
    deviceShadowJsonObj_1["Status"]    = "Blinds are open";
  }else{
    deviceShadowJsonObj_1["Status"]    = "Blinds are closed";
  }
  deviceShadowJsonObj_1["temperature"]  = (random(70, 89))/10;
  deviceShadowJsonObj_1["humidity"]  = 74;
  deviceShadowJsonObj_1["carbon"]  = random(400, 450);

  serializeJson(deviceShadowJsonArray, deviceShadowStr);
  payload = deviceShadowStr.c_str();
  Serial.printf("publishing %s to %s\n", payload, deviceShadowStream);
  Bytebeam.publishToStream(deviceShadowStream, payload);
}
