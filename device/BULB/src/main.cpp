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

    if (device.getDeviceType() == KEY_ACTUATOR)
    {
        pinMode(pin, OUTPUT);
    }
    else
    {
        pinMode(pin, INPUT);
        // pinMode(pin, INPUT);
    }

    while (!device.deviceConnected())
    {
        // SETUP BROADCAST
        delay(2000); //fix
        setupBroadcast(DEVICE_MODE);
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

    Serial.println("service Name\t:" + service[1].getServiceName());
    Serial.println("service Unit\t:" + service[1].getServiceUnit());
    Serial.println("service Data\t:" + service[1].getServiceData());
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

        client.subscribe(device.getPervasiveTopicWildcard());
        delay(2000);

        if (integrateReady)
        {
            keepAliveState();

            if (dataReady && actionActuator())
            {
                // action actuator + publish state
                Serial.println("action" + service[0].getServiceData());
                digitalWrite(pin, transformDataRelay(service[0].getServiceData().toInt()));
            }
        }

    }
    else
    {
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

    String test = "{\"deviceName\":\"BULB-001\",\"category\":\"BULB\",\"deviceType\":\"actuator\",\"ackTopic\":\"kardel/ack/actuator/BULB-001\",\"location\":\"kamar\",\"service\":{\"lamp\":{\"name\":\"lamp\",\"unit\":\"\",\"data\":\"True\"},\"level\":{\"name\":\"level\",\"unit\":\" %\",\"data\":0}},\"integration\":{\"max\":2,\"category\":[\"LDR\",\"PIR\"]}}";
    client.publish(PROJECT_BROADCAST_TOPIC, payloadBC);
    Serial.println(payloadBC);
    Serial.println(PROJECT_BROADCAST_TOPIC);
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

void keepAliveState()
{
    
    for (int i = 0; i < device.getMaximumIntegration(); i++)
    {
        currentMillisKeepAlive = millis();

        if (previousMillisKeepAlive[i] == 0)
        {
            previousMillisKeepAlive[i] = currentMillisKeepAlive;
        }

        // Serial.println(currentMillisKeepAlive + ", " + previousMillisKeepAlive[i]);

        if (currentMillisKeepAlive - previousMillisKeepAlive[i] >= intervalTimeout)
        {
            previousMillisKeepAlive[i] = currentMillisKeepAlive;
            String deviceName = device.getDeviceNameIntegration(i);
            // Serial.println(String(i) + ", " + device.getDeviceNameIntegration(0) + "-" + device.getDeviceNameIntegration(1));
            
            int whileCounter = 0;
            int indexPath = 0;
            while(deviceName == "")
            {
                if (whileCounter == 0)
                {
                    indexPath = i-1;
                    deviceName = device.getDeviceNameIntegration(indexPath);
                }
                else if (whileCounter < 2)
                {
                    indexPath = i+1;
                    deviceName = device.getDeviceNameIntegration(indexPath);
                }
                else 
                {
                    Serial.println("null variable");
                }
                
                whileCounter++;
            }

            Serial.println("object deleted - " + deviceName + " - " + previousMillisKeepAlive[i]);

            if (deviceName == "")
            {
                device.removeIntegration(indexPath);
            }
            else
            {
                device.removeIntegration(i);
            }
            
            updateIntegrationtoGateway(deviceName);
            faultCounter++;
            Serial.println("faultCounter: " + String(faultCounter) + ", integrate: " + String(deviceTotalIntegrate));
 
            if (faultCounter == deviceTotalIntegrate)
            {
                previousMillisKeepAlive[i] = 0;
                currentMillisKeepAlive = 0;
                integrateReady = false;
                dataReady = false;
                faultCounter = 0;
                deviceTotalIntegrate = 0;
                Serial.println("all device integration has disconnected");
            }

            break;
        }
    }
}

void updateIntegrationtoGateway(String deviceName)
{
    String payloadBC = "{\"deviceName\": \"" + deviceName + "\"" + ",";
    payloadBC += "\"location\": \"" + device.getLocation() + "\"";
    payloadBC += "}";

    client.publish(device.getPervasiveTopic() + "/remove", payloadBC);
    Serial.println("success");
}

void messageReceived(String &topic, String &payload)
{
    // Serial.println("\nincoming: " + topic + " - " + payload);

    if (device.deviceConnected() && (device.getDeviceType() == KEY_ACTUATOR))
    //save relational object of sensor, if device registered!
    {
        if (topic == device.getPervasiveTopicUpdateRelational())
        {
            // Serial.println("received topic in update relational");
            updateTopicRelational(payload);
            delay(2000);
        }
        else if (topic != (device.getPervasiveTopic() + "/remove"))
        {
            // Serial.println("received payload in topic relational");
            getDataFromTopicRelational(topic, payload);
        }
    }

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
    }

    delay(3000);
}

void updateTopicRelational(String payload)
{
    //append topic from object relational
    DynamicJsonBuffer jsonBuffer;
    JsonObject &msgJson = jsonBuffer.parseObject(payload);

    if (msgJson["statuscode"] == 200)
    {
        for (int i = 0; i < device.getMaximumIntegration(); i++)
        {
            if (device.getTopicIntegration(i) == "")
            {
                previousMillisKeepAlive[i] = millis();
                Serial.println("slot-" + String(i) + " available");
                device.appendTopicIntegration(msgJson["update_topic_sensor"], i);
                device.appendDeviceNameIntegration(msgJson["deviceName"], i);
                Serial.println(device.getTopicIntegration(i) + " - " + device.getDeviceNameIntegration(i) + " - " + previousMillisKeepAlive[i]);
                deviceTotalIntegrate++;
                Serial.println("integrate: "+String(deviceTotalIntegrate));
                integrateReady = true; 
                break;
            }
        }
    }
    else
    {
        Serial.println("Failed");
    }
}

void getDataFromTopicRelational(String topic, String payload)
{
    // subscribe all triggered + insert data sensor
    DynamicJsonBuffer jsonBuffer;
    JsonObject &msgJson = jsonBuffer.parseObject(payload);
    String deviceNameFromRelational = msgJson["deviceName"];
    String deviceService = getServicesFromDeviceName(deviceNameFromRelational.substring(0, 3));
    String data = msgJson["service"][deviceService]["data"];

    for (int i = 0; i < device.getMaximumIntegration(); i++)
    {
        if (deviceNameFromRelational == device.getDeviceNameIntegration(i))
        {
            device.appendDeviceDataIntegration(data, i);
            previousMillisKeepAlive[i] = millis();
            dataReady = true;
            break;
        }
    }
    dataCounter++;
    checkDeviceDataIntegration();
    publishData();
}

void checkDeviceDataIntegration()
{
    for (int i = 0; i < device.getMaximumIntegration(); i++)
    {
        Serial.println(device.getDeviceNameIntegration(i) + " => " + device.getDeviceDataIntegration(i) + " - " + previousMillisKeepAlive[i]);
    }
}

String getServicesFromDeviceName(String name)
{
    String result = "";
    if (name == "LDR")
    {
        return "light";
    }
    else if (name == "PIR")
    {
        return "motion";
    }
    else if (name == "BUZ")
    {
        return "vibration";
    }
    else
    {
        return "ERROR";
    }
}

bool actionActuator()
{
    int actionForRelay = 0;
    bool swipeCondition = false;
    int deviceTotal = calculateCurrentDeviceIntegrate();

    for (int i = 0; i < device.getMaximumIntegration(); i++)
    {
        if (i == 0)
        {
            if (!isLDR(i))
            {
                swipeCondition = true;
                actionForRelay = actionActuatorWithSwipeCondition(actionForRelay);
                break;
            }

            if (device.getDeviceDataIntegration(i).toInt() == DEVICE_THRESHOLD_INTEGRATION_LDR)
            {
                actionForRelay = 0;
            }
            else
            {
                actionForRelay = 1;
            }

            if (deviceTotal > 1)
            {
                actionForRelay = actionActuatorWithoutSwipeCondition(actionForRelay, 1);
            }
            else
            {
                break;
            }
        }
    }
    service[0].setServiceData(String(actionForRelay));
    return true;
}

int calculateCurrentDeviceIntegrate()
{
    return deviceTotalIntegrate;
}

bool isLDR(int i)
{
    if (device.getDeviceNameIntegration(i).substring(0, 3) == "LDR")
    {
        return true;
    }
    return false;
}

int actionActuatorWithSwipeCondition(int actionForRelay)
{
    int actionForRelayIntegrateLDR = 0;
    int deviceTotal = calculateCurrentDeviceIntegrate();

    for (int i = 0; i < device.getMaximumIntegration(); i++)
    {
        if (i == 0)
        {
            if (device.getDeviceDataIntegration(i).toInt() == DEVICE_THRESHOLD_INTEGRATION_PIR)
            {
                actionForRelay = 1;
            }
            else
            {
                actionForRelay = 0;
            }

            if (deviceTotal == 1)
            {
                break;
            }
        }
        else if (i == 1)
        {
            if (device.getDeviceDataIntegration(i).toInt() == DEVICE_THRESHOLD_INTEGRATION_LDR)
            {
                actionForRelayIntegrateLDR = 0;
            }
            else
            {
                actionForRelayIntegrateLDR = 1;
            }
        }
    }

    if (deviceTotal > 1)
    {
        actionForRelay = actionActuatorWithoutSwipeCondition(actionForRelayIntegrateLDR, 0);
    }

    return actionForRelay;
}

int actionActuatorWithoutSwipeCondition(int actionForRelay, int i)
{
    if (actionForRelay == 0)
    {
        if (device.getDeviceDataIntegration(i).toInt() == DEVICE_THRESHOLD_INTEGRATION_PIR)
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
        if (device.getDeviceDataIntegration(i).toInt() == DEVICE_THRESHOLD_INTEGRATION_PIR)
        {
            actionForRelay = 0;
        }
        else
        {
            actionForRelay = 1;
        }
    }
    return actionForRelay;
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
    if (device.getDeviceType() == KEY_SENSOR) {
        String dataSensor = getDataFromSensor();
        service[0].setServiceData(dataSensor);
    }

    Serial.println("prepare to sending data to gateway!");

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