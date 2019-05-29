#include <AccelStepper.h>
#include "ClockStepper.h"

void ClockStepper::step4(long step)
{
  if(step%4  ==  ((previousStep + 1)%4))
  {
    angle += ROTATION_PER_STEP;
  } else if(step%4  ==  ((previousStep - 1)%4))
  {
    angle -= ROTATION_PER_STEP;
  } else
  {
    Serial.printf("step %d Missing step! \n", step);
  }
  previousStep = step;
  
  if(angle > 1.0)
    angle -= 1.0;
  if(angle < 0.0)
    angle += 1.0;
  
	this->setOutputPins((1 << (step%4)));
  //Serial.print(".");
}
