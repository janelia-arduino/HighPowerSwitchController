// ----------------------------------------------------------------------------
// HighPowerSwitchController.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "HighPowerSwitchController.h"


using namespace high_power_switch_controller;

HighPowerSwitchController::HighPowerSwitchController()
{
}

HighPowerSwitchController::~HighPowerSwitchController()
{
  disableAll();
}

void HighPowerSwitchController::setup()
{
  // Parent Setup
  ModularDeviceBase::setup();

  // Pin Setup
  pinMode(constants::enable_pin,OUTPUT);
  disableAll();

  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    pinMode(constants::signal_pins[channel],OUTPUT);
    pinMode(constants::fault_pins[channel],INPUT);
  }
  setAllChannelsOff();

  // Interrupts

  // Set Device ID
  modular_server_.setDeviceName(constants::device_name);

  // Add Hardware
  modular_server_.addHardware(constants::hardware_info,
                              interrupts_);

  // Add Firmware
  modular_server_.addFirmware(constants::firmware_info,
                              properties_,
                              parameters_,
                              functions_,
                              callbacks_);
  // Properties

  // Parameters
  modular_server::Parameter & channel_parameter = modular_server_.createParameter(constants::channel_parameter_name);
  channel_parameter.setRange(0,constants::CHANNEL_COUNT-1);

  // Functions
  modular_server::Function & enable_all_function = modular_server_.createFunction(constants::enable_all_function_name);
  enable_all_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::enableAllHandler));

  modular_server::Function & disable_all_function = modular_server_.createFunction(constants::disable_all_function_name);
  disable_all_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::disableAllHandler));

  modular_server::Function & enabled_function = modular_server_.createFunction(constants::enabled_function_name);
  enabled_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::enabledHandler));
  enabled_function.setReturnTypeBool();

  modular_server::Function & set_channel_on_function = modular_server_.createFunction(constants::set_channel_on_function_name);
  set_channel_on_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::setChannelOnHandler));
  set_channel_on_function.addParameter(channel_parameter);

  modular_server::Function & set_channel_off_function = modular_server_.createFunction(constants::set_channel_off_function_name);
  set_channel_off_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::setChannelOffHandler));
  set_channel_off_function.addParameter(channel_parameter);

  // Callbacks

}

void HighPowerSwitchController::enableAll()
{
  digitalWrite(constants::enable_pin,LOW);
  enabled_ = true;
}

void HighPowerSwitchController::disableAll()
{
  digitalWrite(constants::enable_pin,HIGH);
  enabled_ = false;
}

bool HighPowerSwitchController::enabled()
{
  return enabled_;
}

void HighPowerSwitchController::setChannelOn(const size_t channel)
{
  if (channel < constants::CHANNEL_COUNT)
  {
    digitalWrite(constants::signal_pins[channel],HIGH);
  }
}

void HighPowerSwitchController::setChannelOff(const size_t channel)
{
  if (channel < constants::CHANNEL_COUNT)
  {
    digitalWrite(constants::signal_pins[channel],LOW);
  }
}

void HighPowerSwitchController::setAllChannelsOn()
{
  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    setChannelOn(channel);
  }
}

void HighPowerSwitchController::setAllChannelsOff()
{
  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    setChannelOff(channel);
  }
}

// Handlers must be non-blocking (avoid 'delay')
//
// modular_server_.parameter(parameter_name).getValue(value) value type must be either:
// fixed-point number (int, long, etc.)
// floating-point number (float, double)
// bool
// const char *
// ArduinoJson::JsonArray *
// ArduinoJson::JsonObject *
//
// For more info read about ArduinoJson parsing https://github.com/janelia-arduino/ArduinoJson
//
// modular_server_.property(property_name).getValue(value) value type must match the property default type
// modular_server_.property(property_name).setValue(value) value type must match the property default type
// modular_server_.property(property_name).getElementValue(value) value type must match the property array element default type
// modular_server_.property(property_name).setElementValue(value) value type must match the property array element default type

void HighPowerSwitchController::enableAllHandler()
{
  enableAll();
}

void HighPowerSwitchController::disableAllHandler()
{
  disableAll();
}

void HighPowerSwitchController::enabledHandler()
{
  bool all_enabled = enabled();
  modular_server_.response().returnResult(all_enabled);
}

void HighPowerSwitchController::setChannelOnHandler()
{
  int channel;
  modular_server_.parameter(constants::channel_parameter_name).getValue(channel);
  setChannelOn(channel);
}

void HighPowerSwitchController::setChannelOffHandler()
{
  int channel;
  modular_server_.parameter(constants::channel_parameter_name).getValue(channel);
  setChannelOff(channel);
}

