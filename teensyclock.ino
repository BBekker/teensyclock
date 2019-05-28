#include <Time.h>

#include <IntervalTimer.h>
#include <Wire.h>
#include "clockStepper.h"

#define STEPPER_STEPS_PER_ROTATION 64
#define STEPPER_GEAR_RATIO 25792/405
#define WHEEL_GEAR_RATIO  100/99
#define STEPS_PER_ROTATION STEPPER_STEPS_PER_ROTATION * STEPPER_GEAR_RATIO * WHEEL_GEAR_RATIO

#define ROTATION_PER_STEP 0.000048003699327057935052324954148236055669435753587226237
#define ROTATION_PER_SECOND ( 1.0 / 12.0 / 60.0 / 60.0)
#define ROTATION_PER_MINUTE ( 1.0 / 12.0 / 60.0 )
#define ROTATION_PER_HOUR (1.0 / 12.0)


#define DS3231_ADDRESS  0x68
#define DS3231_CONTROL  0x0E
#define DS3231_STATUSREG 0x0F

#define ANGLE_OFFSET 0.0

#define HALL_PIN 23
#define LED_PIN 13


IntervalTimer clocktick;
double target;
ClockStepper steppermotor = ClockStepper(8, 12, 10, 11, 9);


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

void homing()
{
  //Low when sensing magnet
  while(digitalRead(HALL_PIN))
  {
    steppermotor.move(1);
    steppermotor.run();
  }
  steppermotor.angle = ANGLE_OFFSET;
}

/*
 * Tick
 * Runs once a second to update the time.
 */
void tick()
{
  target = hour() * ROTATION_PER_HOUR + minute() * ROTATION_PER_MINUTE + second() * ROTATION_PER_SECOND;
  uint32_t steps_to_set = 0;
  // Always move forward to reach target
  if(target - steppermotor.angle >= 0)
  { 
    steps_to_set = (target - steppermotor.angle) / ROTATION_PER_STEP;
  }
  else
  {
    steps_to_set = (target - (steppermotor.angle-1.0)) / ROTATION_PER_STEP;
  }

  
  //Print current time for debug
  Serial.printf("%d:%d:%d , %d-%d-%d \n", hour(), minute(), second(), day(), month(), year());

  steppermotor.move(steps_to_set);

  //Update our time with the real RTC time once a minute.
  if(second() == 0)
  {   
    updateTime();
  }
  
}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9800);
  


  //Read current time from RTC
  Wire.begin();
  updateTime();

  //Prep hall sensor
  pinMode(HALL_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  //Find zero point
  steppermotor.setMaxSpeed(500);
  steppermotor.setAcceleration(1000);
  homing();

  //Start clock
  //Run tick function once every second
  clocktick.begin(tick, 1000000);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if(Serial.available())
  {
    receiveSerial();
  }
  
  steppermotor.run();

  digitalWrite(LED_PIN, !digitalRead(HALL_PIN));
  //delay(1000);
}
