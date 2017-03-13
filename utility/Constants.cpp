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

const long analog_write_min = 0;
const long analog_write_max = 255;
// BTS3256D max switching frequency = 12kHz
// PWM frequency = 488.28Hz
// 12kHz/488.28 = 24.576 -> 24
const long channel_pwm_min = 0;
const long channel_pwm_max = 24;

const int bad_index = -1;

// Interrupts

// Units
CONSTANT_STRING(ms_unit,"ms");

// Properties
CONSTANT_STRING(power_max_property_name,"powerMax");

// Parameters
CONSTANT_STRING(channel_parameter_name,"channel");

CONSTANT_STRING(channels_parameter_name,"channels");

CONSTANT_STRING(power_parameter_name,"power");
const long power_min = 0;
const long power_max = 100;

CONSTANT_STRING(powers_parameter_name,"powers");

CONSTANT_STRING(delay_parameter_name,"delay");
const long delay_min = 0;
const long delay_max = 2000000000;

CONSTANT_STRING(period_parameter_name,"period");
const long period_min = 2;
const long period_max = 2000000000;

CONSTANT_STRING(on_duration_parameter_name,"on_duration");
const long on_duration_min = 1;
const long on_duration_max = 2000000000;

CONSTANT_STRING(count_parameter_name,"count");
const long count_min = 1;
const long count_max = 2000000000;

CONSTANT_STRING(pwm_index_parameter_name,"pwm_index");

// Functions
CONSTANT_STRING(enable_all_function_name,"enableAll");
CONSTANT_STRING(disable_all_function_name,"disableAll");
CONSTANT_STRING(enabled_function_name,"enabled");
CONSTANT_STRING(set_power_when_on_function_name,"setPowerWhenOn");
CONSTANT_STRING(set_powers_when_on_function_name,"setPowersWhenOn");
CONSTANT_STRING(set_all_powers_when_on_function_name,"setAllPowersWhenOn");
CONSTANT_STRING(get_powers_when_on_function_name,"getPowersWhenOn");
CONSTANT_STRING(get_powers_function_name,"getPowers");
CONSTANT_STRING(set_channel_on_function_name,"setChannelOn");
CONSTANT_STRING(set_channel_off_function_name,"setChannelOff");
CONSTANT_STRING(set_channels_on_function_name,"setChannelsOn");
CONSTANT_STRING(set_channels_off_function_name,"setChannelsOff");
CONSTANT_STRING(toggle_channel_function_name,"toggleChannel");
CONSTANT_STRING(toggle_channels_function_name,"toggleChannels");
CONSTANT_STRING(toggle_all_channels_function_name,"toggleAllChannels");
CONSTANT_STRING(set_all_channels_on_function_name,"setAllChannelsOn");
CONSTANT_STRING(set_all_channels_off_function_name,"setAllChannelsOff");
CONSTANT_STRING(set_channel_on_all_others_off_function_name,"setChannelOnAllOthersOff");
CONSTANT_STRING(set_channel_off_all_others_on_function_name,"setChannelOffAllOthersOn");
CONSTANT_STRING(set_channels_on_all_others_off_function_name,"setChannelsOnAllOthersOff");
CONSTANT_STRING(set_channels_off_all_others_on_function_name,"setChannelsOffAllOthersOn");
CONSTANT_STRING(channel_is_on_function_name,"channelIsOn");
CONSTANT_STRING(get_channels_on_function_name,"getChannelsOn");
CONSTANT_STRING(get_channels_off_function_name,"getChannelsOff");
CONSTANT_STRING(get_channel_count_function_name,"getChannelCount");
CONSTANT_STRING(add_pwm_function_name,"addPwm");
CONSTANT_STRING(start_pwm_function_name,"startPwm");
CONSTANT_STRING(stop_pwm_function_name,"stopPwm");
CONSTANT_STRING(stop_all_pwm_function_name,"stopAllPwm");

// Callbacks

// Errors
CONSTANT_STRING(pwm_error,"Maximum number of pwm already set, must stop one to add another.");
}
}
