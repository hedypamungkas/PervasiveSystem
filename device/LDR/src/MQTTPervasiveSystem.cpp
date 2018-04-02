//
//  MQTTPervasiveSystem.cpp
//  PervasiveSystemProject
//
//  Created by Hedy Pamungkas on 9/8/17.
//  Copyright © 2017 Hedy Pamungkas. All rights reserved.
//

#include <Arduino.h>
#include "MQTTPervasiveSystem.h"

void PervasiveDevice::setDeviceName(String newDeviceNew)
{
    deviceName = newDeviceNew;
}

void PervasiveDevice::setCategory(String newCategory)
{
    category = newCategory;
}

void PervasiveDevice::setDeviceType(String newDeviceType)
{
    deviceType = newDeviceType;
}

void PervasiveDevice::setAckTopic(String newAckTopic)
{
    ackTopic = newAckTopic;
}

void PervasiveDevice::setLocation(String newLocation)
{
    location = newLocation;
}

void PervasiveDevice::setMaximumIntegration(int newMaximumIntegration)
{
    maximumIntegration = newMaximumIntegration;
}

void PervasiveDevice::appendCategoryIntegration(String newCategoryIntegration, int indexPath)
{
    categoryIntegration[indexPath] = newCategoryIntegration;
}

void PervasiveDevice::appendTopicIntegration(String newTopicIntegration, int indexPath)
{
    topicIntegration[indexPath] = newTopicIntegration;
}

void PervasiveDevice::appendDeviceNameIntegration(String newDeviceNameIntegration, int indexPath)
{
    deviceNameIntegration[indexPath] = newDeviceNameIntegration;
}

void PervasiveDevice::appendDeviceDataIntegration(String newDeviceDataIntegration, int indexPath)
{
    deviceDataIntegration[indexPath] = newDeviceDataIntegration;
}

void PervasiveDevice::removeIntegration(int indexPath)
{
    topicIntegration[indexPath] = "";
    deviceNameIntegration[indexPath] = "";
    deviceDataIntegration[indexPath] = "";
}

void PervasiveDevice::setDeviceConnected(bool newState)
{
    connected = newState;
}

void PervasiveDevice::setPervasiveTopic(String newTopicPervasive)
{
    pervasiveTopic = newTopicPervasive;
}

void PervasiveService::setServiceName(String newServiceName)
{
    serviceName = newServiceName;
}

void PervasiveService::setServiceUnit(String newServiceUnit)
{
    serviceUnit = newServiceUnit;
}

void PervasiveService::setServiceData(String newServiceData)
{
    serviceData = newServiceData;
}

String PervasiveDevice::getDeviceName() const
{
    return deviceName;
}

String PervasiveDevice::getCategory() const
{
    return category;
}

String PervasiveDevice::getDeviceType() const
{
    return deviceType;
}

String PervasiveDevice::getAckTopic() const
{
    return ackTopic;
}

String PervasiveDevice::getLocation() const
{
    return location;
}

int PervasiveDevice::getMaximumIntegration() const
{
    return maximumIntegration;
}

String PervasiveDevice::getCategoryIntegration(int indexPath)
{
    return categoryIntegration[indexPath];
}

String PervasiveDevice::getTopicIntegration(int indexPath)
{
    return topicIntegration[indexPath];
}

String PervasiveDevice::getDeviceNameIntegration(int indexPath)
{
    return deviceNameIntegration[indexPath];
}

String PervasiveDevice::getDeviceDataIntegration(int indexPath)
{
    return deviceDataIntegration[indexPath];
}

bool PervasiveDevice::deviceConnected()
{
    return connected;
}

String PervasiveDevice::getPervasiveTopic() const
{
    return pervasiveTopic;
}

String PervasiveDevice::getPervasiveTopicUpdateRelational() const
{
    return pervasiveTopic + "/update";
}

String PervasiveDevice::getPervasiveTopicWildcard() const
{
    String newTopic = ackTopic.substring(0,6);
    return newTopic + "/pervasive/" + location + "/#";
}

String PervasiveService::getServiceName() const
{
    return serviceName;
}

String PervasiveService::getServiceUnit() const
{
    return serviceUnit;
}

String PervasiveService::getServiceData() const
{
    return serviceData;
}