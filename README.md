# HighPowerSwitchController

Authors:

    Peter Polidoro <polidorop@janelia.hhmi.org>

License:

    BSD

[Examples](./examples)

## API

```json
{
  "id":"getApi",
  "result":{
    "firmware":["HighPowerSwitchController"],
    "functions":[
      "enableAll",
      "disableAll",
      "enabled",
      "setPowerWhenOn",
      "setPowersWhenOn",
      "setAllPowersWhenOn",
      "getPowersWhenOn",
      "getPowers",
      "setChannelOn",
      "setChannelOff",
      "setChannelsOn",
      "setChannelsOff",
      "toggleChannel",
      "toggleChannels",
      "toggleAllChannels",
      "setAllChannelsOn",
      "setAllChannelsOff",
      "setChannelOnAllOthersOff",
      "setChannelOffAllOthersOn",
      "setChannelsOnAllOthersOff",
      "setChannelsOffAllOthersOn",
      "channelIsOn",
      "getChannelsOn",
      "getChannelsOff",
      "getChannelCount",
      "addPwm",
      "startPwm",
      "addRecursivePwm",
      "startRecursivePwm",
      "stopPwm",
      "stopAllPwm",
      "getChannelsPwmIndexes",
      "getPwmInfo"
    ],
    "parameters":[
      "channel",
      "channels",
      "power",
      "powers",
      "delay",
      "period",
      "on_duration",
      "count",
      "pwm_index",
      "periods",
      "on_durations"
    ],
    "properties":[
      "powerMax"
    ],
    "callbacks":[]
  }
}
```

## More Detailed Modular Device Information

[modular-devices](https://github.com/janelia-modular-devices/modular-devices)

## Install Library Dependencies

[arduino-libraries](https://github.com/janelia-arduino/arduino-libraries)
