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
    .part_number=1210,
    .version_major=1,
    .version_minor=0,
  };

const size_t enable_pin = 52;

const size_t signal_pins[CHANNEL_COUNT] =
  {
    2,
    6,
    29,
    30,
    35,
    36,
    22,
    23
  };

const size_t fault_pins[CHANNEL_COUNT] =
  {
    55,
    54,
    40,
    41,
    42,
    50,
    49,
    16
  };

// Interrupts

// Units

// Properties
const long power_max_default[CHANNEL_COUNT] =
  {
    100,
    100,
    100,
    100,
    100,
    100,
    100,
    100,
  };

const long states_array_default[STATE_COUNT] =
  {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
  };

// Parameters

// Functions

// Callbacks

// Errors
}
}
#endif
