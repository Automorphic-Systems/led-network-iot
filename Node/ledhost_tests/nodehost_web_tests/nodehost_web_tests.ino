#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <ArduinoJson.h>

/* Config file */
#define LEDHOST_CONFIG "/ledhost_config.json"

StaticJsonDocument<1024> configFile;

/* WiFi Credentials */
const char* ssid;
const char* password;

/* HTTP Setup */
ESP8266WebServer server(8000);


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

  /* Verify web host setup */
  testWebHostSetup();
}

/* Loop */
void loop() {    
    server.handleClient();

    
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

void testWebHostSetup() {
    server.on("/", handleRoot);
    server.on("/testget", testGet);
    server.on("/testpost", HTTP_POST, testPost);
    server.on("/led_on", handleLedOn);
    server.on("/led_off", handleLedOff);
    server.begin();
    Serial.println ( "HTTP server started" );  
}

/* Web Server Handlers */
void handleRoot() {
 server.send(200, "text/html", "<h2>Node MCU LED Host</h2><br/>IP Address - " + WiFi.localIP().toString());
}

void handleLedOn() {
    Serial.println("Turning on board LED on..");
    digitalWrite(LED_BUILTIN, 0);
    server.send(200, "text/plain","{ result: 'led on' }");
}

void handleLedOff() {
    Serial.println("Turning on board LED off..");
    digitalWrite(LED_BUILTIN, 1);
    server.send(200, "text/plain","{ result: 'led off' }");
}

void testGet() {   
    server.send(200, "text/plain","{ result: 'get' }");
}

void testPost() {   
    String response = "{ result: '";
    response += server.arg("test");
    response += "'}";
    server.send(200, "text/plain", response);
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

            Serial.print("SSID: ");
            Serial.println(ssid);
            Serial.print("Password: ");    
            Serial.println(password);            
        }    
    }
}
