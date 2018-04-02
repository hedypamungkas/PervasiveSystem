//
//  MQTTPervasiveSystem.h
//  PervasiveSystemProject
//
//  Created by Hedy Pamungkas on 9/8/17.
//  Copyright © 2017 Hedy Pamungkas. All rights reserved.
//

#ifndef MQTTPervasiveSystem_h
#define MQTTPervasiveSystem_h

#include "Arduino.h"

class PervasiveDevice
{
public:
    void setDeviceName(String);
    void setCategory(String);
    void setDeviceType(String);
    void setAckTopic(String);
    void setLocation(String);
    void setMaximumIntegration(int);
    void appendCategoryIntegration(String, int);
    void appendTopicIntegration(String, int);
    void appendDeviceNameIntegration(String, int);
    void appendDeviceDataIntegration(String, int);
    void setDeviceConnected(bool);
    void setPervasiveTopic(String);
    String getDeviceName() const;
    String getCategory() const;
    String getDeviceType() const;
    String getAckTopic() const;
    String getLocation() const;
    int getMaximumIntegration() const;
    String getCategoryIntegration(int);
    String getTopicIntegration(int);
    String getDeviceNameIntegration(int);
    String getDeviceDataIntegration(int);
    bool deviceConnected();
    String getPervasiveTopic() const;
    String getPervasiveTopicUpdateRelational() const;
    
private:
    String deviceName;
    String category;
    String deviceType;
    String ackTopic;
    String location;
    int maximumIntegration;
    String categoryIntegration[5];
    String topicIntegration[5] = {"", "", "", "", ""};
    String deviceNameIntegration[5] = {"", "", "", "", ""};
    String deviceDataIntegration[5];
    bool connected = false;
    String pervasiveTopic;
};

class PervasiveService
{
public:
    void setServiceName(String);
    void setServiceUnit(String);
    void setServiceData(String);
    String getServiceName() const;
    String getServiceUnit() const;
    String getServiceData() const;
    
private:
    String serviceName;
    String serviceUnit;
    String serviceData;
};

#endif /* MQTTPervasiveSystem_h */
