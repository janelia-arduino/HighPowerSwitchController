// ----------------------------------------------------------------------------
// 5x3.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "5x3.h"


#if defined(__MK64FX512__)

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

const size_t enable_pins[CHANNEL_COUNT] = {49};
const size_t step_pins[CHANNEL_COUNT] = {53};
const size_t dir_pins[CHANNEL_COUNT] = {51};

size_t step_half_period_us_max = 20;

// Interrupts

// Units

// Properties
const ConstantString * const enable_polarity_default[CHANNEL_COUNT] =
  {
    &polarity_high,
  };

const ConstantString * const step_polarity_default[CHANNEL_COUNT] =
  {
    &polarity_high,
  };

const ConstantString * const dir_polarity_default[CHANNEL_COUNT] =
  {
    &polarity_high,
  };

const ConstantString * const mode_default[CHANNEL_COUNT] =
  {
    &mode_position,
  };

// Parameters

// Functions

// Callbacks

// Errors
}
}
#endif
