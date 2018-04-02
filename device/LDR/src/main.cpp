//
//  main.cpp
//  PervasiveSystemProject
//
//  Created by Hedy Pamungkas on 9/8/17.
//  Copyright © 2017 Hedy Pamungkas. All rights reserved.
//

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include "main.h"
#include "metadata.h"
#include "MQTTPervasiveSystem.h"

PervasiveDevice device;
PervasiveService service[2];

WiFiClient net;
MQTTClient client(512);

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(9600);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    client.begin(MQTT_SERVER_NAME, net);
    client.onMessage(messageReceived);

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
    validateDataModel();

    pinMode(pin, INPUT);

    while (!device.deviceConnected())
    {
        // SETUP BROADCAST
        client.publish("kardel/test", "ready!");
        delay(2000); //fix
        setupBroadcast(DEVICE_MODE);
        // delay(5000);
        client.subscribe(String(device.getAckTopic()));
    }
    digitalWrite(pin, transformDataRelay(0));
}

void validateDataModel()
{
    Serial.println("Model of Device Object");

    Serial.println("device Name\t\t: " + device.getDeviceName());
    Serial.println("device Category\t\t: " + device.getCategory());
    Serial.println("device Type\t\t: " + device.getDeviceType());
    Serial.println("device Ack Topic\t: " + device.getAckTopic());
    Serial.println("device Location\t\t: " + device.getLocation());

    Serial.println("Model of Service Object");

    Serial.println("service Name\t:" + service[0].getServiceName());
    Serial.println("service Unit\t:" + service[0].getServiceUnit());
    Serial.println("service Data\t:" + service[0].getServiceData());
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
    
    publishData();

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
    String payloadBC = "{\"deviceName\": \"" + device.getDeviceName() + "\"" + ",";
    payloadBC += "\"category\": \"" + device.getCategory() + "\"" + ",";
    payloadBC += "\"deviceType\": \"" + device.getDeviceType() + "\"" + ",";
    payloadBC += "\"ackTopic\": \"" + device.getAckTopic() + "\"" + ",";
    payloadBC += "\"location\": \"" + device.getLocation() + "\"" + ",";

    // Root=Service
    payloadBC += "\"service\": {";
    for (int i = 0; i < DEVICE_SERVICE_TOTAL; i++)
    {
        payloadBC += "\"" + DEVICE_SERVICE_NAME[i] + "\": {";
        payloadBC += "\"name\": \"" + service[i].getServiceName() + "\"" + ",";
        payloadBC += "\"unit\": \"" + service[i].getServiceData() + "\"" + ",";
        payloadBC += "\"data\": \"" + service[i].getServiceData() + "\"" + "}";
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
        payloadBC += "\"integration\": {\"max\":" + String(device.getMaximumIntegration()) + ",";
        payloadBC += "\"category\": [\"" + device.getCategoryIntegration(0) + "\"" + ", \"" + device.getCategoryIntegration(1) + "\"" + "]}";
    }
    payloadBC += "}";

    // String test = "{\"deviceName\":\"BULB-001\",\"category\":\"BULB\",\"deviceType\":\"actuator\",\"ackTopic\":\"kardel/ack/actuator/BULB-001\",\"location\":\"kamar\",\"service\":{\"lamp\":{\"name\":\"lamp\",\"unit\":\"\",\"data\":\"True\"},\"level\":{\"name\":\"level\",\"unit\":\" %\",\"data\":0}},\"integration\":{\"max\":2,\"category\":[\"LDR\",\"PIR\"]}}";
    Serial.println(payloadBC);
    client.publish("kardel/broadcast", payloadBC);
    Serial.println("msg sent");
    delay(2000);
}

void connect()
{
    // Serial.print("checking wifi...");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(1000);
    }

    Serial.print("connecting...");
    while (!client.connect("arduino", MQTT_USERNAME, MQTT_PASSWORD))
    {
        Serial.print(".");
        delay(1000);
    }

    Serial.print(" ok!\n");
}

void messageReceived(String &topic, String &payload)
{
    Serial.println("\nincoming: " + topic + " - " + payload);

    if (topic == device.getAckTopic())
    {

        DynamicJsonBuffer jsonBuffer;
        JsonObject &msgJson = jsonBuffer.parseObject(payload);
        // Check message response
        if (msgJson["statuscode"] == 200)
        {
            Serial.println("Registration Done");
            device.setDeviceConnected(true);
            device.setPervasiveTopic(msgJson[String("replytopic_data")]);
        }
        else
        {
            Serial.println("Failed");
            device.setDeviceConnected(false);
        }
        delay(5000);
    }

    delay(3000);
}

int transformDataRelay(int data)
{
    if (data == 1)
    {
        return 0;
    }
    return 1;
}

void publishData()
{
    if (device.getDeviceType() == KEY_SENSOR)
    {
        String dataSensor = getDataFromSensor();
        service[0].setServiceData(dataSensor);
    }

    String payloadPublish = "{\"deviceName\": \"" + device.getDeviceName() + "\"" + ",";
    payloadPublish += "\"deviceType\": \"" + device.getDeviceType() + "\"" + ",";

    // Root=Service
    payloadPublish += "\"service\": {";
    for (int i = 0; i < DEVICE_SERVICE_TOTAL; i++)
    {
        payloadPublish += "\"" + service[i].getServiceName() + "\": {";
        payloadPublish += "\"name\": \"" + service[i].getServiceName() + "\"" + ",";
        payloadPublish += "\"unit\": \"" + service[i].getServiceUnit() + "\"" + ",";
        payloadPublish += "\"data\": \"" + service[i].getServiceData() + "\"" + "}";
        if (i + 1 < DEVICE_SERVICE_TOTAL)
        {
            payloadPublish += ",";
        }
    }
    payloadPublish += "}";

    payloadPublish += "}";

    Serial.println(payloadPublish);
    client.publish(device.getPervasiveTopic(), payloadPublish);
}

String getDataFromSensor()
{
    int data = digitalRead(pin);
    return String(data);
}