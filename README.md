# HighPowerSwitchController

Authors:

    Peter Polidoro <polidorop@janelia.hhmi.org>

License:

    BSD

## Ancestors

[ModularServer](https://github.com/janelia-arduino/ModularServer)

[ModularDeviceBase](https://github.com/janelia-arduino/ModularDeviceBase)

## Clients

## Devices

[modular_device_base](https://github.com/janelia-modular-devices/modular_device_base.git)

[high_power_switch_controller](https://github.com/janelia-modular-devices/high_power_switch_controller.git)

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
      "setChannelOnAtPower",
      "setChannelOff",
      "setChannelsOn",
      "setChannelsOnAtPower",
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

## API Verbose

```json
{
  "id":"getApiVerbose",
  "result":{
    "firmware":["HighPowerSwitchController"],
    "functions":[
      {
        "name":"enableAll",
        "parameters":[]
      },
      {
        "name":"disableAll",
        "parameters":[]
      },
      {
        "name":"enabled",
        "parameters":[],
        "result_info":{
          "type":"bool"
        }
      },
      {
        "name":"setPowerWhenOn",
        "parameters":[
          "channel",
          "power"
        ],
        "result_info":{
          "type":"long"
        }
      },
      {
        "name":"setPowersWhenOn",
        "parameters":[
          "powers"
        ],
        "result_info":{
          "type":"array",
          "array_element_type":"long"
        }
      },
      {
        "name":"setAllPowersWhenOn",
        "parameters":[
          "power"
        ],
        "result_info":{
          "type":"array",
          "array_element_type":"long"
        }
      },
      {
        "name":"getPowersWhenOn",
        "parameters":[],
        "result_info":{
          "type":"array",
          "array_element_type":"long"
        }
      },
      {
        "name":"getPowers",
        "parameters":[],
        "result_info":{
          "type":"array",
          "array_element_type":"long"
        }
      },
      {
        "name":"setChannelOn",
        "parameters":[
          "channel"
        ]
      },
      {
        "name":"setChannelOnAtPower",
        "parameters":[
          "channel",
          "power"
        ]
      },
      {
        "name":"setChannelOff",
        "parameters":[
          "channel"
        ]
      },
      {
        "name":"setChannelsOn",
        "parameters":[
          "channels"
        ]
      },
      {
        "name":"setChannelsOnAtPower",
        "parameters":[
          "channels",
          "power"
        ]
      },
      {
        "name":"setChannelsOff",
        "parameters":[
          "channels"
        ]
      },
      {
        "name":"toggleChannel",
        "parameters":[
          "channel"
        ]
      },
      {
        "name":"toggleChannels",
        "parameters":[
          "channels"
        ]
      },
      {
        "name":"toggleAllChannels",
        "parameters":[]
      },
      {
        "name":"setAllChannelsOn",
        "parameters":[]
      },
      {
        "name":"setAllChannelsOff",
        "parameters":[]
      },
      {
        "name":"setChannelOnAllOthersOff",
        "parameters":[
          "channel"
        ]
      },
      {
        "name":"setChannelOffAllOthersOn",
        "parameters":[
          "channel"
        ]
      },
      {
        "name":"setChannelsOnAllOthersOff",
        "parameters":[
          "channels"
        ]
      },
      {
        "name":"setChannelsOffAllOthersOn",
        "parameters":[
          "channels"
        ]
      },
      {
        "name":"channelIsOn",
        "parameters":[
          "channel"
        ],
        "result_info":{
          "type":"bool"
        }
      },
      {
        "name":"getChannelsOn",
        "parameters":[],
        "result_info":{
          "type":"array",
          "array_element_type":"long"
        }
      },
      {
        "name":"getChannelsOff",
        "parameters":[],
        "result_info":{
          "type":"array",
          "array_element_type":"long"
        }
      },
      {
        "name":"getChannelCount",
        "parameters":[]
      },
      {
        "name":"addPwm",
        "parameters":[
          "channels",
          "delay",
          "period",
          "on_duration",
          "count"
        ],
        "result_info":{
          "type":"long"
        }
      },
      {
        "name":"startPwm",
        "parameters":[
          "channels",
          "delay",
          "period",
          "on_duration"
        ],
        "result_info":{
          "type":"long"
        }
      },
      {
        "name":"addRecursivePwm",
        "parameters":[
          "channels",
          "delay",
          "periods",
          "on_durations",
          "count"
        ],
        "result_info":{
          "type":"long"
        }
      },
      {
        "name":"startRecursivePwm",
        "parameters":[
          "channels",
          "delay",
          "periods",
          "on_durations"
        ],
        "result_info":{
          "type":"long"
        }
      },
      {
        "name":"stopPwm",
        "parameters":[
          "pwm_index"
        ]
      },
      {
        "name":"stopAllPwm",
        "parameters":[]
      },
      {
        "name":"getChannelsPwmIndexes",
        "parameters":[],
        "result_info":{
          "type":"array",
          "array_element_type":"long"
        }
      },
      {
        "name":"getPwmInfo",
        "parameters":[],
        "result_info":{
          "type":"array",
          "array_element_type":"object"
        }
      }
    ],
    "parameters":[
      {
        "name":"channel",
        "type":"long"
      },
      {
        "name":"channels",
        "type":"array",
        "array_element_type":"long"
      },
      {
        "name":"power",
        "type":"long",
        "units":"%"
      },
      {
        "name":"powers",
        "type":"array",
        "array_element_type":"long",
        "units":"%"
      },
      {
        "name":"delay",
        "type":"long",
        "units":"ms"
      },
      {
        "name":"period",
        "type":"long",
        "units":"ms"
      },
      {
        "name":"on_duration",
        "type":"long",
        "units":"ms"
      },
      {
        "name":"count",
        "type":"long",
        "units":"ms"
      },
      {
        "name":"pwm_index",
        "type":"long"
      },
      {
        "name":"periods",
        "type":"array",
        "array_element_type":"long",
        "units":"ms"
      },
      {
        "name":"on_durations",
        "type":"array",
        "array_element_type":"long",
        "units":"ms"
      }
    ],
    "properties":[
      {
        "name":"powerMax",
        "type":"array",
        "array_element_type":"long",
        "units":"%",
        "functions":[
          "getValue",
          "setValue",
          "getDefaultValue",
          "setValueToDefault",
          "getElementValue",
          "setElementValue",
          "getDefaultElementValue",
          "setElementValueToDefault",
          "setAllElementValues",
          "getArrayLength",
          "setArrayLength"
        ],
        "parameters":[
          "value",
          "element_index",
          "element_value",
          "array_length"
        ]
      }
    ],
    "callbacks":[]
  }
}
```

## More Detailed Modular Device Information

[modular-devices](https://github.com/janelia-modular-devices/modular-devices)

## Install Library Dependencies

[arduino-libraries](https://github.com/janelia-arduino/arduino-libraries)
