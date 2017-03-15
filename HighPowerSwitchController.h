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

#include "EventController.h"

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

  long setPowerWhenOn(const size_t channel, const long power);
  long getPowerWhenOn(const size_t channel);
  long getPower(const size_t channel);

  void setChannels(const uint32_t channels);
  void setChannelOn(const size_t channel);
  void setChannelOff(const size_t channel);
  void setChannelsOn(const uint32_t channels);
  void setChannelsOff(const uint32_t channels);
  void toggleChannel(const size_t channel);
  void toggleChannels(const uint32_t channels);
  void toggleAllChannels();
  void setAllChannelsOn();
  void setAllChannelsOff();
  void setChannelOnAllOthersOff(const size_t channel);
  void setChannelOffAllOthersOn(const size_t channel);
  void setChannelsOnAllOthersOff(const uint32_t channels);
  void setChannelsOffAllOthersOn(const uint32_t channels);
  bool channelIsOn(const size_t channel);
  uint32_t getChannelsOn();
  size_t getChannelCount();

  int addPwm(const uint32_t channels,
             const long delay,
             const long period,
             const long on_duration,
             const long count);
  int startPwm(const uint32_t channels,
               const long delay,
               const long period,
               const long on_duration);

  typedef Array<long,high_power_switch_controller::constants::PWM_LEVEL_COUNT_MAX> RecursivePwmValues;

  int addRecursivePwm(const uint32_t channels,
                      RecursivePwmValues delays,
                      RecursivePwmValues periods,
                      RecursivePwmValues on_durations,
                      const long count);
  int startRecursivePwm(const uint32_t channels,
                        RecursivePwmValues delays,
                        RecursivePwmValues periods,
                        RecursivePwmValues on_durations);

  void stopPwm(const int pwm_index);
  void stopAllPwm();

  typedef Array<RecursivePwmValues,high_power_switch_controller::constants::CHANNEL_COUNT> PwmStatus;
  PwmStatus getPwmStatus();

  uint32_t arrayToChannels(ArduinoJson::JsonArray & channels_array);

  // Handlers
  virtual void startPwmHandler(int index);
  virtual void stopPwmHandler(int index);

private:
  modular_server::Interrupt interrupts_[high_power_switch_controller::constants::INTERRUPT_COUNT_MAX];

  modular_server::Property properties_[high_power_switch_controller::constants::PROPERTY_COUNT_MAX];
  modular_server::Parameter parameters_[high_power_switch_controller::constants::PARAMETER_COUNT_MAX];
  modular_server::Function functions_[high_power_switch_controller::constants::FUNCTION_COUNT_MAX];
  modular_server::Callback callbacks_[high_power_switch_controller::constants::CALLBACK_COUNT_MAX];

  bool enabled_;
  uint32_t channels_;
  long powers_[high_power_switch_controller::constants::CHANNEL_COUNT];
  long pwm_status_[high_power_switch_controller::constants::CHANNEL_COUNT][high_power_switch_controller::constants::PWM_LEVEL_COUNT_MAX];

  EventController<high_power_switch_controller::constants::EVENT_COUNT_MAX> event_controller_;

  IndexedContainer<high_power_switch_controller::constants::PwmInfo,
                   high_power_switch_controller::constants::INDEXED_PWM_COUNT_MAX> indexed_pwm_;

  long powerToAnalogWriteValue(const long power);
  void setPowersToMax();

  void updateChannel(const size_t channel);
  void updateAllChannels();

  void setChannelPwmStatusRunning(size_t channel, size_t level);
  void setChannelsPwmStatusRunning(uint32_t channels, size_t level);
  void setChannelPwmStatusStopped(size_t channel, size_t level);
  void setChannelsPwmStatusStopped(uint32_t channels, size_t level);
  void setAllPwmStatusStopped();

  // Handlers
  void setPowerMaxHandler(const size_t channel);
  void enableAllHandler();
  void disableAllHandler();
  void enabledHandler();
  void setPowerWhenOnHandler();
  void setPowersWhenOnHandler();
  void setAllPowersWhenOnHandler();
  void getPowersWhenOnHandler();
  void getPowersHandler();
  void setChannelOnHandler();
  void setChannelOffHandler();
  void setChannelsOnHandler();
  void setChannelsOffHandler();
  void toggleChannelHandler();
  void toggleChannelsHandler();
  void toggleAllChannelsHandler();
  void setAllChannelsOnHandler();
  void setAllChannelsOffHandler();
  void setChannelOnAllOthersOffHandler();
  void setChannelOffAllOthersOnHandler();
  void setChannelsOnAllOthersOffHandler();
  void setChannelsOffAllOthersOnHandler();
  void channelIsOnHandler();
  void getChannelsOnHandler();
  void getChannelsOffHandler();
  void getChannelCountHandler();
  void addPwmHandler();
  void startPwmHandler();
  void addRecursivePwmHandler();
  void startRecursivePwmHandler();
  void stopPwmHandler();
  void stopAllPwmHandler();
  void getPwmStatusHandler();
  void setChannelsOnHandler(int index);
  void setChannelsOffHandler(int index);
  void startRecursivePwmHandler(int index);
  void stopRecursivePwmHandler(int index);

};

#endif
