#include <HighPowerSwitchController.h>


HighPowerSwitchController dev;

void setup()
{
  dev.setup();
  dev.startServer();
}

void loop()
{
  dev.update();
}
