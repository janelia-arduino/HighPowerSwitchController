// ----------------------------------------------------------------------------
// Constants.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "Constants.h"


namespace high_power_switch_controller
{
namespace constants
{
CONSTANT_STRING(device_name,"high_power_switch_controller");

CONSTANT_STRING(firmware_name,"HighPowerSwitchController");
// Use semantic versioning http://semver.org/
const modular_server::FirmwareInfo firmware_info =
  {
    .name_ptr=&firmware_name,
    .version_major=1,
    .version_minor=0,
    .version_patch=0,
  };

CONSTANT_STRING(hardware_name,"high_power_switch_controller");

// Interrupts

// Units

// Properties

// Parameters
CONSTANT_STRING(channel_parameter_name,"channel");

// Functions
CONSTANT_STRING(enable_all_function_name,"enableAll");
CONSTANT_STRING(disable_all_function_name,"disableAll");
CONSTANT_STRING(enabled_function_name,"enabled");
CONSTANT_STRING(set_channel_on_function_name,"setChannelOn");
CONSTANT_STRING(set_channel_off_function_name,"setChannelOff");

// Callbacks

// Errors
}
}
