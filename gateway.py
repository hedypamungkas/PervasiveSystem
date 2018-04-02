import paho.mqtt.client as paho
import datetime, json, time, sys, ast, pprint, os

class MQTTPervasiveSystem(object):

    def __init__(self, mqtt_host, project_name, mqtt_user_name, mqtt_password, mqtt_port=1883,
                    mqtt_keepalive=60, mqtt_qos=0):
        self.mqtt_host = mqtt_host
        self.mqtt_port = mqtt_port
        self.mqtt_user_name = mqtt_user_name
        self.mqtt_password = mqtt_password
        self.mqtt_keepalive = mqtt_keepalive
        self.mqtt_qos = mqtt_qos
        self.project_name = project_name
        self.mqtt_topic_name = project_name + "/#"
        self.topic_broadcast = project_name + "/broadcast"
        self.topic_pervasive = project_name + "/pervasive/"
        self.targetLocation = ""
        self.client = paho.Client()
        self.counterOfConnect = 0
        self.client.on_message = self.on_message
        self.client.on_connect = self.on_connect
        # self.client.on_log = self.on_log
        self.device = {
            "sensor": {

            },
            "actuator": {

            },
            "locations": {

            }
        }

    def checkingDevice(self, msg):
        data = json.loads(msg)
        flag = False

        #Checking Device Name
        obj = self.device[str(data["deviceType"])].keys()
        try:
            obj.index(data["deviceName"])
        except ValueError:
            flag = True
        else:
            flag = False
            return flag

        if (flag==True):
            #Checking Actuator First
            if(str(data["deviceType"])=="sensor"):
               return bool(self.device["actuator"])
            else:
                return True

    def checkingRelation(self, msg):
        data = json.loads(msg)
        resultName = ""
        statusRelationalFlag = False
        currentItemInLocation = 0

        # 1. searching actuator data in location & msg=sensor
        try:
            targetLocationActuator = self.device["locations"][str(data["location"])]
            for listobj in targetLocationActuator:
                for item in listobj.items():
                    if (item[0]=="deviceName"):
                        currentItemInLocation = len(targetLocationActuator)-1
                        resultName = item[1]
                        break
                break
        except KeyError:
            statusRelationalFlag = False
            print('Actuator not available in this area!')
        else:
            statusRelationalFlag = True


        # 2. get data max and category compatibility in actuator data
        if (statusRelationalFlag):
            try:
                print('masuk')
                targetIntegrationActuator = self.device["actuator"][str(resultName)]["integration"]
                categoryCompability = targetIntegrationActuator["category"]
                maximumIntegration = int(targetIntegrationActuator["max"])
            except KeyError:
                statusRelationalFlag = False
            else:
                statusRelationalFlag = True

            # 3. check condition location
            if (currentItemInLocation<maximumIntegration) and (statusRelationalFlag):
                try:
                    categoryCompability.index(str(data["category"]))
                except ValueError:
                    print('Incompability')
                else:
                    print('Compability')
                    self.relationCompatibility(msg, targetLocationActuator[0]["topic_pervasive"])
            else:
                print('Oops, already max!')

        return statusRelationalFlag

    def relationCompatibility(self, msg, target_topic=''):
        data = json.loads(msg)
        replytopic = self.topic_pervasive + data["location"] + "/" + data["deviceType"] + "/" + data["deviceName"]
        print('relationCompatibility')

        try:
            self.device["locations"][str(data["location"])].append(
                {
                    "deviceName": str(data["deviceName"]),
                    "deviceType": str(data["deviceType"]),
                    "topic_pervasive": str(replytopic) + "/data"
                }
            )
        except KeyError:
            self.device["locations"][str(data["location"])] = [
                {
                    "deviceName": str(data["deviceName"]),
                    "deviceType": str(data["deviceType"]),
                    "topic_pervasive": str(replytopic) + "/data"
                }
            ]

        if (bool(target_topic)):
            update_relational = {
                "statuscode": 200,
                "deviceName": str(data["deviceName"]),
                "update_topic_sensor": str(replytopic) + "/data"
            }
            print(target_topic)
            self.client.publish(target_topic + "/update", str(update_relational), self.mqtt_qos)

    def deviceConnect(self, msg):
        data = json.loads(msg)
        replytopic = self.topic_pervasive + data["location"] + "/" + data["deviceType"] + "/" + data["deviceName"]
        isCompatibility = False

        if (self.counterOfConnect != 0):

            if (self.checkingDevice(msg)):

                if (str(data["deviceType"])=="sensor"):
                    print('masuk sensor')
                    isCompatibility = self.checkingRelation(msg)
                else:
                    self.relationCompatibility(msg)
                    self.device[str(data["deviceType"])][str(data["deviceName"])] = ast.literal_eval(msg)

                device_connect_msg = {
                    "statuscode": 200,
                    "replytopic_data": str(replytopic) + "/data"
                }

                if (isCompatibility):
                    self.device[str(data["deviceType"])][str(data["deviceName"])] = ast.literal_eval(msg)

                os.system('clear')
                pprint.pprint(json.dumps(self.device), indent=1)
            else:
                device_connect_msg = {
                    "statuscode": 404
                }
                print('blocked!')

            self.client.publish(data["ackTopic"], str(device_connect_msg), self.mqtt_qos)
            self.counterOfConnect = 0

        else:
            self.counterOfConnect = 1

    def removeIntegration(self, topic, msg):
        # print topic, msg
        data = json.loads(msg)
        targetIndex = 0
        counter = 0
        self.targetLocation = str(data["location"])

        try:
            #remove from sensor object
            self.device["sensor"].pop(str(data["deviceName"]))

            #first, search index the target deviceName in object location
            for item in self.device["locations"][self.targetLocation]:
                if (item["deviceName"] == data["deviceName"]):
                    targetIndex = counter
                    break

                counter = counter + 1

            #remove deviceName in object location
            self.device["locations"][self.targetLocation].pop(targetIndex)
            print "success!"
            os.system('clear')
            pprint.pprint(json.dumps(self.device), indent=1)
        except KeyError:
            print "error: KeyError - " + str(data["location"]) + ", " + str(data["deviceName"])

    def pervasivePersistance(self, topic, msg):
        if (str(topic).find("remove")==-1):
            data = json.loads(msg)
            print("masuk update fix")

            #Checking Device Name
            obj = self.device[str(data["deviceType"])].keys()
            try:
                obj.index(str(data["deviceName"]))
            except ValueError:
                print('device has not been registered')
            else:
                self.device[str(data["deviceType"])][str(data["deviceName"])]["service"] = data["service"]
                os.system('clear')
                pprint.pprint(json.dumps(self.device), indent=1)

    def connect(self):
        print("Starting MQTT Stream")
        self.client.connect_async(
            host=self.mqtt_host,
            port=self.mqtt_port,
            keepalive=self.mqtt_keepalive
        )

    def username_pw_set(self):
        self.client.username_pw_set(
            self.mqtt_user_name,
            password=self.mqtt_password
        )

    def on_message(self, mqttc, userdata, msg):
        # print('on_message topic: {0}\tmsg.payload: {1}\ttime: {2}'.format(
        #     msg.topic,
        #     msg.payload,
        #     datetime.datetime.now())
        # )
        # print(msg.payload)
        # insert data_sensor
        if (msg.topic==self.topic_broadcast):
            self.deviceConnect(msg.payload)
        elif (str(msg.topic).find(self.topic_pervasive)!=-1):
            if (str(msg.topic).find("remove")!=-1):
                #remove integration
                print("masuk remove")
                self.removeIntegration(msg.topic, msg.payload)
            else:
                if (str(msg.topic).find("update")==-1):
                    print("masuk update")
                    self.pervasivePersistance(msg.topic, msg.payload)



    def on_log(self, client, userdata, level, buf):
        print("log: ",buf)
        self.client.on_log=self.on_log #client logging

    def on_connect(self, mqttc, userdata, flags, msg):
        rc = mqttc.subscribe(self.mqtt_topic_name, 0)
        # print('Subscribe topic: {0} RC: {1} time: {2}'.format(self.mqtt_topic_name, rc, datetime.datetime.now()))

if __name__ == '__main__':

    try:
        gateway = MQTTPervasiveSystem(
            mqtt_host="ec2-13-229-110-14.ap-southeast-1.compute.amazonaws.com",
            project_name="kardel",
            mqtt_user_name="admintes",
            mqtt_password="admin123"
        )
        gateway.connect()
        gateway.username_pw_set()
        gateway.client.loop_forever()
    except KeyboardInterrupt:
        print('\nSYSTEM HAS BEEN FORCED CLOSE\n')
        sys.exit()
