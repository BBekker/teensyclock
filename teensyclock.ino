#include <Time.h>
#include <TimeLib.h>

#include <IntervalTimer.h>
#include <Wire.h>
#include <AccelStepper.h>

#define STEPPER_STEPS_PER_ROTATION 64
#define STEPPER_GEAR_RATIO 25792/405
#define WHEEL_GEAR_RATIO  100/99
#define STEPS_PER_ROTATION STEPPER_STEPS_PER_ROTATION * STEPPER_GEAR_RATIO * WHEEL_GEAR_RATIO

#define SECONDS_PER_ROTATION 60*60*12

#define DS3231_ADDRESS  0x68
#define DS3231_CONTROL  0x0E
#define DS3231_STATUSREG 0x0F


IntervalTimer clocktick;
uint32_t position; //Position in steps
AccelStepper steppermotor = AccelStepper(8, 4, 2, 3, 1);

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

//////////
// Serial Function
/////////


/*
 * Receive a new time over serial
 * Format: yyyymmddhhmmss<newline>
 */
void receiveSerial()
{
  static char buffer[100];
  static int cursor;
  buffer[cursor] = Serial.read();
  if(buffer[cursor] == '\n')
  {
    //If we received exactly enough characters
    Serial.print("received");
    if(cursor == 14)
    {
      Serial.print("matched");
      buffer[cursor] = '\0';
      String stringyfied = String(buffer);
      int _year = stringyfied.substring(0, 4).toInt();
      int _month = stringyfied.substring(4, 6).toInt();
      int _day = stringyfied.substring(6, 8).toInt();
      int _hour = stringyfied.substring(8, 10).toInt();
      int _min = stringyfied.substring(10, 12).toInt();
      int _sec = stringyfied.substring(12, 14).toInt();
      updateRTC(_year,_month,_day,_hour,_min,_sec);
      updateTime();
    }
    cursor = 0;
  }
  else
  {   
    cursor = (cursor + 1)%100;
  }
}


///////
// The actual program is below here
//////



/*
 * Tick
 * Runs once a second to update the time.
 */
void tick()
{
  //Update our time with the real RTC time once a minute.
  if(second() == 0)
  {   
    updateTime();
  }

  //Print current time for debug
  Serial.printf("%d:%d:%d , %d-%d-%d \n", hour(), minute(), second(), day(), month(), year());

  steppermotor.setMaxSpeed(500);
  steppermotor.setAcceleration(1000);
  steppermotor.move(10);
  
}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9800);
  
  //Run tick function once every second
  clocktick.begin(tick, 1000000);

  //Read current time from RTC
  Wire.begin();
  updateTime();
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if(Serial.available())
  {
    receiveSerial();
  }
  
  steppermotor.run();
  //delay(1000);
}
