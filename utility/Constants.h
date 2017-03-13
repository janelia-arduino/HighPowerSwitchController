// ----------------------------------------------------------------------------
// Constants.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef HIGH_POWER_SWITCH_CONTROLLER_CONSTANTS_H
#define HIGH_POWER_SWITCH_CONTROLLER_CONSTANTS_H
#include "ConstantVariable.h"
#include "ModularServer.h"

#include "EventController.h"


namespace high_power_switch_controller
{
namespace constants
{
//MAX values must be >= 1, >= created/copied count, < RAM limit
enum{PROPERTY_COUNT_MAX=1};
enum{PARAMETER_COUNT_MAX=9};
enum{FUNCTION_COUNT_MAX=30};
enum{CALLBACK_COUNT_MAX=1};

extern ConstantString device_name;

extern ConstantString firmware_name;
extern const modular_server::FirmwareInfo firmware_info;

extern ConstantString hardware_name;
extern const modular_server::HardwareInfo hardware_info;

extern const size_t enable_pin;

extern const long analog_write_min;
extern const long analog_write_max;
extern const long channel_pwm_min;
extern const long channel_pwm_max;

extern const int bad_index;

struct PulseInfo
{
  uint32_t channels;
  EventIdPair event_id_pair;
};

// Interrupts

// Units
extern ConstantString ms_unit;

// Properties
// Property values must be long, double, bool, long[], double[], bool[], char[], ConstantString *, (ConstantString *)[]
extern ConstantString power_max_property_name;

// Parameters
extern ConstantString channel_parameter_name;

extern ConstantString channels_parameter_name;

extern ConstantString power_parameter_name;
extern const long power_min;
extern const long power_max;

extern ConstantString powers_parameter_name;

extern ConstantString delay_parameter_name;
extern const long delay_min;
extern const long delay_max;

extern ConstantString period_parameter_name;
extern const long period_min;
extern const long period_max;

extern ConstantString on_duration_parameter_name;
extern const long on_duration_min;
extern const long on_duration_max;

extern ConstantString count_parameter_name;
extern const long count_min;
extern const long count_max;

extern ConstantString pwm_index_parameter_name;

// Functions
extern ConstantString enable_all_function_name;
extern ConstantString disable_all_function_name;
extern ConstantString enabled_function_name;
extern ConstantString set_power_when_on_function_name;
extern ConstantString set_powers_when_on_function_name;
extern ConstantString set_all_powers_when_on_function_name;
extern ConstantString get_powers_when_on_function_name;
extern ConstantString get_powers_function_name;
extern ConstantString set_channel_on_function_name;
extern ConstantString set_channel_off_function_name;
extern ConstantString set_channels_on_function_name;
extern ConstantString set_channels_off_function_name;
extern ConstantString toggle_channel_function_name;
extern ConstantString toggle_channels_function_name;
extern ConstantString toggle_all_channels_function_name;
extern ConstantString set_all_channels_on_function_name;
extern ConstantString set_all_channels_off_function_name;
extern ConstantString set_channel_on_all_others_off_function_name;
extern ConstantString set_channel_off_all_others_on_function_name;
extern ConstantString set_channels_on_all_others_off_function_name;
extern ConstantString set_channels_off_all_others_on_function_name;
extern ConstantString channel_is_on_function_name;
extern ConstantString get_channels_on_function_name;
extern ConstantString get_channels_off_function_name;
extern ConstantString get_channel_count_function_name;
extern ConstantString add_pwm_function_name;
extern ConstantString start_pwm_function_name;
extern ConstantString stop_pwm_function_name;
extern ConstantString stop_all_pwm_function_name;
extern ConstantString get_pwm_status_function_name;

// Callbacks

// Errors
extern ConstantString pwm_error;
}
}
#include "5x3.h"
#include "3x2.h"
#endif
