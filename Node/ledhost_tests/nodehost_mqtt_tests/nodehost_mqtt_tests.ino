#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoMqttClient.h>
#include <FS.h>
#include <ArduinoJson.h>

/* Config file */
#define LEDHOST_CONFIG "/ledhost_config.json"

StaticJsonDocument<1024> configFile;

/* WiFi Credentials */
const char* ssid;
const char* password;
const char* broker;
const char* topic;
int port;

/* MQTT */
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);


/* Setup  */
void setup() {
  
  /* Set pins, clear UART */  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(D4, OUTPUT);
  delay(2000);
  Serial.begin(115200);
  delay(500);
  Serial.flush();
  while ( Serial.available() ) Serial.read(); 

  /* Verify config file */
  testLoadConfig();

  /* Verify wi-fi connectivity */  
  testWifiSetup();

}

/* Loop */
void loop() {    

}

void testWifiSetup() {
    Serial.print("Configuring access point...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected \n\r\ IP address: ");
    Serial.println(WiFi.localIP());    
}

bool testMQTTConnect() {
    if (!mqttClient.connect(broker, port)) {
      Serial.print("MQTT connection failed! Error code = ");
      Serial.println(mqttClient.connectError());    
      return false;
    }  

    return true;
}

void testMQTTSubscribe() {
    
  Serial.print("Subscribing to topic: ");
  Serial.println(topic);

  mqttClient.subscribe(topic);  
}

void testMQTTSubscribeBadTopic() {
  
  Serial.print("Subscribing to invalid topic: ");

  mqttClient.subscribe("not_valid_topic");
  
}

void testMQTTPublish() {

  
}

void testMQTTConsume() {
  
}


void testLoadConfig() {
  
    if (SPIFFS.begin()) {
        Serial.println("Reading from file system..");
    
        File f = SPIFFS.open(LEDHOST_CONFIG, "r");
    
        if (f && f.size()) {
            Serial.println("Opening config..");
            size_t filesize = f.size();
            
            char jsonData[filesize+1];
            f.read((uint8_t *)jsonData, sizeof(jsonData));
            f.close();
            jsonData[filesize]='\0';
            
            Serial.println("Output of config file: ");
            Serial.println(jsonData);
            
            DeserializationError error = deserializeJson(configFile, jsonData);

            if (error) {
                Serial.println("Failure to parse json data");
                return;
            }

            ssid = configFile["wifi_ssid"];
            password = configFile["wifi_pwd"];
            port = configFile["mqtt_port"];
            broker = configFile["mqtt_broker"];
            topic = configFile["mqtt_sub_topic"];
                       
            Serial.print("SSID: ");
            Serial.println(ssid);
            Serial.print("Password: ");    
            Serial.println(password);            
        }    
    }
}
