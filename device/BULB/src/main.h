//
//  main.h
//  PervasiveSystemProject
//
//  Created by Hedy Pamungkas on 9/8/17.
//  Copyright © 2017 Hedy Pamungkas. All rights reserved.
//

#ifndef main_h
#define main_h

#include <Arduino.h>

//setup wifi and mqtt
// const char *WIFI_SSID = "Theex-HQ";
// const char *WIFI_PASSWORD = "JuraganPeceLyeye";
const char *WIFI_SSID = "Hedy's iPhone";
const char *WIFI_PASSWORD = "cumangbuatcobacoba";
const char *MQTT_SERVER_NAME = "ec2-13-229-110-14.ap-southeast-1.compute.amazonaws.com";
const char *MQTT_USERNAME = "admintes";
const char *MQTT_PASSWORD = "admin123";

//setup pin
int dataCounter = 0;
int currentValidCounter = 1;
int faultCounter = 0;
int deviceTotalIntegrate = 0;
int maximumFaultTolerance = 5;
const long intervalTimeout = 20000;
unsigned long currentMillisKeepAlive = 0;
unsigned long previousMillisKeepAlive[] = {0, 0};
bool integrateReady = false;
bool dataReady = false;
bool updateIntegrationSuccess = false;
#define pin D0

//indexing method
void setupDevice(int);
void setupService(int);
void messageReceived(String&, String&);
void setup();
void loop();
void validateDataModel();
void validateDataModelIntegration();
void checkDeviceDataIntegration();
void setupBroadcast(int);
void connect();
void keepAliveState();
void updateTopicRelational(String);
void updateIntegrationtoGateway(String);
void getDataFromTopicRelational(String, String);
String getServicesFromDeviceName(String);
bool actionActuator();
int calculateCurrentDeviceIntegrate();
bool isLDR(int);
int actionActuatorWithSwipeCondition(int);
int actionActuatorWithoutSwipeCondition(int, int);
int transformDataRelay(int);
void publishData();
String getDataFromSensor();

#endif /* main_h */