//
//  main.cpp
//  PervasiveSystemProject
//
//  Created by Hedy Pamungkas on 9/8/17.
//  Copyright © 2017 Hedy Pamungkas. All rights reserved.
//

#include <iostream>
#include "main.h"
#include "MQTTPervasiveSystem.h"
#include "Arduino.h"
#include "ArduinoJson.h"
#include "MQTTClient.h"
#include "ESP8266WiFi.h"
#include "metadata.h"

PervasiveDevice device;
PervasiveService service[2];

WiFiClient net;
MQTTClient client;

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(9600);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    client.begin(MQTT_SERVER_NAME, net);
    
    // connect();
    while (!client.connected())
    {
        connect();
    }
    
    // SETUP DEVICE
    // 0 = mode actuator, 1 = mode sensor
    setupDevice(DEVICE_MODE);
    for (int i = 0; i < DEVICE_SERVICE_TOTAL; i++)
    {
        setupService(i);
    }
    delay(2000);
    
    while (!device.deviceConnected())
    {
        // SETUP BROADCAST
        setupBroadcast(DEVICE_MODE);
        client.subscribe(device.getAckTopic());
        
        delay(5000);
        /*
         - while
         - publish metadata to PROJECT_BROADCAST_TOPIC
         - get msg from subscribe this.ackTopic
         - if 200 set true, else set false
         - end
         */
    }
    
    //after that, return OK!
}

void loop()
{
    // put your main code here, to run repeatedly:
    client.loop();
    delay(10); // <- fixes some issues with WiFi stability
    
    if (!client.connected())
    {
        connect();
    }
    
    if (device.getDeviceType() == KEY_ACTUATOR)
    {
        // looping subscribe actuator
        for (int i = 0; i < DEVICE_INTEGRATION_MAX; i++)
        {
            //tambahin subscribe ack
            client.subscribe(device.getPervasiveTopicUpdateRelational());
            client.subscribe(device.getTopicIntegration(i));
        }
        
        // action actuator + publish state
        if (actionActuator())
        {
            publishData();
        }
    } else {
        publishData();
    }
    
    delay(1000);
}

void setupDevice(int mode)
{
    device.setDeviceName(DEVICE_NAME);
    device.setCategory(DEVICE_CATEGORY);
    device.setDeviceType(DEVICE_TYPE);
    device.setAckTopic(DEVICE_ACKTOPIC);
    device.setLocation(DEVICE_LOCATION);
    
    if (mode == 0)
    {
        device.setMaximumIntegration(DEVICE_INTEGRATION_MAX);
        
        for (int i = 0; i < 2; i++)
        {
            device.appendCategoryIntegration(DEVICE_INTEGRATION_CATEGORY[i], i);
        }
    }
}

void setupService(int index)
{
    service[index].setServiceName(DEVICE_SERVICE_NAME[index]);
    service[index].setServiceUnit(DEVICE_SERVICE_UNIT[index]);
    service[index].setServiceData(DEVICE_SERVICE_DATA[index]);
}

void setupBroadcast(int mode)
{
    String payloadBC = "{\"deviceName\":" + device.getDeviceName() + ",";
    payloadBC += "\"category\":" + device.getCategory() + ",";
    payloadBC += "\"deviceType\":" + device.getDeviceType() + ",";
    payloadBC += "\"ackTopic\":" + device.getAckTopic() + ",";
    payloadBC += "\"location\":" + device.getLocation() + ",";
    
    // Root=Service
    payloadBC += "\"service\": {";
    for (int i = 0; i < DEVICE_SERVICE_TOTAL; i++)
    {
        payloadBC += "\"" + DEVICE_INTEGRATION_CATEGORY[i] + "\": {";
        payloadBC += "\"name\":" + service[i].getServiceName() + ",";
        payloadBC += "\"unit\":" + service[i].getServiceData() + ",";
        payloadBC += "\"data\":" + service[i].getServiceData() + "}";
        if (i + 1 < DEVICE_SERVICE_TOTAL)
        {
            payloadBC += ",";
        }
    }
    payloadBC += "}";
    
    if (mode == 0)
    {
        // Additional of Actuator Mode
        // Root=Integration
        payloadBC += ",";
        payloadBC += "\"integration\": {\"max\"" + String(device.getMaximumIntegration()) + ",";
        payloadBC += "\"category\":" + device.getCategoryIntegration(0) + device.getCategoryIntegration(1) + "}";
    }
    payloadBC += "}";
    
    client.publish(PROJECT_BROADCAST_TOPIC, payloadBC);
}

void connect()
{
    Serial.print("checking wifi...");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(1000);
    }
    
    Serial.print("\nconnecting...");
    while (!client.connect("arduino", MQTT_USERNAME, MQTT_PASSWORD))
    {
        Serial.print(".");
        delay(1000);
    }
    
    Serial.println("\nconnected!");
    
    // client.subscribe("/example");
    // client.unsubscribe("/example");
}

void messageReceived(String topic, String payload, char *bytes, unsigned int length)
{
    Serial.print("incoming: ");
    Serial.print(topic);
    Serial.print(" - ");
    Serial.print(payload);
    Serial.println();
    
    // Parsing data process
    int statuscode = 200;
    topic = "kardel/pervasive/kamar/sensor/LDR-001/data";
    String target_deviceName = "PIR-001";
    String target_topic = "kardel/pervasive/kamar/sensor/PIR-001/data";
    int data_sensor = 0;
    
    if (topic == device.getAckTopic())
    {
        DynamicJsonBuffer jsonBuffer;
        JsonObject &msgJson = jsonBuffer.parseObject(payload);
        // Check message response
        if (msgJson[String("statuscode")] == 200)
        {
            device.setDeviceConnected(true);
            device.setPervasiveTopic(msgJson[String("replytopic_data")]);
        }
        else
        {
            device.setDeviceConnected(false);
        }
    }
    
    if (device.deviceConnected() && (device.getDeviceType() == KEY_ACTUATOR))
        //save relational object of sensor, if device registered!
    {
        if (topic == device.getPervasiveTopicUpdateRelational())
        {
            updateTopicRelational(payload);
        }
        else
        {
            getDataFromTopicRelational(topic, payload);
        }
    }
}

void updateTopicRelational(String payload)
{
    //append topic from object relational
    DynamicJsonBuffer jsonBuffer;
    JsonObject &msgJson = jsonBuffer.parseObject(payload);
    
    if (msgJson[String("statuscode")] == 200)
    {
        for (int i = 0; i < device.getMaximumIntegration(); i++)
        {
            if (device.getTopicIntegration(i) == "")
            {
                device.appendTopicIntegration(msgJson[String("update_topic_sensor")], i);
                device.appendDeviceNameIntegration(msgJson[String("deviceName")], i);
                break;
            }
        }
    }
}

void getDataFromTopicRelational(String topic, String payload)
{
    // subscribe all triggered + insert data sensor
    DynamicJsonBuffer jsonBuffer;
    JsonObject &msgJson = jsonBuffer.parseObject(payload);
    String deviceService = getServicesFromDeviceName(msgJson[String("deviceName")]);
    
    for (int i = 0; i < device.getMaximumIntegration(); i++)
    {
        if (topic.substring(17) == device.getLocation() + "/" + device.getDeviceNameIntegration(i) + "/data")
        {
            if (msgJson[String("deviceName")] == device.getDeviceNameIntegration(i))
            {
                device.appendDeviceDataIntegration(msgJson[String("service")][String("data")], i);
                break;
            }
        }
    }
}

String getServicesFromDeviceName(String name)
{
    for (int i = 0; i < sizeof(KEY_LIST_OF_SERVICES[0]) / sizeof(KEY_LIST_OF_SERVICES[0][0]); i++)
    {
        for (int j = 0; j < sizeof(KEY_LIST_OF_SERVICES[1]) / sizeof(KEY_LIST_OF_SERVICES[1][0]); i++)
        {
            if (name.substring(0, 3) == KEY_LIST_OF_SERVICES[i][j])
            {
                return KEY_LIST_OF_SERVICES[i][0];
            }
        }
    }
}

bool actionActuator()
{
    int actionForRelay = 0;
    
    for (int i = 0; i < device.getMaximumIntegration(); i++)
    {
        if (i == 0)
        {
            if (device.getDeviceDataIntegration(i).toInt() < DEVICE_THRESHOLD_INTEGRATION_LDR)
            {
                actionForRelay = 0;
            }
            else
            {
                actionForRelay = 1;
            }
        }
        else if (i == 1)
        {
            if (actionForRelay == 0)
            {
                //case pagi
                if (device.getDeviceDataIntegration(1).toInt() == DEVICE_THRESHOLD_INTEGRATION_PIR)
                {
                    actionForRelay = 1;
                }
                else
                {
                    actionForRelay = 0;
                }
            }
            else
            {
                if (device.getDeviceDataIntegration(1).toInt() == DEVICE_THRESHOLD_INTEGRATION_PIR)
                {
                    actionForRelay = 0;
                }
                else
                {
                    actionForRelay = 1;
                }
            }
        }
    }
    
    service[0].setServiceData(String(actionForRelay));
    return true;
}

void publishData()
{
    String payloadPublish = "{\"deviceName\":" + device.getDeviceName() + ",";
    payloadPublish += "\"deviceType\":" + device.getDeviceType() + ",";
    
    // Root=Service
    payloadPublish += "\"service\": {";
    for (int i = 0; i < DEVICE_SERVICE_TOTAL; i++)
    {
        payloadPublish += "\"" + service[i].getServiceName() + "\": {";
        payloadPublish += "\"name\":" + service[i].getServiceName() + ",";
        payloadPublish += "\"unit\":" + service[i].getServiceData() + ",";
        payloadPublish += "\"data\":" + service[i].getServiceData() + "}";
        if (i + 1 < DEVICE_SERVICE_TOTAL)
        {
            payloadPublish += ",";
        }
    }
    payloadPublish += "}";
    
    payloadPublish += "}";
    
    client.publish(device.getPervasiveTopic(), payloadPublish);
}
