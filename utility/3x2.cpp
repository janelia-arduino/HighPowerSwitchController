// ----------------------------------------------------------------------------
// 3x2.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "3x2.h"


#if defined(__MK20DX256__)

namespace high_power_switch_controller
{
namespace constants
{
const modular_server::HardwareInfo hardware_info =
  {
    .name_ptr=&hardware_name,
    .part_number=0,
    .version_major=1,
    .version_minor=0,
  };

const size_t cs_pins[CONTROLLER_COUNT] = {10};
const size_t enable_pins[CHANNEL_COUNT] = {2,3,4};

// Interrupts

// Units

// Properties
const double steps_per_position_units_default[CHANNEL_COUNT] =
  {
    steps_per_position_units_element_default,
    steps_per_position_units_element_default,
    steps_per_position_units_element_default,
  };

const double velocity_max_default[CHANNEL_COUNT] =
  {
    velocity_max_element_default,
    velocity_max_element_default,
    velocity_max_element_default,
  };

const double velocity_min_default[CHANNEL_COUNT] =
  {
    velocity_min_element_default,
    velocity_min_element_default,
    velocity_min_element_default,
  };

const double acceleration_max_default[CHANNEL_COUNT] =
  {
    acceleration_max_element_default,
    acceleration_max_element_default,
    acceleration_max_element_default,
  };

const ConstantString * const enable_polarity_default[CHANNEL_COUNT] =
  {
    &polarity_high,
    &polarity_high,
    &polarity_high,
  };

const ConstantString * const step_polarity_default[CHANNEL_COUNT] =
  {
    &polarity_high,
    &polarity_high,
    &polarity_high,
  };

const ConstantString * const dir_polarity_default[CHANNEL_COUNT] =
  {
    &polarity_high,
    &polarity_high,
    &polarity_high,
  };

const bool left_switch_stop_enabled_default[CHANNEL_COUNT] =
  {
    false,
    false,
    false,
  };

const bool right_switch_stop_enabled_default[CHANNEL_COUNT] =
  {
    false,
    false,
    false,
  };

const bool switch_soft_stop_enabled_default[CHANNEL_COUNT] =
  {
    false,
    false,
    false,
  };

const double home_velocity_default[CHANNEL_COUNT] =
  {
    home_velocity_element_default,
    home_velocity_element_default,
    home_velocity_element_default,
  };

// Parameters
const long channel_max = CHANNEL_COUNT - 1;

// Functions

// Callbacks

// Errors
}
}
#endif
