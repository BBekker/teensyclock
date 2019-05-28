#include <Time.h>
#include <TimeLib.h>

///////////
// RTC FUNCTIONS
//////////

byte bcd2byte(byte bcd)
{
  return (bcd & 0x0F) + ((bcd >> 4) *10);
}

byte bin2bcd(byte val)
{
  return (val % 10) + ((val / 10) << 4);
}

//Update the external RTC
void updateRTC(unsigned int _year,unsigned int _month,unsigned int _day,unsigned int _hour,unsigned int _min,unsigned int _sec)
{
  Wire.beginTransmission(DS3231_ADDRESS);
  Wire.write((byte)0); // start at location 0
  Wire.write(bin2bcd(_sec));
  Wire.write(bin2bcd(_min));
  Wire.write(bin2bcd(_hour));
  Wire.write(bin2bcd(0));
  Wire.write(bin2bcd(_day));
  Wire.write(bin2bcd(_month));
  Wire.write(bin2bcd(_year - 2000));
  Wire.endTransmission();
}

//Update the internal clock with the external RTC
void updateTime()
{
  Wire.beginTransmission(DS3231_ADDRESS);
  Wire.write((byte)0);  
  Wire.endTransmission();

  Wire.requestFrom(DS3231_ADDRESS, 7);
  uint8_t ss = bcd2byte(Wire.read() & 0x7F);
  uint8_t mm = bcd2byte(Wire.read());
  uint8_t hh = bcd2byte(Wire.read());
  Wire.read();
  uint8_t d = bcd2byte(Wire.read());
  uint8_t m = bcd2byte(Wire.read());
  uint16_t y = bcd2byte(Wire.read()) + 2000;

  setTime(hh, mm, ss, d, m,y);
  //Get DS3231 time
  //setTime(
}
