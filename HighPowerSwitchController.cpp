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

  // Event Controller Setup
  event_controller_.setup();

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

  // PWM Status
  setAllPwmStatusFalse();

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

  setPowersToMax();

  // Parameters
  modular_server::Parameter & channel_parameter = modular_server_.createParameter(constants::channel_parameter_name);
  channel_parameter.setRange(0,constants::CHANNEL_COUNT-1);

  modular_server::Parameter & channels_parameter = modular_server_.createParameter(constants::channels_parameter_name);
  channels_parameter.setRange(0,constants::CHANNEL_COUNT-1);
  channels_parameter.setArrayLengthRange(1,constants::CHANNEL_COUNT);

  modular_server::Parameter & power_parameter = modular_server_.createParameter(constants::power_parameter_name);
  power_parameter.setRange(constants::power_min,constants::power_max);

  modular_server::Parameter & powers_parameter = modular_server_.createParameter(constants::powers_parameter_name);
  powers_parameter.setRange(constants::power_min,constants::power_max);
  powers_parameter.setArrayLengthRange(constants::CHANNEL_COUNT,constants::CHANNEL_COUNT);

  modular_server::Parameter & delay_parameter = modular_server_.createParameter(constants::delay_parameter_name);
  delay_parameter.setRange(constants::delay_min,constants::delay_max);
  delay_parameter.setUnits(constants::ms_unit);

  modular_server::Parameter & period_parameter = modular_server_.createParameter(constants::period_parameter_name);
  period_parameter.setRange(constants::period_min,constants::period_max);
  period_parameter.setUnits(constants::ms_unit);

  modular_server::Parameter & on_duration_parameter = modular_server_.createParameter(constants::on_duration_parameter_name);
  on_duration_parameter.setRange(constants::on_duration_min,constants::on_duration_max);
  on_duration_parameter.setUnits(constants::ms_unit);

  modular_server::Parameter & count_parameter = modular_server_.createParameter(constants::count_parameter_name);
  count_parameter.setRange(constants::count_min,constants::count_max);
  count_parameter.setUnits(constants::ms_unit);

  modular_server::Parameter & pwm_index_parameter = modular_server_.createParameter(constants::pwm_index_parameter_name);
  pwm_index_parameter.setRange(0,constants::INDEXED_PULSES_COUNT_MAX-1);

  // Functions
  modular_server::Function & enable_all_function = modular_server_.createFunction(constants::enable_all_function_name);
  enable_all_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::enableAllHandler));

  modular_server::Function & disable_all_function = modular_server_.createFunction(constants::disable_all_function_name);
  disable_all_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::disableAllHandler));

  modular_server::Function & enabled_function = modular_server_.createFunction(constants::enabled_function_name);
  enabled_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::enabledHandler));
  enabled_function.setReturnTypeBool();

  modular_server::Function & set_power_when_on_function = modular_server_.createFunction(constants::set_power_when_on_function_name);
  set_power_when_on_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::setPowerWhenOnHandler));
  set_power_when_on_function.addParameter(channel_parameter);
  set_power_when_on_function.addParameter(power_parameter);
  set_power_when_on_function.setReturnTypeLong();

  modular_server::Function & set_powers_when_on_function = modular_server_.createFunction(constants::set_powers_when_on_function_name);
  set_powers_when_on_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::setPowersWhenOnHandler));
  set_powers_when_on_function.addParameter(powers_parameter);
  set_powers_when_on_function.setReturnTypeArray();

  modular_server::Function & set_all_powers_when_on_function = modular_server_.createFunction(constants::set_all_powers_when_on_function_name);
  set_all_powers_when_on_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::setAllPowersWhenOnHandler));
  set_all_powers_when_on_function.addParameter(power_parameter);
  set_all_powers_when_on_function.setReturnTypeArray();

  modular_server::Function & get_powers_when_on_function = modular_server_.createFunction(constants::get_powers_when_on_function_name);
  get_powers_when_on_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::getPowersWhenOnHandler));
  get_powers_when_on_function.setReturnTypeArray();

  modular_server::Function & get_powers_function = modular_server_.createFunction(constants::get_powers_function_name);
  get_powers_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::getPowersHandler));
  get_powers_function.setReturnTypeArray();

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

  modular_server::Function & channel_is_on_function = modular_server_.createFunction(constants::channel_is_on_function_name);
  channel_is_on_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::channelIsOnHandler));
  channel_is_on_function.addParameter(channel_parameter);
  channel_is_on_function.setReturnTypeBool();

  modular_server::Function & get_channels_on_function = modular_server_.createFunction(constants::get_channels_on_function_name);
  get_channels_on_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::getChannelsOnHandler));
  get_channels_on_function.setReturnTypeArray();

  modular_server::Function & get_channels_off_function = modular_server_.createFunction(constants::get_channels_off_function_name);
  get_channels_off_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::getChannelsOffHandler));
  get_channels_off_function.setReturnTypeArray();

  modular_server::Function & get_channel_count_function = modular_server_.createFunction(constants::get_channel_count_function_name);
  get_channel_count_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::getChannelCountHandler));

  modular_server::Function & add_pwm_function = modular_server_.createFunction(constants::add_pwm_function_name);
  add_pwm_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::addPwmHandler));
  add_pwm_function.addParameter(channels_parameter);
  add_pwm_function.addParameter(delay_parameter);
  add_pwm_function.addParameter(period_parameter);
  add_pwm_function.addParameter(on_duration_parameter);
  add_pwm_function.addParameter(count_parameter);
  add_pwm_function.setReturnTypeLong();

  modular_server::Function & start_pwm_function = modular_server_.createFunction(constants::start_pwm_function_name);
  start_pwm_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::startPwmHandler));
  start_pwm_function.addParameter(channels_parameter);
  start_pwm_function.addParameter(delay_parameter);
  start_pwm_function.addParameter(period_parameter);
  start_pwm_function.addParameter(on_duration_parameter);
  start_pwm_function.setReturnTypeLong();

  modular_server::Function & stop_pwm_function = modular_server_.createFunction(constants::stop_pwm_function_name);
  stop_pwm_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::stopPwmHandler));
  stop_pwm_function.addParameter(pwm_index_parameter);

  modular_server::Function & stop_all_pwm_function = modular_server_.createFunction(constants::stop_all_pwm_function_name);
  stop_all_pwm_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::stopAllPwmHandler));

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

long HighPowerSwitchController::setPowerWhenOn(const size_t channel, const long power)
{
  long power_to_set = 0;
  if (channel < constants::CHANNEL_COUNT)
  {
    power_to_set = power;
    if (power_to_set < constants::power_min)
    {
      power_to_set = constants::power_min;
    }
    else
    {
      modular_server::Property & power_max_property = modular_server_.property(constants::power_max_property_name);
      long power_max;
      power_max_property.getElementValue(channel,power_max);
      if (power_to_set > power_max)
      {
        power_to_set = power_max;
      }
    }
    noInterrupts();
    powers_[channel] = power_to_set;
    interrupts();
    updateChannel(channel);
  }
  return power_to_set;
}

long HighPowerSwitchController::getPowerWhenOn(const size_t channel)
{
  long power = constants::power_min;
  if (channel < constants::CHANNEL_COUNT)
  {
    noInterrupts();
    power = powers_[channel];
    interrupts();
  }
  return power;
}

long HighPowerSwitchController::getPower(const size_t channel)
{
  long power = constants::power_min;
  if (channel < constants::CHANNEL_COUNT)
  {
    if (channelIsOn(channel))
    {
      noInterrupts();
      power = getPowerWhenOn(channel);
      interrupts();
    }
  }
  return power;
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

bool HighPowerSwitchController::channelIsOn(const size_t channel)
{
  bool channel_is_on = false;
  if (channel < constants::CHANNEL_COUNT)
  {
    noInterrupts();
    uint32_t channels = channels_;
    interrupts();
    uint32_t bit = 1;
    if ((bit << channel) & channels)
    {
      channel_is_on = true;
    }
  }
  return channel_is_on;
}

uint32_t HighPowerSwitchController::getChannelsOn()
{
  return channels_;
}

size_t HighPowerSwitchController::getChannelCount()
{
  return constants::CHANNEL_COUNT;
}

int HighPowerSwitchController::addPwm(const uint32_t channels,
                                      const long delay,
                                      const long period,
                                      const long on_duration,
                                      const long count)
{
  if (indexed_pulses_.full())
  {
    return constants::bad_index;
  }
  high_power_switch_controller::constants::PulseInfo pulse_info;
  pulse_info.channels = channels;
  int index = indexed_pulses_.add(pulse_info);
  EventIdPair event_id_pair = event_controller_.addPwmUsingDelay(makeFunctor((Functor1<int> *)0,*this,&HighPowerSwitchController::setChannelsOnHandler),
                                                                 makeFunctor((Functor1<int> *)0,*this,&HighPowerSwitchController::setChannelsOffHandler),
                                                                 delay,
                                                                 period,
                                                                 on_duration,
                                                                 count,
                                                                 index);
  event_controller_.addStartFunctor(event_id_pair,makeFunctor((Functor1<int> *)0,*this,&HighPowerSwitchController::startPwmHandler));
  event_controller_.addStopFunctor(event_id_pair,makeFunctor((Functor1<int> *)0,*this,&HighPowerSwitchController::stopPwmHandler));
  indexed_pulses_[index].event_id_pair = event_id_pair;
  event_controller_.enable(event_id_pair);
  return index;
}

int HighPowerSwitchController::startPwm(const uint32_t channels,
                                        const long delay,
                                        const long period,
                                        const long on_duration)
{
  if (indexed_pulses_.full())
  {
    return -1;
  }
  high_power_switch_controller::constants::PulseInfo pulse_info;
  pulse_info.channels = channels;
  int index = indexed_pulses_.add(pulse_info);
  EventIdPair event_id_pair = event_controller_.addInfinitePwmUsingDelay(makeFunctor((Functor1<int> *)0,*this,&HighPowerSwitchController::setChannelsOnHandler),
                                                                         makeFunctor((Functor1<int> *)0,*this,&HighPowerSwitchController::setChannelsOffHandler),
                                                                         delay,
                                                                         period,
                                                                         on_duration,
                                                                         index);
  event_controller_.addStartFunctor(event_id_pair,makeFunctor((Functor1<int> *)0,*this,&HighPowerSwitchController::startPwmHandler));
  event_controller_.addStopFunctor(event_id_pair,makeFunctor((Functor1<int> *)0,*this,&HighPowerSwitchController::stopPwmHandler));
  indexed_pulses_[index].event_id_pair = event_id_pair;
  event_controller_.enable(event_id_pair);
  return index;
}

void HighPowerSwitchController::stopPwm(const int pwm_index)
{
  if (pwm_index < 0)
  {
    return;
  }
  if (indexed_pulses_.indexHasValue(pwm_index))
  {
    constants::PulseInfo pulse_info = indexed_pulses_[pwm_index];
    event_controller_.remove(pulse_info.event_id_pair);
  }
}

void HighPowerSwitchController::stopAllPwm()
{
  for (size_t i=0; i<constants::INDEXED_PULSES_COUNT_MAX; ++i)
  {
    stopPwm(i);
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

void HighPowerSwitchController::updateChannel(const size_t channel)
{
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

void HighPowerSwitchController::updateAllChannels()
{
  noInterrupts();
  uint32_t channels = channels_;
  interrupts();
  setChannels(channels);
}

void HighPowerSwitchController::setAllPwmStatusFalse()
{
  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    for (size_t level=0; level<=constants::PWM_LEVEL_COUNT_MAX; ++level)
    {
      pwm_status_[channel][level] = false;
    }
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

void HighPowerSwitchController::startPwmHandler(int index)
{
}

void HighPowerSwitchController::stopPwmHandler(int index)
{
  uint32_t & channels = indexed_pulses_[index].channels;
  setChannelsOff(channels);
  indexed_pulses_.remove(index);
}

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

  updateChannel(channel);
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

void HighPowerSwitchController::setPowerWhenOnHandler()
{
  size_t channel;
  modular_server_.parameter(constants::channel_parameter_name).getValue(channel);
  size_t power;
  modular_server_.parameter(constants::power_parameter_name).getValue(power);
  power = setPowerWhenOn(channel,power);
  modular_server_.response().returnResult(power);
}

void HighPowerSwitchController::setPowersWhenOnHandler()
{
  ArduinoJson::JsonArray * powers_array_ptr;
  modular_server_.parameter(constants::powers_parameter_name).getValue(powers_array_ptr);

  modular_server_.response().writeResultKey();
  modular_server_.response().beginArray();

  size_t channel = 0;
  for (ArduinoJson::JsonArray::iterator powers_it=powers_array_ptr->begin();
       powers_it != powers_array_ptr->end();
       ++powers_it)
  {
    long power = *powers_it;
    power = setPowerWhenOn(channel,power);
    modular_server_.response().write(power);
    ++channel;
  }

  modular_server_.response().endArray();
}

void HighPowerSwitchController::setAllPowersWhenOnHandler()
{
  size_t power_to_set;
  modular_server_.parameter(constants::power_parameter_name).getValue(power_to_set);

  modular_server_.response().writeResultKey();
  modular_server_.response().beginArray();

  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    long power = setPowerWhenOn(channel,power_to_set);
    modular_server_.response().write(power);
  }

  modular_server_.response().endArray();
}

void HighPowerSwitchController::getPowersWhenOnHandler()
{
  modular_server_.response().writeResultKey();
  modular_server_.response().beginArray();
  long power;
  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    power = getPowerWhenOn(channel);
    modular_server_.response().write(power);
  }
  modular_server_.response().endArray();
}

void HighPowerSwitchController::getPowersHandler()
{
  modular_server_.response().writeResultKey();
  modular_server_.response().beginArray();
  long power;
  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    power = getPower(channel);
    modular_server_.response().write(power);
  }
  modular_server_.response().endArray();
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
  size_t channel;
  modular_server_.parameter(constants::channel_parameter_name).getValue(channel);
  setChannelOnAllOthersOff(channel);
}

void HighPowerSwitchController::setChannelOffAllOthersOnHandler()
{
  size_t channel;
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

void HighPowerSwitchController::channelIsOnHandler()
{
  size_t channel;
  modular_server_.parameter(constants::channel_parameter_name).getValue(channel);
  bool channel_is_on = channelIsOn(channel);
  modular_server_.response().returnResult(channel_is_on);
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
  size_t channel_count = getChannelCount();
  modular_server_.response().returnResult(channel_count);
}

void HighPowerSwitchController::addPwmHandler()
{
  ArduinoJson::JsonArray * channels_array_ptr;
  modular_server_.parameter(constants::channels_parameter_name).getValue(channels_array_ptr);
  long delay;
  modular_server_.parameter(constants::delay_parameter_name).getValue(delay);
  long period;
  modular_server_.parameter(constants::period_parameter_name).getValue(period);
  long on_duration;
  modular_server_.parameter(constants::on_duration_parameter_name).getValue(on_duration);
  long count;
  modular_server_.parameter(constants::count_parameter_name).getValue(count);
  const uint32_t channels = arrayToChannels(*channels_array_ptr);
  int index = addPwm(channels,delay,period,on_duration,count);
  if (index >= 0)
  {
    modular_server_.response().returnResult(index);
  }
  else
  {
    modular_server_.response().returnError(constants::pwm_error);
  }
}

void HighPowerSwitchController::startPwmHandler()
{
  ArduinoJson::JsonArray * channels_array_ptr;
  modular_server_.parameter(constants::channels_parameter_name).getValue(channels_array_ptr);
  long delay;
  modular_server_.parameter(constants::delay_parameter_name).getValue(delay);
  long period;
  modular_server_.parameter(constants::period_parameter_name).getValue(period);
  long on_duration;
  modular_server_.parameter(constants::on_duration_parameter_name).getValue(on_duration);
  const uint32_t channels = arrayToChannels(*channels_array_ptr);
  int index = startPwm(channels,delay,period,on_duration);
  if (index >= 0)
  {
    modular_server_.response().returnResult(index);
  }
  else
  {
    modular_server_.response().returnError(constants::pwm_error);
  }
}

void HighPowerSwitchController::stopPwmHandler()
{
  int pwm_index;
  modular_server_.parameter(constants::pwm_index_parameter_name).getValue(pwm_index);
  stopPwm(pwm_index);
}

void HighPowerSwitchController::stopAllPwmHandler()
{
  stopAllPwm();
}

void HighPowerSwitchController::getPwmStatusHandler()
{
}

void HighPowerSwitchController::setChannelsOnHandler(int index)
{
  uint32_t & channels = indexed_pulses_[index].channels;
  setChannelsOn(channels);
}

void HighPowerSwitchController::setChannelsOffHandler(int index)
{
  uint32_t & channels = indexed_pulses_[index].channels;
  setChannelsOff(channels);
}