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
  channels_ = 0;
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
  modular_server::Property & power_max_property = modular_server_.createProperty(constants::power_max_property_name,constants::power_max_default);
  power_max_property.setRange(constants::power_min,constants::power_max);
  power_max_property.attachPostSetElementValueFunctor(makeFunctor((Functor1<const size_t> *)0,*this,&HighPowerSwitchController::setPowerMaxHandler));

  modular_server::Property & states_property = modular_server_.createProperty(constants::states_property_name,constants::states_array_default);

  setPowersToMax();

  // Parameters
  modular_server::Parameter & channel_parameter = modular_server_.createParameter(constants::channel_parameter_name);
  channel_parameter.setRange(0,constants::CHANNEL_COUNT-1);

  modular_server::Parameter & channels_parameter = modular_server_.createParameter(constants::channels_parameter_name);
  channels_parameter.setRange(0,constants::CHANNEL_COUNT-1);
  channels_parameter.setArrayLengthRange(1,constants::CHANNEL_COUNT);

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

  modular_server::Function & set_channels_on_function = modular_server_.createFunction(constants::set_channels_on_function_name);
  set_channels_on_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::setChannelsOnHandler));
  set_channels_on_function.addParameter(channels_parameter);

  modular_server::Function & set_channels_off_function = modular_server_.createFunction(constants::set_channels_off_function_name);
  set_channels_off_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::setChannelsOffHandler));
  set_channels_off_function.addParameter(channels_parameter);

  modular_server::Function & toggle_channel_function = modular_server_.createFunction(constants::toggle_channel_function_name);
  toggle_channel_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::toggleChannelHandler));
  toggle_channel_function.addParameter(channel_parameter);

  modular_server::Function & toggle_channels_function = modular_server_.createFunction(constants::toggle_channels_function_name);
  toggle_channels_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::toggleChannelsHandler));
  toggle_channels_function.addParameter(channels_parameter);

  modular_server::Function & toggle_all_channels_function = modular_server_.createFunction(constants::toggle_all_channels_function_name);
  toggle_all_channels_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::toggleAllChannelsHandler));

  modular_server::Function & set_all_channels_on_function = modular_server_.createFunction(constants::set_all_channels_on_function_name);
  set_all_channels_on_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::setAllChannelsOnHandler));

  modular_server::Function & set_all_channels_off_function = modular_server_.createFunction(constants::set_all_channels_off_function_name);
  set_all_channels_off_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::setAllChannelsOffHandler));

  modular_server::Function & set_channel_on_all_others_off_function = modular_server_.createFunction(constants::set_channel_on_all_others_off_function_name);
  set_channel_on_all_others_off_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::setChannelOnAllOthersOffHandler));
  set_channel_on_all_others_off_function.addParameter(channel_parameter);

  modular_server::Function & set_channel_off_all_others_on_function = modular_server_.createFunction(constants::set_channel_off_all_others_on_function_name);
  set_channel_off_all_others_on_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::setChannelOffAllOthersOnHandler));
  set_channel_off_all_others_on_function.addParameter(channel_parameter);

  modular_server::Function & set_channels_on_all_others_off_function = modular_server_.createFunction(constants::set_channels_on_all_others_off_function_name);
  set_channels_on_all_others_off_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::setChannelsOnAllOthersOffHandler));
  set_channels_on_all_others_off_function.addParameter(channels_parameter);

  modular_server::Function & set_channels_off_all_others_on_function = modular_server_.createFunction(constants::set_channels_off_all_others_on_function_name);
  set_channels_off_all_others_on_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::setChannelsOffAllOthersOnHandler));
  set_channels_off_all_others_on_function.addParameter(channels_parameter);

  modular_server::Function & get_channels_on_function = modular_server_.createFunction(constants::get_channels_on_function_name);
  get_channels_on_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::getChannelsOnHandler));
  get_channels_on_function.setReturnTypeArray();

  modular_server::Function & get_channels_off_function = modular_server_.createFunction(constants::get_channels_off_function_name);
  get_channels_off_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::getChannelsOffHandler));
  get_channels_off_function.setReturnTypeArray();

  modular_server::Function & get_channel_count_function = modular_server_.createFunction(constants::get_channel_count_function_name);
  get_channel_count_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::getChannelCountHandler));

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

void HighPowerSwitchController::setChannels(const uint32_t channels)
{
  uint32_t bit = 1;
  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    if ((bit << channel) & channels)
    {
      setChannelOn(channel);
    }
    else
    {
      setChannelOff(channel);
    }
  }
}

void HighPowerSwitchController::setChannelOn(const size_t channel)
{
  if (channel < constants::CHANNEL_COUNT)
  {
    uint32_t bit = 1;
    bit = bit << channel;

    noInterrupts();
    long power = powers_[channel];
    interrupts();
    long analog_write_value = powerToAnalogWriteValue(power);

    noInterrupts();
    channels_ |= bit;
    analogWrite(constants::signal_pins[channel],analog_write_value);
    interrupts();
  }
}

void HighPowerSwitchController::setChannelOff(const size_t channel)
{
  if (channel < constants::CHANNEL_COUNT)
  {
    uint32_t bit = 1;
    bit = bit << channel;

    noInterrupts();
    channels_ &= ~bit;
    analogWrite(constants::signal_pins[channel],constants::analog_write_min);
    interrupts();
  }
}

void HighPowerSwitchController::setChannelsOn(const uint32_t channels)
{
  uint32_t bit = 1;
  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    if ((bit << channel) & channels)
    {
      setChannelOn(channel);
    }
  }
}

void HighPowerSwitchController::setChannelsOff(const uint32_t channels)
{
  uint32_t bit = 1;
  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    if ((bit << channel) & channels)
    {
      setChannelOff(channel);
    }
  }
}

void HighPowerSwitchController::toggleChannel(const size_t channel)
{
  if (channel < constants::CHANNEL_COUNT)
  {
    uint32_t bit = 1;
    bit = bit << channel;
    noInterrupts();
    uint32_t channels = channels_;
    interrupts();
    channels ^= bit;
    if ((bit << channel) & channels)
    {
      setChannelOn(channel);
    }
    else
    {
      setChannelOff(channel);
    }
  }
}

void HighPowerSwitchController::toggleChannels(const uint32_t channels)
{
  noInterrupts();
  channels_ ^= channels;
  interrupts();
  setChannels(channels_);
}

void HighPowerSwitchController::toggleAllChannels()
{
  noInterrupts();
  channels_ = ~channels_;
  interrupts();
  setChannels(channels_);
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

void HighPowerSwitchController::setChannelOnAllOthersOff(const size_t channel)
{
  if (channel < constants::CHANNEL_COUNT)
  {
    uint32_t bit = 1;
    bit = bit << channel;
    noInterrupts();
    channels_ = bit;
    interrupts();
    setChannels(channels_);
  }
}

void HighPowerSwitchController::setChannelOffAllOthersOn(const size_t channel)
{
  if (channel < constants::CHANNEL_COUNT)
  {
    uint32_t bit = 1;
    bit = bit << channel;
    noInterrupts();
    channels_ = ~bit;
    interrupts();
    setChannels(channels_);
  }
}

void HighPowerSwitchController::setChannelsOnAllOthersOff(const uint32_t channels)
{
  noInterrupts();
  channels_ = channels;
  interrupts();
  setChannels(channels_);
}

void HighPowerSwitchController::setChannelsOffAllOthersOn(const uint32_t channels)
{
  noInterrupts();
  channels_ = ~channels;
  interrupts();
  setChannels(channels_);
}

uint32_t HighPowerSwitchController::getChannelsOn()
{
  return channels_;
}

size_t HighPowerSwitchController::getChannelCount()
{
  return constants::CHANNEL_COUNT;
}

long HighPowerSwitchController::powerToAnalogWriteValue(const long power)
{
  long pwm_value = map(power,
                       constants::power_min,
                       constants::power_max,
                       constants::channel_pwm_min,
                       constants::channel_pwm_max);
  long analog_write_value = map(pwm_value,
                                constants::channel_pwm_min,
                                constants::channel_pwm_max,
                                constants::analog_write_min,
                                constants::analog_write_max);
  return analog_write_value;
}

void HighPowerSwitchController::setPowersToMax()
{
  modular_server::Property & power_max_property = modular_server_.property(constants::power_max_property_name);
  long power_max;
  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    power_max_property.getElementValue(channel,power_max);
    noInterrupts();
    powers_[channel] = power_max;
    interrupts();
  }
}

uint32_t HighPowerSwitchController::arrayToChannels(ArduinoJson::JsonArray & channels_array)
{
  uint32_t channels = 0;
  uint32_t bit = 1;
  for (ArduinoJson::JsonArray::iterator channels_it=channels_array.begin();
       channels_it != channels_array.end();
       ++channels_it)
  {
    long channel = *channels_it;
    channels |= bit << channel;
  }
  return channels;
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

void HighPowerSwitchController::setPowerMaxHandler(const size_t channel)
{
  modular_server::Property & power_max_property = modular_server_.property(constants::power_max_property_name);
  long power_max;
  power_max_property.getElementValue(channel,power_max);
  noInterrupts();
  if (powers_[channel] > power_max)
  {
    powers_[channel] = power_max;
  }
  interrupts();

  uint32_t bit = 1;
  bit = bit << channel;
  noInterrupts();
  uint32_t channels = channels_;
  interrupts();
  if ((bit << channel) & channels)
  {
    setChannelOn(channel);
  }
}

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
  size_t channel;
  modular_server_.parameter(constants::channel_parameter_name).getValue(channel);
  setChannelOn(channel);
}

void HighPowerSwitchController::setChannelOffHandler()
{
  size_t channel;
  modular_server_.parameter(constants::channel_parameter_name).getValue(channel);
  setChannelOff(channel);
}

void HighPowerSwitchController::setChannelsOnHandler()
{
  ArduinoJson::JsonArray * channels_array_ptr;
  modular_server_.parameter(constants::channels_parameter_name).getValue(channels_array_ptr);
  const uint32_t channels = arrayToChannels(*channels_array_ptr);
  setChannelsOn(channels);
}

void HighPowerSwitchController::setChannelsOffHandler()
{
  ArduinoJson::JsonArray * channels_array_ptr;
  modular_server_.parameter(constants::channels_parameter_name).getValue(channels_array_ptr);
  const uint32_t channels = arrayToChannels(*channels_array_ptr);
  setChannelsOff(channels);
}

void HighPowerSwitchController::toggleChannelHandler()
{
  size_t channel;
  modular_server_.parameter(constants::channel_parameter_name).getValue(channel);
  toggleChannel(channel);
}

void HighPowerSwitchController::toggleChannelsHandler()
{
  ArduinoJson::JsonArray * channels_array_ptr;
  modular_server_.parameter(constants::channels_parameter_name).getValue(channels_array_ptr);
  const uint32_t channels = arrayToChannels(*channels_array_ptr);
  toggleChannels(channels);
}

void HighPowerSwitchController::toggleAllChannelsHandler()
{
  toggleAllChannels();
}

void HighPowerSwitchController::setAllChannelsOnHandler()
{
  setAllChannelsOn();
}

void HighPowerSwitchController::setAllChannelsOffHandler()
{
  setAllChannelsOff();
}

void HighPowerSwitchController::setChannelOnAllOthersOffHandler()
{
  int channel;
  modular_server_.parameter(constants::channel_parameter_name).getValue(channel);
  setChannelOnAllOthersOff(channel);
}

void HighPowerSwitchController::setChannelOffAllOthersOnHandler()
{
  int channel;
  modular_server_.parameter(constants::channel_parameter_name).getValue(channel);
  setChannelOffAllOthersOn(channel);
}

void HighPowerSwitchController::setChannelsOnAllOthersOffHandler()
{
  ArduinoJson::JsonArray * channels_array_ptr;
  modular_server_.parameter(constants::channels_parameter_name).getValue(channels_array_ptr);
  const uint32_t channels = arrayToChannels(*channels_array_ptr);
  setChannelsOnAllOthersOff(channels);
}

void HighPowerSwitchController::setChannelsOffAllOthersOnHandler()
{
  ArduinoJson::JsonArray * channels_array_ptr;
  modular_server_.parameter(constants::channels_parameter_name).getValue(channels_array_ptr);
  const uint32_t channels = arrayToChannels(*channels_array_ptr);
  setChannelsOffAllOthersOn(channels);
}

void HighPowerSwitchController::getChannelsOnHandler()
{
  uint32_t channels_on = getChannelsOn();
  uint32_t bit = 1;
  modular_server_.response().writeResultKey();
  modular_server_.response().beginArray();
  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    if (channels_on & (bit << channel))
    {
      modular_server_.response().write(channel);
    }
  }
  modular_server_.response().endArray();
}

void HighPowerSwitchController::getChannelsOffHandler()
{
  uint32_t channels_on = getChannelsOn();
  uint32_t channels_off = ~channels_on;
  uint32_t bit = 1;
  modular_server_.response().writeResultKey();
  modular_server_.response().beginArray();
  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    if (channels_off & (bit << channel))
    {
      modular_server_.response().write(channel);
    }
  }
  modular_server_.response().endArray();
}

void HighPowerSwitchController::getChannelCountHandler()
{
  int channel_count = getChannelCount();
  modular_server_.response().returnResult(channel_count);
}

