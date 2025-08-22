# actor-PeriodicActionTrigger
This is a utility actor that can trigger any other action on the device at regular intervals.

## Constructor
The constructor takes two parameters:
* `name`: The name of the device (standard for actors)
* `configFile`: An optional name for the file to store the configuration in (should be unique for each instance)

## Actions
This device has the following actions available to it:
| Action ID | Action name   | Action Payload | Return Value              | Description                                        |
| --------- | ------------- | -------------- | ------------------------- | -------------------------------------------------- |
| 0         | triggeraction | Any string     | JSON `success` as boolean | Triggers the set action, passing the payload to it |

## Configuration
Once a device hub is set up, these are the configuration parameters that can be set in the web interface:
* `Name`: The name of this device
* `taskPeriod`: An integer number of milliseconds for the period of how often the task runs (e.g. 1000 would be once a second)
* `taskEnabled`: A checkbox to enable or disable the task
* `Action`: A drop-down list of available actions to trigger each period, one can be selected
* `Payload`: A string payload that is passed to the action triggered. Refer to each actor's documentation to see what payloads are expected
