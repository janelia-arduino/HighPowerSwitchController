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
  // disable();
}

void HighPowerSwitchController::setup()
{
  // Parent Setup
  ModularDeviceBase::setup();

  // Controller Setup
  for (size_t controller_i=0; controller_i<constants::CONTROLLER_COUNT; ++controller_i)
  {
    Controller & controller = controllers_[controller_i];
    controller.setup(constants::cs_pins[controller_i],constants::clock_frequency_mhz);
  }

  // Pin Setup
  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    pinMode(constants::enable_pins[channel], INPUT);
    enabled_[channel] = false;
    homing_[channel] = false;
    homed_[channel] = false;
  }

  // Interrupts
  // #if defined(__MK64FX512__)
  // modular_server::Interrupt & bnc_b_interrupt = modular_server_.createInterrupt(constants::bnc_b_interrupt_name,
  //                                                                               constants::bnc_b_pin);


  // #endif
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
  modular_server::Property & steps_per_position_unit_property = modular_server_.createProperty(constants::steps_per_position_unit_property_name,constants::steps_per_position_unit_default);
  steps_per_position_unit_property.attachPreSetElementValueFunctor(makeFunctor((Functor1<const size_t> *)0,*this,&HighPowerSwitchController::preUpdateScaledPropertiesHandler));
  steps_per_position_unit_property.attachPostSetElementValueFunctor(makeFunctor((Functor1<const size_t> *)0,*this,&HighPowerSwitchController::postUpdateScaledPropertiesHandler));

  modular_server::Property & velocity_max_property = modular_server_.createProperty(constants::velocity_max_property_name,constants::velocity_max_default);
  velocity_max_property.setUnits(constants::position_units_per_second_unit);
  velocity_max_property.setRange(constants::velocity_max_min,constants::velocity_max_max);
  velocity_max_property.attachPostSetElementValueFunctor(makeFunctor((Functor1<const size_t> *)0,*this,&HighPowerSwitchController::setLimitsHandler));

  modular_server::Property & velocity_min_property = modular_server_.createProperty(constants::velocity_min_property_name,constants::velocity_min_default);
  velocity_min_property.setUnits(constants::position_units_per_second_unit);
  velocity_min_property.setRange(constants::velocity_min_min,constants::velocity_min_max);
  velocity_min_property.attachPostSetElementValueFunctor(makeFunctor((Functor1<const size_t> *)0,*this,&HighPowerSwitchController::setLimitsHandler));

  modular_server::Property & acceleration_max_property = modular_server_.createProperty(constants::acceleration_max_property_name,constants::acceleration_max_default);
  acceleration_max_property.setUnits(constants::position_units_per_second_per_second_unit);
  acceleration_max_property.setRange(constants::acceleration_max_min,constants::acceleration_max_max);
  acceleration_max_property.attachPostSetElementValueFunctor(makeFunctor((Functor1<const size_t> *)0,*this,&HighPowerSwitchController::setLimitsHandler));

  modular_server::Property & enable_polarity_property = modular_server_.createProperty(constants::enable_polarity_property_name,constants::enable_polarity_default);
  enable_polarity_property.setSubset(constants::polarity_ptr_subset);

  modular_server::Property & step_polarity_inverted_property = modular_server_.createProperty(constants::step_polarity_inverted_property_name,constants::step_polarity_inverted_default);
  step_polarity_inverted_property.attachPostSetValueFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::setStepPolarityInvertedHandler));

  modular_server::Property & dir_polarity_inverted_property = modular_server_.createProperty(constants::dir_polarity_inverted_property_name,constants::dir_polarity_inverted_default);
  dir_polarity_inverted_property.attachPostSetValueFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::setDirPolarityInvertedHandler));

  modular_server::Property & switch_active_polarity_property = modular_server_.createProperty(constants::switch_active_polarity_property_name,constants::switch_active_polarity_default);
  switch_active_polarity_property.setSubset(constants::polarity_ptr_subset);
  switch_active_polarity_property.attachPostSetValueFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::setSwitchActivePolarityHandler));

  modular_server::Property & left_switch_stop_enabled_property = modular_server_.createProperty(constants::left_switch_stop_enabled_property_name,constants::left_switch_stop_enabled_default);
  left_switch_stop_enabled_property.attachPostSetElementValueFunctor(makeFunctor((Functor1<const size_t> *)0,*this,&HighPowerSwitchController::setLeftSwitchStopEnabledHandler));

  modular_server::Property & right_switches_enabled_property = modular_server_.createProperty(constants::right_switches_enabled_property_name,constants::right_switches_enabled_default);
  right_switches_enabled_property.attachPostSetValueFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::setRightSwitchesEnabledHandler));

  modular_server::Property & right_switch_stop_enabled_property = modular_server_.createProperty(constants::right_switch_stop_enabled_property_name,constants::right_switch_stop_enabled_default);
  right_switch_stop_enabled_property.attachPostSetElementValueFunctor(makeFunctor((Functor1<const size_t> *)0,*this,&HighPowerSwitchController::setRightSwitchStopEnabledHandler));

  modular_server::Property & switch_soft_stop_enabled_property = modular_server_.createProperty(constants::switch_soft_stop_enabled_property_name,constants::switch_soft_stop_enabled_default);
  switch_soft_stop_enabled_property.attachPostSetElementValueFunctor(makeFunctor((Functor1<const size_t> *)0,*this,&HighPowerSwitchController::setSwitchSoftStopEnabledHandler));

  modular_server::Property & home_velocity_property = modular_server_.createProperty(constants::home_velocity_property_name,constants::home_velocity_default);
  home_velocity_property.setUnits(constants::position_units_per_second_unit);
  home_velocity_property.setRange(-constants::velocity_max_max,constants::velocity_max_max);

  reinitialize();

  // Parameters
  modular_server::Parameter & channel_parameter = modular_server_.createParameter(constants::channel_parameter_name);
  channel_parameter.setRange(constants::channel_min,constants::channel_max);

  modular_server::Parameter & position_parameter = modular_server_.createParameter(constants::position_parameter_name);
  position_parameter.setTypeDouble();

  modular_server::Parameter & velocity_parameter = modular_server_.createParameter(constants::velocity_parameter_name);
  velocity_parameter.setTypeDouble();

  // Functions
  modular_server::Function & reinitialize_function = modular_server_.createFunction(constants::reinitialize_function_name);
  reinitialize_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::reinitializeHandler));

  modular_server::Function & enable_function = modular_server_.createFunction(constants::enable_function_name);
  enable_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::enableHandler));
  enable_function.addParameter(channel_parameter);

  modular_server::Function & disable_function = modular_server_.createFunction(constants::disable_function_name);
  disable_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::disableHandler));
  disable_function.addParameter(channel_parameter);

  modular_server::Function & enable_all_function = modular_server_.createFunction(constants::enable_all_function_name);
  enable_all_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::enableAllHandler));

  modular_server::Function & disable_all_function = modular_server_.createFunction(constants::disable_all_function_name);
  disable_all_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::disableAllHandler));

  modular_server::Function & enabled_function = modular_server_.createFunction(constants::enabled_function_name);
  enabled_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::enabledHandler));
  enabled_function.setReturnTypeArray();

  modular_server::Function & move_by_function = modular_server_.createFunction(constants::move_by_function_name);
  move_by_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::moveByHandler));
  move_by_function.addParameter(channel_parameter);
  move_by_function.addParameter(position_parameter);

  modular_server::Function & move_to_function = modular_server_.createFunction(constants::move_to_function_name);
  move_to_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::moveToHandler));
  move_to_function.addParameter(channel_parameter);
  move_to_function.addParameter(position_parameter);

  modular_server::Function & move_at_function = modular_server_.createFunction(constants::move_at_function_name);
  move_at_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::moveAtHandler));
  move_at_function.addParameter(channel_parameter);
  move_at_function.addParameter(velocity_parameter);

  modular_server::Function & move_softly_by_function = modular_server_.createFunction(constants::move_softly_by_function_name);
  move_softly_by_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::moveSoftlyByHandler));
  move_softly_by_function.addParameter(channel_parameter);
  move_softly_by_function.addParameter(position_parameter);

  modular_server::Function & move_softly_to_function = modular_server_.createFunction(constants::move_softly_to_function_name);
  move_softly_to_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::moveSoftlyToHandler));
  move_softly_to_function.addParameter(channel_parameter);
  move_softly_to_function.addParameter(position_parameter);

  modular_server::Function & stop_function = modular_server_.createFunction(constants::stop_function_name);
  stop_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::stopHandler));
  stop_function.addParameter(channel_parameter);

  modular_server::Function & stop_all_function = modular_server_.createFunction(constants::stop_all_function_name);
  stop_all_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::stopAllHandler));

  modular_server::Function & zero_function = modular_server_.createFunction(constants::zero_function_name);
  zero_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::zeroHandler));
  zero_function.addParameter(channel_parameter);

  modular_server::Function & zero_all_function = modular_server_.createFunction(constants::zero_all_function_name);
  zero_all_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::zeroAllHandler));

  modular_server::Function & get_positions_function = modular_server_.createFunction(constants::get_positions_function_name);
  get_positions_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::getPositionsHandler));
  get_positions_function.setReturnTypeArray();

  modular_server::Function & get_target_positions_function = modular_server_.createFunction(constants::get_target_positions_function_name);
  get_target_positions_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::getTargetPositionsHandler));
  get_target_positions_function.setReturnTypeArray();

  modular_server::Function & at_target_positions_function = modular_server_.createFunction(constants::at_target_positions_function_name);
  at_target_positions_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::atTargetPositionsHandler));
  at_target_positions_function.setReturnTypeArray();

  modular_server::Function & get_velocities_function = modular_server_.createFunction(constants::get_velocities_function_name);
  get_velocities_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::getVelocitiesHandler));
  get_velocities_function.setReturnTypeArray();

  modular_server::Function & get_target_velocities_function = modular_server_.createFunction(constants::get_target_velocities_function_name);
  get_target_velocities_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::getTargetVelocitiesHandler));
  get_target_velocities_function.setReturnTypeArray();

  modular_server::Function & at_target_velocities_function = modular_server_.createFunction(constants::at_target_velocities_function_name);
  at_target_velocities_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::atTargetVelocitiesHandler));
  at_target_velocities_function.setReturnTypeArray();

  modular_server::Function & switches_active_function = modular_server_.createFunction(constants::switches_active_function_name);
  switches_active_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::switchesActiveHandler));
  switches_active_function.setReturnTypeArray();

  modular_server::Function & home_function = modular_server_.createFunction(constants::home_function_name);
  home_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::homeHandler));
  home_function.addParameter(channel_parameter);
  home_function.setReturnTypeBool();

  modular_server::Function & homing_function = modular_server_.createFunction(constants::homing_function_name);
  homing_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::homingHandler));
  homing_function.setReturnTypeArray();

  modular_server::Function & homed_function = modular_server_.createFunction(constants::homed_function_name);
  homed_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&HighPowerSwitchController::homedHandler));
  homed_function.setReturnTypeArray();

  // Callbacks

}

void HighPowerSwitchController::update()
{
  // Parent Update
  ModularDeviceBase::update();

  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    if (homing_[channel])
    {
      size_t controller_i = channelToControllerIndex(channel);
      size_t motor_i = channelToMotorIndex(channel);
      Controller & controller = controllers_[controller_i];
      bool latch_position_waiting = controller.latchPositionWaiting(motor_i);
      if (!latch_position_waiting)
      {
        homing_[channel] = false;
        homed_[channel] = true;
        zero(channel);
      }
    }
  }
}

void HighPowerSwitchController::reinitialize()
{
  for (size_t controller_i=0; controller_i<constants::CONTROLLER_COUNT; ++controller_i)
  {
    Controller & controller = controllers_[controller_i];
    controller.setStepDirOutput();
    for (size_t motor_i=0; motor_i<constants::CHANNELS_PER_CONTROLLER_COUNT; ++motor_i)
    {
      controller.disableSwitchSoftStop(motor_i);
    }
  }
  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    setLimitsHandler(channel);
  }
}

void HighPowerSwitchController::enable(const size_t channel)
{
  if (channel < constants::CHANNEL_COUNT)
  {
    const ConstantString * polarity_ptr;
    modular_server_.property(constants::enable_polarity_property_name).getElementValue(channel,polarity_ptr);
    if (polarity_ptr == &constants::polarity_high)
    {
      digitalWrite(constants::enable_pins[channel],HIGH);
    }
    else
    {
      digitalWrite(constants::enable_pins[channel],LOW);
    }
    enabled_[channel] = true;
  }
}

void HighPowerSwitchController::disable(const size_t channel)
{
  if (channel < constants::CHANNEL_COUNT)
  {
    stop(channel);
    const ConstantString * polarity_ptr;
    modular_server_.property(constants::enable_polarity_property_name).getElementValue(channel,polarity_ptr);
    if (polarity_ptr == &constants::polarity_high)
    {
      digitalWrite(constants::enable_pins[channel],LOW);
    }
    else
    {
      digitalWrite(constants::enable_pins[channel],HIGH);
    }
    enabled_[channel] = false;
  }
}

void HighPowerSwitchController::enableAll()
{
  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    enable(channel);
  }
}

void HighPowerSwitchController::disableAll()
{
  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    disable(channel);
  }
}

uint32_t HighPowerSwitchController::enabled()
{
  uint32_t channels_enabled = 0;
  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    bool channel_enabled = enabled_[channel];
    if (channel_enabled)
    {
      channels_enabled |= 1 << channel;
    }
  }
  return channels_enabled;
}

void HighPowerSwitchController::moveBy(const size_t channel, const double position)
{
  if (channel < constants::CHANNEL_COUNT)
  {
    size_t controller_i = channelToControllerIndex(channel);
    size_t motor_i = channelToMotorIndex(channel);
    Controller & controller = controllers_[controller_i];
    controller.setRampMode(motor_i);
    long position_actual = controller.getActualPosition(motor_i);
    long position_target = positionUnitsToSteps(channel,position) + position_actual;
    controller.setTargetPosition(motor_i,position_target);
    homing_[channel] = false;
  }
}

void HighPowerSwitchController::moveTo(const size_t channel, const double position)
{
  if (channel < constants::CHANNEL_COUNT)
  {
    size_t controller_i = channelToControllerIndex(channel);
    size_t motor_i = channelToMotorIndex(channel);
    Controller & controller = controllers_[controller_i];
    controller.setRampMode(motor_i);
    controller.setTargetPosition(motor_i,positionUnitsToSteps(channel,position));
    homing_[channel] = false;
  }
}

void HighPowerSwitchController::moveAt(const size_t channel, const double velocity)
{
  if (channel < constants::CHANNEL_COUNT)
  {
    size_t controller_i = channelToControllerIndex(channel);
    size_t motor_i = channelToMotorIndex(channel);
    Controller & controller = controllers_[controller_i];
    controller.setVelocityMode(motor_i);
    controller.setTargetVelocityInHz(motor_i,positionUnitsToSteps(channel,velocity));
    homing_[channel] = false;
  }
}

void HighPowerSwitchController::moveSoftlyBy(const size_t channel, const double position)
{
  if (channel < constants::CHANNEL_COUNT)
  {
    size_t controller_i = channelToControllerIndex(channel);
    size_t motor_i = channelToMotorIndex(channel);
    Controller & controller = controllers_[controller_i];
    controller.setSoftMode(motor_i);
    long position_actual = controller.getActualPosition(motor_i);
    long position_target = positionUnitsToSteps(channel,position) + position_actual;
    controller.setTargetPosition(motor_i,position_target);
    homing_[channel] = false;
  }
}

void HighPowerSwitchController::moveSoftlyTo(const size_t channel, const double position)
{
  if (channel < constants::CHANNEL_COUNT)
  {
    size_t controller_i = channelToControllerIndex(channel);
    size_t motor_i = channelToMotorIndex(channel);
    Controller & controller = controllers_[controller_i];
    controller.setSoftMode(motor_i);
    controller.setTargetPosition(motor_i,positionUnitsToSteps(channel,position));
    homing_[channel] = false;
  }
}

void HighPowerSwitchController::stop(const size_t channel)
{
  if (channel < constants::CHANNEL_COUNT)
  {
    size_t controller_i = channelToControllerIndex(channel);
    size_t motor_i = channelToMotorIndex(channel);
    Controller & controller = controllers_[controller_i];
    controller.stop(motor_i);
    homing_[channel] = false;
  }
}

void HighPowerSwitchController::stopAll()
{
  for (size_t controller_i=0; controller_i<constants::CONTROLLER_COUNT; ++controller_i)
  {
    Controller & controller = controllers_[controller_i];
    controller.stopAll();
  }
}

void HighPowerSwitchController::zero(const size_t channel)
{
  if (channel < constants::CHANNEL_COUNT)
  {
    size_t controller_i = channelToControllerIndex(channel);
    size_t motor_i = channelToMotorIndex(channel);
    Controller & controller = controllers_[controller_i];
    controller.setVelocityMode(motor_i);
    controller.setActualPosition(motor_i,0);
    controller.setTargetPosition(motor_i,0);
  }
}

void HighPowerSwitchController::zeroAll()
{
  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    zero(channel);
  }
}

double HighPowerSwitchController::getPosition(const size_t channel)
{
  int32_t position = getPositionInSteps(channel);
  return stepsToPositionUnits(channel,position);
}

double HighPowerSwitchController::getTargetPosition(const size_t channel)
{
  double position = 0;
  if (channel < constants::CHANNEL_COUNT)
  {
    size_t controller_i = channelToControllerIndex(channel);
    size_t motor_i = channelToMotorIndex(channel);
    Controller & controller = controllers_[controller_i];
    position = controller.getTargetPosition(motor_i);
  }
  return stepsToPositionUnits(channel,position);
}

bool HighPowerSwitchController::atTargetPosition(const size_t channel)
{
  bool at_target_position = true;
  if (channel < constants::CHANNEL_COUNT)
  {
    size_t controller_i = channelToControllerIndex(channel);
    size_t motor_i = channelToMotorIndex(channel);
    Controller & controller = controllers_[controller_i];
    at_target_position = controller.atTargetPosition(motor_i);
  }
  return at_target_position;
}

double HighPowerSwitchController::getVelocity(const size_t channel)
{
  int32_t velocity = getVelocityInHz(channel);
  return stepsToPositionUnits(channel,velocity);
}

double HighPowerSwitchController::getTargetVelocity(const size_t channel)
{
  double velocity = 0;
  if (channel < constants::CHANNEL_COUNT)
  {
    size_t controller_i = channelToControllerIndex(channel);
    size_t motor_i = channelToMotorIndex(channel);
    Controller & controller = controllers_[controller_i];
    velocity = controller.getTargetVelocityInHz(motor_i);
  }
  return stepsToPositionUnits(channel,velocity);
}

bool HighPowerSwitchController::atTargetVelocity(const size_t channel)
{
  bool at_target_velocity = true;
  if (channel < constants::CHANNEL_COUNT)
  {
    size_t controller_i = channelToControllerIndex(channel);
    size_t motor_i = channelToMotorIndex(channel);
    Controller & controller = controllers_[controller_i];
    at_target_velocity = controller.atTargetVelocity(motor_i);
  }
  return at_target_velocity;
}

bool HighPowerSwitchController::leftSwitchActive(const size_t channel)
{
  bool left_switch_active = false;
  if (channel < constants::CHANNEL_COUNT)
  {
    size_t controller_i = channelToControllerIndex(channel);
    size_t motor_i = channelToMotorIndex(channel);
    Controller & controller = controllers_[controller_i];
    left_switch_active = controller.leftSwitchActive(motor_i);
  }
  return left_switch_active;
}

bool HighPowerSwitchController::rightSwitchActive(const size_t channel)
{
  bool right_switch_active = false;
  if (channel < constants::CHANNEL_COUNT)
  {
    size_t controller_i = channelToControllerIndex(channel);
    size_t motor_i = channelToMotorIndex(channel);
    Controller & controller = controllers_[controller_i];
    right_switch_active = controller.rightSwitchActive(motor_i);
  }
  return right_switch_active;
}

bool HighPowerSwitchController::home(const size_t channel)
{
  if (channel >= constants::CHANNEL_COUNT)
  {
    return false;
  }
  modular_server::Property & home_velocity_property = modular_server_.property(constants::home_velocity_property_name);
  double home_velocity;
  home_velocity_property.getElementValue(channel,home_velocity);

  size_t controller_i = channelToControllerIndex(channel);
  size_t motor_i = channelToMotorIndex(channel);
  Controller & controller = controllers_[controller_i];

  bool home_switch_enabled;
  if (home_velocity < 0)
  {
    modular_server_.property(constants::left_switch_stop_enabled_property_name).getElementValue(channel,home_switch_enabled);
    if (home_switch_enabled)
    {
      controller.setReferenceSwitchToLeft(motor_i);
      if (leftSwitchActive(channel))
      {
        homing_[channel] = false;
        homed_[channel] = true;
        zero(channel);
        return false;
      }
    }
  }
  else
  {
    bool right_switches_enabled;
    modular_server_.property(constants::right_switches_enabled_property_name).getElementValue(channel,right_switches_enabled);
    bool right_switch_stop_enabled;
    modular_server_.property(constants::right_switch_stop_enabled_property_name).getElementValue(channel,right_switch_stop_enabled);
    home_switch_enabled = right_switches_enabled && right_switch_stop_enabled;
    if (home_switch_enabled)
    {
      controller.setReferenceSwitchToRight(motor_i);
      if (rightSwitchActive(channel))
      {
        homing_[channel] = false;
        homed_[channel] = true;
        zero(channel);
        return false;
      }
    }
  }
  if (!home_switch_enabled)
  {
    return false;
  }

  controller.startLatchPositionWaiting(motor_i);
  moveAt(channel,home_velocity);
  homing_[channel] = true;
  homed_[channel] = false;
  return true;
}

bool HighPowerSwitchController::homing(const size_t channel)
{
  if (channel >= constants::CHANNEL_COUNT)
  {
    return false;
  }
  return homing_[channel];
}

bool HighPowerSwitchController::anyHoming()
{
  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    if (homing_[channel])
    {
      return true;
    }
  }
  return false;
}

bool HighPowerSwitchController::homed(const size_t channel)
{
  if (channel >= constants::CHANNEL_COUNT)
  {
    return false;
  }
  return homed_[channel];
}

double HighPowerSwitchController::stepsToPositionUnits(const size_t channel, const double steps)
{
  modular_server::Property & steps_per_position_unit_property = modular_server_.property(constants::steps_per_position_unit_property_name);
  double steps_per_position_unit;
  steps_per_position_unit_property.getElementValue(channel,steps_per_position_unit);

  return steps/steps_per_position_unit;
}

double HighPowerSwitchController::positionUnitsToSteps(const size_t channel, const double position_units)
{
  modular_server::Property & steps_per_position_unit_property = modular_server_.property(constants::steps_per_position_unit_property_name);
  double steps_per_position_unit;
  steps_per_position_unit_property.getElementValue(channel,steps_per_position_unit);

  return position_units*steps_per_position_unit;
}

int32_t HighPowerSwitchController::getPositionInSteps(const size_t channel)
{
  int32_t position = 0;
  if (channel < constants::CHANNEL_COUNT)
  {
    size_t controller_i = channelToControllerIndex(channel);
    size_t motor_i = channelToMotorIndex(channel);
    Controller & controller = controllers_[controller_i];
    position = controller.getActualPosition(motor_i);
  }
  return position;
}

int32_t HighPowerSwitchController::getVelocityInHz(const size_t channel)
{
  int32_t velocity = 0;
  if (channel < constants::CHANNEL_COUNT)
  {
    size_t controller_i = channelToControllerIndex(channel);
    size_t motor_i = channelToMotorIndex(channel);
    Controller & controller = controllers_[controller_i];
    velocity = controller.getActualVelocityInHz(motor_i);
  }
  return velocity;
}

size_t HighPowerSwitchController::channelToControllerIndex(const size_t channel)
{
  return channel/constants::CHANNELS_PER_CONTROLLER_COUNT;
}

size_t HighPowerSwitchController::channelToMotorIndex(const size_t channel)
{
  return channel%constants::CHANNELS_PER_CONTROLLER_COUNT;
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

void HighPowerSwitchController::preUpdateScaledPropertiesHandler(const size_t channel)
{
  modular_server::Property & velocity_min_property = modular_server_.property(constants::velocity_min_property_name);
  double velocity_min;
  velocity_min_property.getElementValue(channel,velocity_min);
  velocity_min_steps_[channel] = positionUnitsToSteps(channel,velocity_min);

  modular_server::Property & velocity_max_property = modular_server_.property(constants::velocity_max_property_name);
  double velocity_max;
  velocity_max_property.getElementValue(channel,velocity_max);
  velocity_max_steps_[channel] = positionUnitsToSteps(channel,velocity_max);

  modular_server::Property & acceleration_max_property = modular_server_.property(constants::acceleration_max_property_name);
  double acceleration_max;
  acceleration_max_property.getElementValue(channel,acceleration_max);
  acceleration_max_steps_[channel] = positionUnitsToSteps(channel,acceleration_max);

  modular_server::Property & home_velocity_property = modular_server_.property(constants::home_velocity_property_name);
  double home_velocity;
  home_velocity_property.getElementValue(channel,home_velocity);
  home_velocity_steps_[channel] = positionUnitsToSteps(channel,home_velocity);
}

void HighPowerSwitchController::postUpdateScaledPropertiesHandler(const size_t channel)
{
  modular_server::Property & velocity_min_property = modular_server_.property(constants::velocity_min_property_name);

  modular_server::Property & velocity_max_property = modular_server_.property(constants::velocity_max_property_name);

  modular_server::Property & acceleration_max_property = modular_server_.property(constants::acceleration_max_property_name);

  modular_server::Property & home_velocity_property = modular_server_.property(constants::home_velocity_property_name);

  velocity_min_property.disableFunctors();
  velocity_max_property.disableFunctors();
  acceleration_max_property.disableFunctors();
  home_velocity_property.disableFunctors();

  velocity_min_property.setElementValue(channel,stepsToPositionUnits(channel,velocity_min_steps_[channel]));

  velocity_max_property.setElementValue(channel,stepsToPositionUnits(channel,velocity_max_steps_[channel]));

  acceleration_max_property.setElementValue(channel,stepsToPositionUnits(channel,acceleration_max_steps_[channel]));

  home_velocity_property.setElementValue(channel,stepsToPositionUnits(channel,home_velocity_steps_[channel]));

  velocity_min_property.reenableFunctors();
  velocity_max_property.reenableFunctors();
  acceleration_max_property.reenableFunctors();
  home_velocity_property.reenableFunctors();

  setLimitsHandler(channel);
}

void HighPowerSwitchController::setLimitsHandler(const size_t channel)
{
  stopAll();

  modular_server::Property & velocity_min_property = modular_server_.property(constants::velocity_min_property_name);
  double velocity_min;
  velocity_min_property.getElementValue(channel,velocity_min);

  modular_server::Property & velocity_max_property = modular_server_.property(constants::velocity_max_property_name);
  double velocity_max;
  velocity_max_property.getElementValue(channel,velocity_max);

  modular_server::Property & acceleration_max_property = modular_server_.property(constants::acceleration_max_property_name);
  double acceleration_max;
  acceleration_max_property.getElementValue(channel,acceleration_max);

  size_t controller_i = channelToControllerIndex(channel);
  size_t motor_i = channelToMotorIndex(channel);
  Controller & controller = controllers_[controller_i];

  controller.setLimitsInHz(motor_i,
                           positionUnitsToSteps(channel,velocity_min),
                           positionUnitsToSteps(channel,velocity_max),
                           positionUnitsToSteps(channel,acceleration_max));
}

void HighPowerSwitchController::reinitializeHandler()
{
  reinitialize();
}

void HighPowerSwitchController::setStepPolarityInvertedHandler()
{
  bool inverted;
  modular_server_.property(constants::step_polarity_inverted_property_name).getValue(inverted);
  for (size_t controller_i=0; controller_i<constants::CONTROLLER_COUNT; ++controller_i)
  {
    Controller & controller = controllers_[controller_i];
    if (inverted)
    {
      controller.enableInverseStepPolarity();
    }
    else
    {
      controller.disableInverseStepPolarity();
    }
  }
}

void HighPowerSwitchController::setDirPolarityInvertedHandler()
{
  bool inverted;
  modular_server_.property(constants::step_polarity_inverted_property_name).getValue(inverted);
  for (size_t controller_i=0; controller_i<constants::CONTROLLER_COUNT; ++controller_i)
  {
    Controller & controller = controllers_[controller_i];
    if (inverted)
    {
      controller.enableInverseDirPolarity();
    }
    else
    {
      controller.disableInverseDirPolarity();
    }
  }
}

void HighPowerSwitchController::setSwitchActivePolarityHandler()
{
  const ConstantString * polarity_ptr;
  modular_server_.property(constants::switch_active_polarity_property_name).getValue(polarity_ptr);
  for (size_t controller_i=0; controller_i<constants::CONTROLLER_COUNT; ++controller_i)
  {
    Controller & controller = controllers_[controller_i];
    if (polarity_ptr == &constants::polarity_high)
    {
      controller.setSwitchesActiveHigh();
    }
    else if (polarity_ptr == &constants::polarity_low)
    {
      controller.setSwitchesActiveLow();
    }
  }
}

void HighPowerSwitchController::setLeftSwitchStopEnabledHandler(const size_t channel)
{
  bool enabled;
  modular_server_.property(constants::left_switch_stop_enabled_property_name).getElementValue(channel,enabled);
  size_t controller_i = channelToControllerIndex(channel);
  size_t motor_i = channelToMotorIndex(channel);
  Controller & controller = controllers_[controller_i];
  if (enabled)
  {
    controller.enableLeftSwitchStop(motor_i);
  }
  else
  {
    controller.disableLeftSwitchStop(motor_i);
  }
}

void HighPowerSwitchController::setRightSwitchesEnabledHandler()
{
  bool enabled;
  modular_server_.property(constants::right_switches_enabled_property_name).getValue(enabled);
  for (size_t controller_i=0; controller_i<constants::CONTROLLER_COUNT; ++controller_i)
  {
    Controller & controller = controllers_[controller_i];
    if (enabled)
    {
      controller.enableRightSwitches();
    }
    else
    {
      controller.disableRightSwitches();
    }
  }
}

void HighPowerSwitchController::setRightSwitchStopEnabledHandler(const size_t channel)
{
  bool enabled;
  modular_server_.property(constants::right_switch_stop_enabled_property_name).getElementValue(channel,enabled);
  size_t controller_i = channelToControllerIndex(channel);
  size_t motor_i = channelToMotorIndex(channel);
  Controller & controller = controllers_[controller_i];
  if (enabled)
  {
    controller.enableRightSwitchStop(motor_i);
  }
  else
  {
    controller.disableRightSwitchStop(motor_i);
  }
}

void HighPowerSwitchController::setSwitchSoftStopEnabledHandler(const size_t channel)
{
  bool enabled;
  modular_server_.property(constants::right_switch_stop_enabled_property_name).getElementValue(channel,enabled);
  size_t controller_i = channelToControllerIndex(channel);
  size_t motor_i = channelToMotorIndex(channel);
  Controller & controller = controllers_[controller_i];
  if (enabled)
  {
    controller.enableSwitchSoftStop(motor_i);
  }
  else
  {
    controller.disableSwitchSoftStop(motor_i);
  }
}

void HighPowerSwitchController::enableHandler()
{
  long channel;
  modular_server_.parameter(constants::channel_parameter_name).getValue(channel);
  enable(channel);
}

void HighPowerSwitchController::disableHandler()
{
  long channel;
  modular_server_.parameter(constants::channel_parameter_name).getValue(channel);
  disable(channel);
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
  uint32_t channels_enabled = enabled();
  modular_server_.response().writeResultKey();
  modular_server_.response().beginArray();
  int bit = 1;
  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    bool channel_enabled = (bit << channel) & channels_enabled;
    modular_server_.response().write(channel_enabled);
  }
  modular_server_.response().endArray();
}

void HighPowerSwitchController::moveByHandler()
{
  long channel;
  modular_server_.parameter(constants::channel_parameter_name).getValue(channel);
  double position;
  modular_server_.parameter(constants::position_parameter_name).getValue(position);
  moveBy(channel,position);
}

void HighPowerSwitchController::moveToHandler()
{
  long channel;
  modular_server_.parameter(constants::channel_parameter_name).getValue(channel);
  double position;
  modular_server_.parameter(constants::position_parameter_name).getValue(position);
  moveTo(channel,position);
}

void HighPowerSwitchController::moveAtHandler()
{
  long channel;
  modular_server_.parameter(constants::channel_parameter_name).getValue(channel);
  double velocity;
  modular_server_.parameter(constants::velocity_parameter_name).getValue(velocity);
  moveAt(channel,velocity);
}

// void HighPowerSwitchController::moveByAtHandler()
// {
//   long channel;
//   modular_server_.parameter(constants::channel_parameter_name).getValue(channel);
//   long position;
//   modular_server_.parameter(constants::position_parameter_name).getValue(position);
//   long velocity;
//   modular_server_.parameter(constants::velocity_parameter_name).getValue(velocity);
//   moveByAt(channel,position,velocity);
// }

// void HighPowerSwitchController::moveToAtHandler()
// {
//   long channel;
//   modular_server_.parameter(constants::channel_parameter_name).getValue(channel);
//   long position;
//   modular_server_.parameter(constants::position_parameter_name).getValue(position);
//   long velocity;
//   modular_server_.parameter(constants::velocity_parameter_name).getValue(velocity);
//   moveToAt(channel,position,velocity);
// }

void HighPowerSwitchController::moveSoftlyByHandler()
{
  long channel;
  modular_server_.parameter(constants::channel_parameter_name).getValue(channel);
  double position;
  modular_server_.parameter(constants::position_parameter_name).getValue(position);
  moveSoftlyBy(channel,position);
}

void HighPowerSwitchController::moveSoftlyToHandler()
{
  long channel;
  modular_server_.parameter(constants::channel_parameter_name).getValue(channel);
  double position;
  modular_server_.parameter(constants::position_parameter_name).getValue(position);
  moveSoftlyTo(channel,position);
}

void HighPowerSwitchController::stopHandler()
{
  long channel;
  modular_server_.parameter(constants::channel_parameter_name).getValue(channel);
  stop(channel);
}

void HighPowerSwitchController::stopAllHandler()
{
  stopAll();
}

void HighPowerSwitchController::zeroHandler()
{
  long channel;
  modular_server_.parameter(constants::channel_parameter_name).getValue(channel);
  zero(channel);
}

void HighPowerSwitchController::zeroAllHandler()
{
  zeroAll();
}

void HighPowerSwitchController::getPositionsHandler()
{
  double position;
  modular_server_.response().writeResultKey();
  modular_server_.response().beginArray();
  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    position = getPosition(channel);
    modular_server_.response().write(position);
  }
  modular_server_.response().endArray();
}

void HighPowerSwitchController::getTargetPositionsHandler()
{
  double position;
  modular_server_.response().writeResultKey();
  modular_server_.response().beginArray();
  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    position = getPosition(channel);
    modular_server_.response().write(position);
  }
  modular_server_.response().endArray();
}

void HighPowerSwitchController::atTargetPositionsHandler()
{
  bool at_target_position;
  modular_server_.response().writeResultKey();
  modular_server_.response().beginArray();
  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    at_target_position = atTargetPosition(channel);
    modular_server_.response().write(at_target_position);
  }
  modular_server_.response().endArray();
}

void HighPowerSwitchController::getVelocitiesHandler()
{
  double velocity;
  modular_server_.response().writeResultKey();
  modular_server_.response().beginArray();
  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    velocity = getVelocity(channel);
    modular_server_.response().write(velocity);
  }
  modular_server_.response().endArray();
}

void HighPowerSwitchController::getTargetVelocitiesHandler()
{
  double velocity;
  modular_server_.response().writeResultKey();
  modular_server_.response().beginArray();
  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    velocity = getTargetVelocity(channel);
    modular_server_.response().write(velocity);
  }
  modular_server_.response().endArray();
}

void HighPowerSwitchController::atTargetVelocitiesHandler()
{
  bool at_target_velocity;
  modular_server_.response().writeResultKey();
  modular_server_.response().beginArray();
  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    at_target_velocity = atTargetVelocity(channel);
    modular_server_.response().write(at_target_velocity);
  }
  modular_server_.response().endArray();
}

void HighPowerSwitchController::switchesActiveHandler()
{
  modular_server_.response().writeResultKey();
  modular_server_.response().beginArray();
  for (size_t controller_i=0; controller_i<constants::CONTROLLER_COUNT; ++controller_i)
  {
    Controller & controller = controllers_[controller_i];
    for (size_t motor_i=0; motor_i<constants::CHANNELS_PER_CONTROLLER_COUNT; ++motor_i)
    {
      modular_server_.response().beginObject();
      bool left_switch_active = controller.leftSwitchActive(motor_i);
      modular_server_.response().write(constants::left_constant_string,left_switch_active);
      bool right_switch_active = controller.rightSwitchActive(motor_i);
      modular_server_.response().write(constants::right_constant_string,right_switch_active);
      modular_server_.response().endObject();
    }
  }
  modular_server_.response().endArray();
}

void HighPowerSwitchController::homeHandler()
{
  long channel;
  modular_server_.parameter(constants::channel_parameter_name).getValue(channel);
  bool is_homing = home(channel);
  modular_server_.response().returnResult(is_homing);
}

void HighPowerSwitchController::homingHandler()
{
  bool is_homing;
  modular_server_.response().writeResultKey();
  modular_server_.response().beginArray();
  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    is_homing = homing(channel);
    modular_server_.response().write(is_homing);
  }
  modular_server_.response().endArray();
}

void HighPowerSwitchController::homedHandler()
{
  bool is_homed;
  modular_server_.response().writeResultKey();
  modular_server_.response().beginArray();
  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    is_homed = homed(channel);
    modular_server_.response().write(is_homed);
  }
  modular_server_.response().endArray();
}

