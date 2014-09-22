#ifndef LTC2400_h
#define LTC2400_h

#include <stdio.h>
#include <stdlib.h>
#include <Arduino.h>

#ifndef cbi
#define cbi(sfr, bit)     (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit)     (_SFR_BYTE(sfr) |= _BV(bit))
#endif

class LTC2400
{
 private:
  // Don't really know what these are.
  // LTC2400 Chip Select Pin  on Portb 2
  // LTC2400 SDO Select Pin  on Portb 4
  // LTC2400 SCK Select Pin  on Portb 5
  uint8_t ltc_cs;
  uint8_t ltc_miso;
  uint8_t ltc_sck;

  byte SPI_read();
  void printFloat(float value, int places);

 public:
  LTC2400(uint8_t ltc_cs=2, uint8_t ltc_miso=4, uint8_t ltc_sck=5);
  void init();
  void read();
};

#endif
