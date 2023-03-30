#ifndef WIFI_OTA_H
#define WIFI_OTA_H
#include "hardwareDefs.h"
#include <SPIFFS.h>
#include <Update.h>
#include "deviceState.h"


const IPAddress apIP(192, 168, 4, 1);
const IPAddress netMsk(255, 255, 255, 0);

/**
   @brief:
   Function to connect to WiFi to wifi if already connected returns true
   if not then check it the station connected to AP or in portal mode and return true
   or else connectes to the WiFiSTA if not connected
   @param:
   SSID     PassKey     maxDelayRetry       AP-SSID
   @return:
   true if connected to STA or AP
   false in case of faliure
*/


bool reconnectWiFi(const String& ssid, const String& pass, int maxDelay) {
  if (RSTATE.isPortalActive) {
    return true;
  }
  bool connectSuccess = true;
  int loopCounter = 0;
  if (WiFi.isConnected()) {
    DEBUG_PRINTLN("already in STA mode and is connected");
    RSTATE.isAPActive = false;
    return true;
  }
  WiFi.mode(WIFI_STA);  // fixem:: why AP STA mandeep had it like this.
  DEBUG_PRINTF("ssid: %s", ssid.c_str());
  DEBUG_PRINTF("pass: %s", pass.c_str());
  WiFi.begin(ssid.c_str(), pass.c_str());
  DEBUG_PRINTLN("staring wait for connection\n");
  while (WiFi.status() != WL_CONNECTED)
  {
    DEBUG_PRINT(".");
    delay(maxDelay);
    if (loopCounter == 30) {
      DEBUG_PRINTLN("timeout trying to connect to wifi\n");
      connectSuccess = false;
      break;
    }
    loopCounter++;
  }
  connectSuccess = WiFi.isConnected();
  if (connectSuccess) {
    DEBUG_PRINTLN("connected to:  ");
    DEBUG_PRINTLN(WiFi.localIP());
    RSTATE.isAPActive = false;
  }
  return connectSuccess;
}


String getLast3ByteMac(uint8_t* mac, bool fullSizeMac){
  char macStr[9] = { 0 };
  WiFi.macAddress(mac);
  if(!fullSizeMac){
    sprintf(macStr, "%02X%02X%02X", mac[3], mac[4], mac[5]);
  }else{
    sprintf(macStr, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2],mac[3], mac[4], mac[5]);
  }
  return String(macStr);
}


bool APConnection(const String& APssid) {
  WiFi.disconnect();
  uint8_t mac[6];
  String macStr = getLast3ByteMac(mac,false);
  if (WiFi.softAPgetStationNum() > 0 && PSTATE.isOtaAvailable == 0) {
    DEBUG_PRINTLN("Station Connected to SoftAP, keeping soft AP alive");
    RSTATE.isAPActive = true;
    return true;
  }
  WiFi.mode(WIFI_AP);
  if (!WiFi.softAP((APssid+macStr).c_str())) {
    return false;
  }
  delay(100);
  if (!WiFi.softAPConfig(apIP, apIP, netMsk)) {
    return false;
  }
  delay(100);
  DEBUG_PRINT("Connected to: ");
  Serial.println(WiFi.softAPIP());
  RSTATE.isAPActive = true;
  return true;
}


/**
   @brief:
   get the macAddress without colons for device Id
   @return:
   String to be used as a device Id
*/

String macAddrWithoutColons()
{
  uint8_t mac[6];
  WiFi.macAddress(mac);
  char macStr[13] = { 0 };
  sprintf(macStr, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(macStr);
}


#endif
