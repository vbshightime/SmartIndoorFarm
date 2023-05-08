#ifndef SENSOR_READ_H
#define SENSOR_READ_H

#include "Adafruit_SHT31.h"
#include "deviceState.h"
#include "utils.h"
#include "hardwareDefs.h"
#include "SparkFun_SCD4x_Arduino_Library.h" 


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


bool scdInit(SCD4x *scd_sensor)
{
  if (!scd_sensor->begin())
  {
    DEBUG_PRINTLN("Couldn't find SCD");
    setBit(RSTATE.deviceEvents, DeviceStateEvent::DSE_GASFaulty);
    return false;
  }
  clearBit(RSTATE.deviceEvents, DeviceStateEvent::DSE_GASFaulty);
  return true;
}

bool isSCDAvailable(){
    return !testBit(RSTATE.deviceEvents, DeviceStateEvent::DSE_GASFaulty);  
}

bool readSCD(SCD4x *scd_sensor){
  if (scd_sensor->readMeasurement()) // readMeasurement will return true when fresh data is available
  { 
    RSTATE.carbon = scd_sensor->getCO2();
    return true;
  }
  DEBUG_PRINTLN("Failed to get CO2 Values");
  return false;
}




#endif
