#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoWebsockets.h>
#include <FS.h>
#include <ArduinoJson.h>

/* Config file */
#define LEDHOST_CONFIG "/ledhost_config.json"

StaticJsonDocument<1024> configFile;

/* WiFi Credentials */
const char* ssid;
const char* password;
const char* ws_host;
String topic;
int ws_port;

long consumeInterval = 1000;
int msgCount = 0;
unsigned long prevTime, currTime;

/* WebSockets */
using namespace websockets;
WiFiClient wifiClient;
WebsocketsClient wsClient;

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

  /* Websockets callback */
  if (testWSConnect()) {
    wsClient.onMessage([&](WebsocketsMessage message){
      Serial.println("Message received...");
      Serial.println(message.data());
      testWSConsume(message);
    });
  }

  prevTime = millis();
}

/* Loop */
void loop() {    
    currTime = millis();

    if (currTime - prevTime > consumeInterval) {
        prevTime = currTime;

        /* Will need to poll at frame interval rate */
        if(wsClient.available()) {
          wsClient.poll();
        }        
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

bool testWSConnect() {

    Serial.println("Connecting to Websockets server:");
    Serial.print("Server: ");
    Serial.print(ws_host);
    Serial.print(" port: ");
    Serial.println(ws_port);
    
    if (!wsClient.connect(ws_host, ws_port, "/")) {
      Serial.print("Websockets connection failed! ");
      return false;        
    }
    else 
    {
      Serial.println("Connected!...");
      wsClient.send("Connection established!");
      return true;
    }
 }

void testWSConsume(WebsocketsMessage message) {    
  Serial.print("Consuming message: ");

  /* Add calls to LED functions */
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
            ws_host = configFile["ws_host"];
            ws_port = configFile["ws_port"];

            Serial.print("SSID: ");
            Serial.println(ssid);
            Serial.print("Password: ");    
            Serial.println(password);      
            Serial.print("Websockets host: ");
            Serial.println(ws_host);
            Serial.print("Port: ");    
            Serial.println(ws_port);       
        }    
    }
}
