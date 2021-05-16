#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoMqttClient.h>
#include <FS.h>
#include <ArduinoJson.h>

/* Config file */
#define LEDHOST_CONFIG "/ledhost_config.json"
#define MQTT_CONNECTION_TIMEOUT = 30000

StaticJsonDocument<1024> configFile;

/* WiFi Credentials */
const char* ssid;
const char* password;
const char* broker;
const char* chartopic;
String topic;
int port;
bool isMQTTConnected;

long consumeInterval = 1000;
int msgCount = 0;
unsigned long prevTime, currTime;

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

  Serial.println(topic);
  Serial.println("\r\n");

  /* Connect to MQTT host */
  if (testMQTTConnect()) {
    testMQTTSubscribe();
  }

  prevTime = millis();
}

/* Loop */
void loop() {    
    currTime = millis();
    testMQTTConsume();

    if (currTime - prevTime > consumeInterval) {
        prevTime = currTime;
        //testMQTTPublish();
        currTime = millis();
    }
}

void testWifiSetup() {
    Serial.print("Configuring access point...");
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected \n\r\IP address: ");
    Serial.println(WiFi.localIP());    
}

bool testMQTTConnect() {

    Serial.println("Connecting to MQTT server:");
    Serial.print("broker: ");
    Serial.print(broker);
    Serial.print(" port: ");
    Serial.println(port);
    
    if (!mqttClient.connect(broker, port)) {
      Serial.print("MQTT connection failed! Error code = ");
      Serial.println(mqttClient.connectError());    
      return false;        
    }
    else 
    {
      Serial.println("Connected!...");
      return true;
    }
   
    Serial.println("Not connected!...");
    return false;
}

void testMQTTSubscribe() {    
  Serial.print("Subscribing to topic: ");
  Serial.println(topic);

  mqttClient.subscribe(topic);  
}

void testMQTTPublish() {
   
    mqttClient.beginMessage(topic);
    mqttClient.print("Test message from NodeMCU: ");
    mqttClient.println(msgCount);
    mqttClient.endMessage();

    msgCount++;
}

void testMQTTConsume() {
  int messageSize = mqttClient.parseMessage();
  
  if (messageSize) {
    // we received a message, print out the topic and contents
    Serial.print("Consuming message: '");
    Serial.print(mqttClient.messageTopic());
    Serial.print("', length ");
    Serial.print(messageSize);
    Serial.println(" bytes:");

    // use the Stream interface to print the contents
    while (mqttClient.available()) {
      Serial.print((char)mqttClient.read());
    }
    Serial.println();
  }
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
            chartopic = configFile["mqtt_subtopic"];

            topic = String(chartopic);           
            Serial.print("SSID: ");
            Serial.println(ssid);
            Serial.print("Password: ");    
            Serial.println(password);      
            Serial.print("Broker: ");
            Serial.println(broker);
            Serial.print("Port: ");    
            Serial.println(port);       
            Serial.print("Topic: ");    
            Serial.println(topic);                     
        }    
    }
}
