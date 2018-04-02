// //
// //  metadata.h
// //  PervasiveSystemProject
// //
// //  Created by Hedy Pamungkas on 9/8/17.
// //  Copyright © 2017 Hedy Pamungkas. All rights reserved.
// //

// #ifndef metadata_h
// #define metadata_h

// #include <Arduino.h>

// //device
// String PROJECT_NAME = "kardel";
// String PROJECT_ACKTOPIC = PROJECT_NAME + "/ack/";
// String PROJECT_BROADCAST_TOPIC = PROJECT_NAME + "/broadcast";
// String DEVICE_NAME = "PIR-001";
// String DEVICE_CATEGORY = "PIR";
// String DEVICE_TYPE = "sensor";
// String DEVICE_ACKTOPIC = PROJECT_ACKTOPIC + DEVICE_TYPE + "/" + DEVICE_NAME;
// String DEVICE_LOCATION = "kamar";

// //additional for actuator
// int DEVICE_INTEGRATION_MAX = 0;
// String DEVICE_INTEGRATION_CATEGORY[] = {"LDR", "PIR"};
// int DEVICE_THRESHOLD_INTEGRATION_LDR = 0;
// int DEVICE_THRESHOLD_INTEGRATION_PIR = 1;

// //service
// int DEVICE_SERVICE_TOTAL = 1;
// String DEVICE_SERVICE_NAME[] = {"motion"};
// String DEVICE_SERVICE_UNIT[] = {" "};
// String DEVICE_SERVICE_DATA[] = {"0"};

// //other
// String KEY_ACTUATOR = "actuator";
// String KEY_SENSOR = "sensor";
// String KEY_LIST_OF_SERVICES[5][5] = {{"light", "LDR"}, {"motion", "PIR"}, {"vibration", "BUZ"}};
// const int DEVICE_MODE = 1;

// #endif /* metadata_h */

//
//  metadata.h
//  PervasiveSystemProject
//
//  Created by Hedy Pamungkas on 9/8/17.
//  Copyright © 2017 Hedy Pamungkas. All rights reserved.
//

#ifndef metadata_h
#define metadata_h

#include <Arduino.h>

//device
String PROJECT_NAME = "kardel";
String PROJECT_ACKTOPIC = PROJECT_NAME + "/ack/";
String PROJECT_BROADCAST_TOPIC = PROJECT_NAME + "/broadcast";
String DEVICE_NAME = "LDR-001";
String DEVICE_CATEGORY = "LDR";
String DEVICE_TYPE = "sensor";
String DEVICE_ACKTOPIC = PROJECT_ACKTOPIC + DEVICE_TYPE + "/" + DEVICE_NAME;
String DEVICE_LOCATION = "kamar";

//additional for actuator
int DEVICE_INTEGRATION_MAX = 0;
String DEVICE_INTEGRATION_CATEGORY[] = {"LDR", "PIR"};
int DEVICE_THRESHOLD_INTEGRATION_LDR = 0;
int DEVICE_THRESHOLD_INTEGRATION_PIR = 1;

//service
int DEVICE_SERVICE_TOTAL = 1;
String DEVICE_SERVICE_NAME[] = {"light"};
String DEVICE_SERVICE_UNIT[] = {" "};
String DEVICE_SERVICE_DATA[] = {"0"};

//other
String KEY_ACTUATOR = "actuator";
String KEY_SENSOR = "sensor";
String KEY_LIST_OF_SERVICES[5][5] = {{"light", "LDR"}, {"motion", "PIR"}, {"vibration", "BUZ"}};
const int DEVICE_MODE = 1;

#endif /* metadata_h */