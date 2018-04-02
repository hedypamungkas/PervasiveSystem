# PervasiveSystem

This research using pervasive system who can solve these challenges because it can make the computing processs becomes doesn’t feel and run automatically, so don’t need to specify a set of device configuration. 

This system is designed into several parts of the sensor device, actuator and gateway. Sensor and actuator device will recognize their identity to gateway to register the devices using MQTT. 

Gateway will perform the process of making relationship between both devices based on same location and other identity parameter. 

Actuator device will perform a specific action based on sensor data. From the result of the testing, each device successfully recognize the identity to gateway and the gateway succeeded in making relationship, so both devices can communicate well. 

The actuator device successfully receive sensor data within same relation and succeeds to trigger action to lamp.

## Deployment

```
NodeMCU as Microcontroller
PIR (Passive Infrared Sensor) as Motion Sensor
LDR (Light Dependent Sensor) as Light Sensor
MQTT (Message Queuing Telemetry Transport) as Communication Protocol
Platform.io as Cross Platfrom IDE
C++ as Embedded Language
```
