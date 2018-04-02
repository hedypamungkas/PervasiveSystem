//
//  main.h
//  PervasiveSystemProject
//
//  Created by Hedy Pamungkas on 9/8/17.
//  Copyright © 2017 Hedy Pamungkas. All rights reserved.
//

#ifndef main_h
#define main_h

#include "Arduino.h"

//setup wifi and mqtt
const char *WIFI_SSID = "........";
const char *WIFI_PASSWORD = "........";
const char *MQTT_SERVER_NAME = "ngehubx.online";
const char *MQTT_USERNAME = "admintes";
const char *MQTT_PASSWORD = "admin123";

//indexing method
void setupDevice(int);
void setupService(int);
void setup();
void loop();
void setupBroadcast(int);
void connect();
void messageReceived(String, String, char, unsigned int);
void updateTopicRelational(String);
void getDataFromTopicRelational(String, String);
String getServicesFromDeviceName(String);
bool actionActuator();
void publishData();

#endif /* main_h */
