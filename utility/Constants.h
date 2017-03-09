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


namespace high_power_switch_controller
{
namespace constants
{
//MAX values must be >= 1, >= created/copied count, < RAM limit
enum{PROPERTY_COUNT_MAX=1};
enum{PARAMETER_COUNT_MAX=1};
enum{FUNCTION_COUNT_MAX=5};
enum{CALLBACK_COUNT_MAX=1};

extern ConstantString device_name;

extern ConstantString firmware_name;
extern const modular_server::FirmwareInfo firmware_info;

extern ConstantString hardware_name;
extern const modular_server::HardwareInfo hardware_info;

extern const size_t enable_pin;

// Interrupts

// Units

// Properties
// Property values must be long, double, bool, long[], double[], bool[], char[], ConstantString *, (ConstantString *)[]

// Parameters
extern ConstantString channel_parameter_name;

// Functions
extern ConstantString enable_all_function_name;
extern ConstantString disable_all_function_name;
extern ConstantString enabled_function_name;
extern ConstantString set_channel_on_function_name;
extern ConstantString set_channel_off_function_name;

// Callbacks

// Errors
}
}
#include "5x3.h"
#include "3x2.h"
#endif
