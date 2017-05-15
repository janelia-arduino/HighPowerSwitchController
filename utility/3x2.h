// ----------------------------------------------------------------------------
// 3x2.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef HIGH_POWER_SWITCH_CONTROLLER_3X2_CONSTANTS_H
#define HIGH_POWER_SWITCH_CONTROLLER_3X2_CONSTANTS_H
#include "Constants.h"


#if defined(__MK20DX256__)

namespace high_power_switch_controller
{
namespace constants
{
//MAX values must be >= 1, >= created/copied count, < RAM limit
enum{INTERRUPT_COUNT_MAX=1};

enum {CONTROLLER_COUNT=1};
// CHANNEL_COUNT = CONTROLLER_COUNT*CHANNELS_PER_CONTROLLER_COUNT
// CHANNELS_PER_CONTROLLER_COUNT = 3
enum {CHANNEL_COUNT=3};

extern const size_t cs_pins[CONTROLLER_COUNT];
extern const size_t enable_pins[CHANNEL_COUNT];

// Units

// Properties
// Property values must be long, double, bool, long[], double[], bool[], char[], ConstantString *, (ConstantString *)[]
extern const double steps_per_position_units_default[CHANNEL_COUNT];

extern const double velocity_max_default[CHANNEL_COUNT];

extern const double velocity_min_default[CHANNEL_COUNT];

extern const double acceleration_max_default[CHANNEL_COUNT];

extern const ConstantString * const enable_polarity_default[CHANNEL_COUNT];

extern const bool left_switch_stop_enabled_default[CHANNEL_COUNT];

extern const bool right_switch_stop_enabled_default[CHANNEL_COUNT];

extern const bool switch_soft_stop_enabled_default[CHANNEL_COUNT];

extern const double home_velocity_default[CHANNEL_COUNT];

// Parameters

// Functions

// Callbacks

// Errors
}
}
#endif
#endif
