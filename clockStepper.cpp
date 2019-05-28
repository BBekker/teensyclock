#include <AccelStepper.h>
#include "ClockStepper.h"

void ClockStepper::step3(long step)
{
  if(step - previousStep > 0)
  {
    angle += ROTATION_PER_STEP;
  } else if(step-previousStep < 0)
  {
    angle -= ROTATION_PER_STEP;
  }  

  if(angle > 1.0)
    angle -= 1.0;
  if(angle < 0.0)
    angle += 1.0;
  
	this->setOutputPins((1 << (step%4)));
}
