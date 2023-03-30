#ifndef UTILS_H
#define UTILS_H

#include "hardwareDefs.h"
#include <SPIFFS.h>

/**
   @brief:
   Checks for the deviceStateEvent set bits.
   @param:
   deviceEvent      bit position
   @return:
   true if bit is set
*/

bool checkSetBit(int state, int setBit) {
  return (state & (1 << setBit));
}


/**
   @brief:
   Checks for the deviceStateEvent set bits.
   @param:
   deviceEvent      bit position
   @return:
   true if bit is set
*/

bool testBit(uint &bits, int bit)
{
  return (bits & bit);
}

/**
   @brief:
   sets the bit.
   @param:
   deviceEvent      bit position
*/
void setBit(uint &bits, int bit)
{
  bits |= bit;
}

/**
   @brief:
   clears the bit.
   @param:
   deviceEvent      bit position
*/

void clearBit(uint &bits, int bit)
{
  bits &= (~bit);
}


#endif
