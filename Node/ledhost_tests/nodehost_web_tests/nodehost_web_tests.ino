#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>


/* WiFi Credentials */
const char *ssid = "AutomorphicSystems"; //WIFI ssid
const char *password = "walkingonthemoon"; //WIFI password

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
