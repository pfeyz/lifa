#include "LTC2400.h"

LTC2400::LTC2400(uint8_t ltc_cs, uint8_t ltc_miso, uint8_t ltc_sck)
{
  this->ltc_cs = ltc_cs;
  this->ltc_miso = ltc_miso;
  this->ltc_sck = ltc_sck;
};

void LTC2400::init()
{
  cbi(PORTB, this->ltc_sck);      // LTC2400 SCK low
  sbi (DDRB, this->ltc_cs);       // LTC2400 CS HIGH

  cbi (DDRB, this->ltc_miso);
  sbi (DDRB, this->ltc_sck);

  // init SPI Hardware
  sbi(SPCR, MSTR); // SPI master mode
  sbi(SPCR, SPR0); // SPI speed
  sbi(SPCR, SPR1);  // SPI speed
  sbi(SPCR, SPE);   //SPI enable
}

unsigned int LTC2400::read()
{
  float volt;
  float v_ref=4.094;          // Reference Voltage, 5.0 Volt for LT1021 or 3.0 for LP2950-3

  unsigned int ltw = 0;         // ADC Data ling int
  int cnt;                  // counter
  byte b0;                  //
  byte sig;                 // sign bit flag
  char st1[20];             // float voltage text

  cbi(PORTB,this->ltc_cs);             // LTC2400 CS Low
  delayMicroseconds(1);
  if (!(PINB & (1 << 4))) {    // ADC Converter ready ?
    //    cli();
    ltw=0;
    sig=0;

    b0 = this->SPI_read();             // read 4 bytes adc raw data with SPI
    if ((b0 & 0x20) ==0) sig=1;  // is input negative ?
    b0 &=0x1F;                   // discard bit 25..31
    ltw |= b0;
    ltw <<= 8;
    b0 = this->SPI_read();
    ltw |= b0;
    ltw <<= 8;
    b0 = this->SPI_read();
    ltw |= b0;
    ltw <<= 8;
    b0 = this->SPI_read();
    ltw |= b0;

    delayMicroseconds(1);

    sbi(PORTB,this->ltc_cs);           // LTC2400 CS Low

    if (sig) ltw |= 0xf0000000;    // if input negative insert sign bit
    ltw=ltw/16;                    // scale result down , last 4 bits have no information
    volt = ltw * v_ref / 16777216; // max scale
    char tmp[10];
    dtostrf(volt,6,6,tmp);
    tmp[8]='V';
    tmp[9]='\n';
  }
  sbi(PORTB,this->ltc_cs); // LTC2400 CS hi
  delay(20);
  return ltw;
}

byte LTC2400::SPI_read()
{
  SPDR = 0;
  while (!(SPSR & (1 << SPIF))) ; /* Wait for SPI shift out done */
  return SPDR;
}

void LTC2400::printFloat(float value, int places) {
 // this is used to cast digits
 int digit;
 float tens = 0.1;
 int tenscount = 0;
 int i;
 float tempfloat = value;

 // if value is negative, set tempfloat to the abs value

   // make sure we round properly. this could use pow from
 //<math.h>, but doesn't seem worth the import
 // if this rounding step isn't here, the value  54.321 prints as

 // calculate rounding term d:   0.5/pow(10,places)
 float d = 0.5;
 if (value < 0)
   d *= -1.0;
 // divide by ten for each decimal place
 for (i = 0; i < places; i++)
   d/= 10.0;
 // this small addition, combined with truncation will round our

 tempfloat +=  d;

 if (value < 0)
   tempfloat *= -1.0;
 while ((tens * 10.0) <= tempfloat) {
   tens *= 10.0;
   tenscount += 1;
 }

 // write out the negative if needed
 if (value < 0)
   Serial.print('-');

 if (tenscount == 0)
   Serial.print(0, DEC);

 for (i=0; i< tenscount; i++) {
   digit = (int) (tempfloat/tens);
   Serial.print(digit, DEC);
   tempfloat = tempfloat - ((float)digit * tens);
   tens /= 10.0;
 }

 // if no places after decimal, stop now and return
 if (places <= 0)
   return;

 // otherwise, write the point and continue on
 Serial.print(',');

 for (i = 0; i < places; i++) {
   tempfloat *= 10.0;
   digit = (int) tempfloat;
   Serial.print(digit,DEC);
   // once written, subtract off that digit
   tempfloat = tempfloat - (float) digit;
 }
}
