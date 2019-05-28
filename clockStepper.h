#include <AccelStepper.h>

#define ROTATION_PER_STEP 0.000048003699327057935052324954148236055669435753587226237

class ClockStepper : public AccelStepper 
{
public:
  double angle;

	ClockStepper(uint8_t interface = AccelStepper::FULL4WIRE, uint8_t pin1 = 2, uint8_t pin2 = 3, uint8_t pin3 = 4, uint8_t pin4 = 5, bool enable = true)
	: AccelStepper(interface,pin1,pin2,pin3, pin4, enable)
	{
    this->setPinsInverted(true,true,true,true,true);
	}

private:
  long previousStep;
  void step3(long step);
};
