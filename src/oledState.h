#ifndef OLED_STATE_H_
#define OLED_STATE_H_

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSerif9pt7b.h>
#include <Fonts/FreeSerif12pt7b.h>
#include <Fonts/FreeSerifBold12pt7b.h>
#include <Fonts/FreeSerifBold9pt7b.h>
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#include <Fonts/FreeMono9pt7b.h>
#include "utils.h"
#include "hardwareDefs.h"
#include "deviceState.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

bool initDisplay()
{
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    DEBUG_PRINTLN("SSD1306 allocation failed");
    setBit(RSTATE.deviceEvents, DSE_DisplayDisconnected);
    return false;
  }
  clearBit(RSTATE.deviceEvents, DSE_DisplayDisconnected);
  return true;
}

bool isDisplayAvailable()
{
  return !testBit(RSTATE.deviceEvents, DSE_DisplayDisconnected);
}

void clearDisplay()
{
  delay(100); //TODO:: don't know why this is needed.
  display.clearDisplay();
  display.setTextColor(WHITE);
}

void drawThermometer(float Value)
{
  display.fillCircle(8, 55, 3, WHITE);  // Draw filled circle (x,y,radius,color). X and Y are the coordinates for the center point

  // Draw rounded rectangle:
  display.drawRoundRect(6, 3, 5, 49, 2, WHITE);  // Draw rounded rectangle (x,y,width,height,radius,color)

  Value = Value * 0.43; //ratio for show
  display.drawLine(8, 46, 8, 46 - Value, WHITE); // Draw line (x0,y0,x1,y1,color)
  display.setFont();
  display.setCursor(110, 20);
  display.setTextSize(1);
  display.cp437(true);
  display.write(167);
  display.setTextSize(2);
  display.print("C");
}

void drawDroplet(float value) {
  display.fillCircle(8, 55, 7, WHITE);  // Draw filled circle (x,y,radius,color). X and Y are the coordinates for the center point
  display.fillTriangle(8, 48, 4, 55, 12,55,WHITE);
}


void drawLine(uint16_t cursorPosition){
  display.fillRect(0, cursorPosition, 128, 3,WHITE);  // Draw rounded rectangle (x,y,width,height,radius,color)
}

void drawAnotation(String annotation, int16_t cursorPositionY, int16_t cursorPositionX )
{   
    display.setFont();
    display.setTextSize(1);
    display.setCursor(cursorPositionX, cursorPositionY);
    display.println(annotation);
}

void drawLoGo(String annotation){
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setFont(&FreeMonoBoldOblique12pt7b);
    display.setCursor(5, 30);
    display.print(annotation);
}

void drawTag(String annotation){
    display.setTextColor(WHITE);
    display.setTextSize(1);
    //display.setFont(&FreeMono9pt7b);
    display.setCursor(32, 50);
    display.print(annotation);
  }
  

void drawIp(String annotation){
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setFont();
    display.setCursor(1, 25);
    display.print(annotation);
  }


void drawValue(String value,uint16_t cursorPositionY, uint16_t cursorPositionX, uint8_t textSize)
{
  display.setFont(&FreeSerifBold9pt7b);
  display.setTextSize(textSize);
  display.setTextColor(WHITE);
  display.setCursor(cursorPositionX, cursorPositionY);
  display.print(value);
}


//void drawDisplay(DisplayMode mode, String string = String())
void drawDisplay(DisplayMode mode)
{
  display.clearDisplay();

  char valStr[20] = {0};
  switch (mode)
  {
    case DisplayTempHumid:
      {
        drawAnotation("Temprature",0,0);
        snprintf(valStr, 20, "%.1f C", RSTATE.temperature);
        drawValue(valStr,25,0,1);
        drawLine(30);
        drawAnotation("Humidity",34,0);
        snprintf(valStr, 20, "%.1f%c", RSTATE.humidity, 37);
        drawValue(valStr,58,0,1);
        
      break;
    case DisplayTempHumidCO2:
      {
        drawAnotation("Temp",0,0);
        snprintf(valStr, 20, "%.1fC", RSTATE.temperature);
        drawValue(valStr,25,0,1);
        drawAnotation("Humid",0,65);
        snprintf(valStr, 20, "%.1f", RSTATE.humidity);
        drawValue(valStr,25,65,1);
        drawLine(30);
        drawAnotation("CO2",34,0);
        snprintf(valStr, 20, "%dPPM", RSTATE.carbon);
        drawValue(valStr,58,0,1);
      }
      break;
    default:
      display.setTextSize(1);
      display.setCursor(20, 0);
      display.print("NO Mode");
      break;
  }
  yield();
  delay(100);
  display.display();
}
}
#endif
