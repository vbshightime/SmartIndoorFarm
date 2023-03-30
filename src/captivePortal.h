#ifndef CAPTIVE_H_
#define CAPTIVE_H_

#include <pgmspace.h>
#include "deviceState.h"
#include "utils.h"
#include "hardwareDefs.h"
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#define  RESPONSE_LENGTH 200


AsyncWebServer server(80);


char responsePayload[RESPONSE_LENGTH];

char credResponsePayload[RESPONSE_LENGTH];


const char HTML_FORM_PROVISION[] PROGMEM = R"rawliteral(
    <!DOCTYPE HTML><html><head><meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0"><title>Bytebeam</title>
	
    <style>body { background-color: #000000 ; font-family: Arial, Helvetica, Sans-Serif; Color: #FFFFFF; } input[type="submit"]{background-color: #616A6B; border: none;color: white;padding:15px 48px;text-align: center;text-decoration: none;display: inline-block;font-size: 16px;}</style></head>
    <body><center>
        <h1 style="color:#ffffff; font-family:Times New Roman,Times,Serif;padding-top: 10px;padding-bottom: 5px;font-size: 70px;font-style: oblique">Bytebeam</h1>
        <br><label style="color:#FFFFFF;font-family:Times New Roman,Times,Serif;font-size: 24px;padding-top: 5px;padding-bottom: 10px;">Provision your device </label><br><br>
        <FORM action="/provision" method= "POST" enctype="multipart/form-data">
            <P><label style="font-family:Times New Roman">Upload device provisioning JSON file</label><br><br><input type="file" name="data"/><br><br><input type="submit" name="upload" value="Upload" title="Upload File"></P>
        </FORM>
    </center></body></html>
    
)rawliteral";


const char HTML_FORM_CRED[] PROGMEM = R"rawliteral(
    <!DOCTYPE HTML><html><head><meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0"><title>Bytebeam</title>
	
    <style>body { background-color: #000000 ; font-family: Arial, Helvetica, Sans-Serif; Color: #FFFFFF; }input[type=text], select {width: 100%;padding: 12px 20px;margin: 8px 0;display: inline-block;border: 1px solid #ccc;border-radius: 4px;box-sizing: border-box;}</style></head>
    <body><center>
        <h1 style="color:#ffffff; font-family:Times New Roman,Times,Serif;padding-top: 10px;padding-bottom: 5px;font-size: 70px;font-style: oblique">Bytebeam</h1>
        <br><label style="color:#FFFFFF;font-family:Times New Roman,Times,Serif;font-size: 24px;padding-top: 5px;padding-bottom: 10px;">Configure Device Settings</label><br><br>
        <FORM action="/cred" method= "GET">
            <P><label style="font-family:Times New Roman">WiFi SSID</label><br><input maxlength="30px" type = "text" name="ssid" id="ssid" placeholder= "SSID" style="width: 400px; padding: 5px 10px ; margin: 8px 0; border : 2px solid #616A6B; border-radius: 4px; box-sizing:border-box" required;>
            <br><label style="font-family:Times New Roman">WiFi Password</label><br><input maxlength="30px" type = "text" name="pass" id="pass" placeholder= "Password" style="width: 400px; padding: 5px 10px ; margin: 8px 0; border : 2px solid #616A6B; border-radius: 4px; box-sizing:border-box" required;><br>
        </P>
            <INPUT type="submit"><style>input[type="submit"]{background-color: #616A6B; border: none;color: white;padding:15px 48px;text-align: center;text-decoration: none;display: inline-block;font-size: 16px;}</style><br><br>
        </FORM>
    </center></body></html>
)rawliteral";

class ESPCaptivePortal
{
  public:
    // Warning:: requies device state to  be global and longer living
    // than this
    ESPCaptivePortal(DeviceState& devState) {
    }

    /**
       @brief:
       begins the Async WebServer
    */
    void beginServer() {
      DEBUG_PRINTLN("Starting the captive portal. You can configure ESp32 values using portal");
      server.begin();
    }

    /**
       @brief:
       Kills the Async WebServer
    */
    void endPortal() {
      DEBUG_PRINTLN("Ending the captive portal");
      server.reset();
    }

    /**
       @brief:
       Serves the portal
       @param:
       AP active flag
    */
    void servePortal ( bool isAPActive ) {

      server.on("/cred", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (request->params() > 0 && request->hasParam("ssid") && request->hasParam("pass")) {
          String apSSID = request->getParam("ssid")->value();
          String apPass = request->getParam("pass")->value();
          Serial.printf("ssid stored %s\t\n", apSSID.c_str());
          Serial.printf("Pass Stored %s\t\n", apPass.c_str());
          
          snprintf(credResponsePayload, RESPONSE_LENGTH, "{\"apSSID\":%s,\"apPass\":%s}", (apSSID).c_str(), (apPass).c_str());
          request->send(200, "application/json", credResponsePayload);
    }else{
        request->send_P(200, "text/html", HTML_FORM_CRED);
      }
  });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
        request->send_P(200, "text/html", HTML_FORM_PROVISION);
  });

  // run handleUpload function when any file is uploaded
  server.on("/provision", HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(200);
      }, handleUpload);
  
  server.onNotFound(_handleNotFound);
      yield();
  
}

    /**
       @brief:
       Helper funtion for unexpected error
       @param:
       AsyncWebServerRequest
    */
    static void _handleNotFound(AsyncWebServerRequest *request)
    {
      String message = "File Not Found\n\n";
      request->send(404, "text/plain", message);
    }

  /**
       @brief:
       Helper funtion To handle upload
       @param:
       AsyncWebServerRequest
    */
    static void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) 
    {
      String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
      Serial.println(logmessage);

      if (!index) {
          logmessage = "Upload Start: " + String(filename);
          // open the file on first call and store the file handle in the request object
          request->_tempFile = SPIFFS.open("/"+ filename, "w");
          Serial.println(logmessage);
      }

      if (len) {
          // stream the incoming chunk to the opened file
          request->_tempFile.write(data, len);
          logmessage = "Writing file: " + String(filename) + " index=" + String(index) + " len=" + String(len);
          Serial.println(logmessage);
      }

      if (final) {
          logmessage = "Upload Complete: " + String(filename) + ",size: " + String(index + len);
          // close the file handle as the upload is now done
          request->_tempFile.close();
          Serial.println(logmessage);
          request->redirect("/");
        }
      }

};


#endif
