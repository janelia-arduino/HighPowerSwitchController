// ----------------------------------------------------------------------------
// 3x2.cpp
//
//
// Authors:
// Peter Polidoro peter@polidoro.io
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
  .part_number=1211,
  .version_major=1,
  .version_minor=0,
};

const size_t enable_pin = 2;

const size_t signal_pins[CHANNEL_COUNT] =
{
  3,
  4,
};

const size_t fault_pins[CHANNEL_COUNT] =
{
  5,
  6,
};

// Pins

// Units

// Properties
const long power_max_default[CHANNEL_COUNT] =
{
  100,
  100,
};

// Parameters

// Functions

// Callbacks

// Errors
}
}
#endif
