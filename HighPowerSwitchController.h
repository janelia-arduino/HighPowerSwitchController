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

  void enableAll();
  void disableAll();
  bool enabled();
  void setChannelOn(const size_t channel);
  void setChannelOff(const size_t channel);
  void setAllChannelsOn();
  void setAllChannelsOff();

private:
  modular_server::Interrupt interrupts_[high_power_switch_controller::constants::INTERRUPT_COUNT_MAX];

  modular_server::Property properties_[high_power_switch_controller::constants::PROPERTY_COUNT_MAX];
  modular_server::Parameter parameters_[high_power_switch_controller::constants::PARAMETER_COUNT_MAX];
  modular_server::Function functions_[high_power_switch_controller::constants::FUNCTION_COUNT_MAX];
  modular_server::Callback callbacks_[high_power_switch_controller::constants::CALLBACK_COUNT_MAX];

  bool enabled_;

  // Handlers
  void enableAllHandler();
  void disableAllHandler();
  void enabledHandler();
  void setChannelOnHandler();
  void setChannelOffHandler();

};

#endif
