// ----------------------------------------------------------------------------
// HighPowerSwitchController.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef HIGH_POWER_SWITCH_CONTROLLER_H
#define HIGH_POWER_SWITCH_CONTROLLER_H
#include "Flash.h"
#include <EEPROM.h>
#include "Streaming.h"
#include "ArduinoJson.h"
#include "JsonSanitizer.h"
#include "JsonStream.h"
#include "Array.h"
#include "Vector.h"
#include "MemoryFree.h"
#include "ConstantVariable.h"
#include "SavedVariable.h"
#include "Functor.h"
#include "IndexedContainer.h"
#include "FunctorCallbacks.h"

#include "ModularServer.h"
#include "ModularDeviceBase.h"

#include "utility/Constants.h"


class HighPowerSwitchController : public ModularDeviceBase
{
public:
  HighPowerSwitchController();
  ~HighPowerSwitchController();
  virtual void setup();
  virtual void update();

  virtual void reinitialize();

  void enable(const size_t channel);
  void disable(const size_t channel);
  void enableAll();
  void disableAll();
  uint32_t enabled();

  void moveBy(const size_t channel, const double position);
  void moveTo(const size_t channel, const double position);
  void moveAt(const size_t channel, const double velocity);
  void moveSoftlyBy(const size_t channel, const double position);
  void moveSoftlyTo(const size_t channel, const double position);
  void stop(const size_t channel);
  void stopAll();

  void zero(const size_t channel);
  void zeroAll();

  double getPosition(const size_t channel);
  double getTargetPosition(const size_t channel);
  bool atTargetPosition(const size_t channel);

  double getVelocity(const size_t channel);
  double getTargetVelocity(const size_t channel);
  bool atTargetVelocity(const size_t channel);

  bool leftSwitchActive(const size_t channel);
  bool rightSwitchActive(const size_t channel);

  bool home(const size_t channel);
  bool homing(const size_t channel);
  bool anyHoming();
  bool homed(const size_t channel);

protected:
  virtual double stepsToPositionUnits(const size_t channel, const double steps);
  virtual double positionUnitsToSteps(const size_t channel, const double position_units);

  int32_t getPositionInSteps(const size_t channel);
  int32_t getVelocityInHz(const size_t channel);

  // Handlers
  void preUpdateScaledPropertiesHandler(const size_t channel);
  void postUpdateScaledPropertiesHandler(const size_t channel);

private:
  modular_server::Interrupt interrupts_[high_power_switch_controller::constants::INTERRUPT_COUNT_MAX];

  modular_server::Property properties_[high_power_switch_controller::constants::PROPERTY_COUNT_MAX];
  modular_server::Parameter parameters_[high_power_switch_controller::constants::PARAMETER_COUNT_MAX];
  modular_server::Function functions_[high_power_switch_controller::constants::FUNCTION_COUNT_MAX];
  modular_server::Callback callbacks_[high_power_switch_controller::constants::CALLBACK_COUNT_MAX];

  typedef TMC429 Controller;
  Controller controllers_[high_power_switch_controller::constants::CONTROLLER_COUNT];
  bool enabled_[high_power_switch_controller::constants::CHANNEL_COUNT];
  bool homing_[high_power_switch_controller::constants::CHANNEL_COUNT];
  bool homed_[high_power_switch_controller::constants::CHANNEL_COUNT];

  double velocity_min_steps_[high_power_switch_controller::constants::CHANNEL_COUNT];
  double velocity_max_steps_[high_power_switch_controller::constants::CHANNEL_COUNT];
  double acceleration_max_steps_[high_power_switch_controller::constants::CHANNEL_COUNT];
  double home_velocity_steps_[high_power_switch_controller::constants::CHANNEL_COUNT];

  size_t channelToControllerIndex(const size_t channel);
  size_t channelToMotorIndex(const size_t channel);

  // Handlers
  void setLimitsHandler(const size_t channel);
  void reinitializeHandler();
  void setStepPolarityInvertedHandler();
  void setDirPolarityInvertedHandler();
  void setSwitchActivePolarityHandler();
  void setLeftSwitchStopEnabledHandler(const size_t channel);
  void setRightSwitchesEnabledHandler();
  void setRightSwitchStopEnabledHandler(const size_t channel);
  void setSwitchSoftStopEnabledHandler(const size_t channel);
  void enableHandler();
  void disableHandler();
  void enableAllHandler();
  void disableAllHandler();
  void enabledHandler();
  void moveByHandler();
  void moveToHandler();
  void moveAtHandler();
  void moveSoftlyByHandler();
  void moveSoftlyToHandler();
  void stopHandler();
  void stopAllHandler();
  void zeroHandler();
  void zeroAllHandler();
  void getPositionsHandler();
  void getTargetPositionsHandler();
  void atTargetPositionsHandler();
  void getVelocitiesHandler();
  void getTargetVelocitiesHandler();
  void atTargetVelocitiesHandler();
  void switchesActiveHandler();
  void homeHandler();
  void homingHandler();
  void homedHandler();

};

#endif
