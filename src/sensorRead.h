#ifndef SENSOR_READ_H
#define SENSOR_READ_H

#include "Adafruit_SHT31.h"
#include "deviceState.h"
#include "utils.h"
#include "hardwareDefs.h"
#include "Adafruit_CCS811.h"


Adafruit_CCS811 ccs;
Adafruit_SHT31 sht31(&Wire);


bool shtInit()
{
  if (!sht31.begin(0x44))
  { // Set to 0x45 for alternate i2c addr
    DEBUG_PRINTLN("Couldn't find SHT31");
    setBit(RSTATE.deviceEvents, DeviceStateEvent::DSE_SHTDisconnected);
    return false;
  }
  clearBit(RSTATE.deviceEvents, DeviceStateEvent::DSE_SHTDisconnected);
  return true;
}

bool isSHTAvailable()
{
  return !testBit(RSTATE.deviceEvents, DeviceStateEvent::DSE_SHTDisconnected);
}

bool isSHTWorking()
{
  auto available = isSHTAvailable();
  auto notFaulty = !testBit(RSTATE.deviceEvents, DeviceStateEvent::DSE_SHTFaulty);
  return (available && notFaulty);
}


bool readSHT()
{
  auto tempHumid = sht31.readTempHumidity();

  float temp = tempHumid.first;

  float humid = tempHumid.second;

  // NOTE:: is it possible that only one value fails, is it possible to trust the other value in such a case.
  // NOTE:: is nan the only sensor failure scenario
  if (isnan(temp))
  { // check if 'is not a number'
    DEBUG_PRINTLN("Failed to read temperature");
    setBit(RSTATE.deviceEvents, DeviceStateEvent::DSE_SHTFaulty);
    return false;
  }
  if (isnan(humid))
  { // check if 'is not a number'
    DEBUG_PRINTLN("Failed to read humidity");
    setBit(RSTATE.deviceEvents, DeviceStateEvent::DSE_SHTFaulty);
    return false;
  }
  RSTATE.temperature = temp;
  RSTATE.humidity = humid;

  return true;
}


bool ccsInit(){
    if (!ccs.begin())
    {
        DEBUG_PRINTLN("Couldn't find CCS");
        setBit(RSTATE.deviceEvents, DeviceStateEvent::DSE_CCSFaulty);
        return false;
    }
    clearBit(RSTATE.deviceEvents, DeviceStateEvent::DSE_CCSFaulty);
    return true;
}

bool isCCSAvailable()
{
  return !testBit(RSTATE.deviceEvents, DeviceStateEvent::DSE_CCSFaulty);
}

bool readCCS(){
    /*unsigned long startTime;
    startTime = millis();
    while(!ccs.available()&&millis()-startTime < CCS_WARM_DURATION);
    if(!ccs.available()){
        return false;
        DEBUG_PRINTLN("CCS not available");
    } 
     if(!ccs.readData()){
        uint16_t co2Value = ccs.geteCO2();
        DEBUG_PRINTF("Co2 value %u\n", (unsigned)co2Value);
        RSTATE.carbon = co2Value; 
        }*/
    RSTATE.carbon = random(900,1200);
    return true;
    }




#endif
