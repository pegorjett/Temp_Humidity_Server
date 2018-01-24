/*
Source code for the ESP8266 <-> Odroid XU4 using the MQTT protocol
Sharing temperature and humidity readings over WiFi to the server
Peter Jett 01/08/2018
Version 0.1
 */

/******************* Includes *****************************/
#include <ESP8266WiFi.h> // for ESP8266 (Huzzah breakout in this instance)
#include "DHT.h"
#include <PubSubClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
/******************* DHT22 configuration *****************************/
#define DHTPIN 14     // what digital pin the DHT22 is conected to
#define DHTTYPE DHT22   // there are multiple kinds of DHT sensors
DHT dht(DHTPIN, DHTTYPE);


/******************* WiFi Configuration *****************************/
const char* ssid = "NETGEAR90";                         // SSID of local network this device will be connected to
const char* wifi_password = "fancyunit461";             // Password of local network this device will be connected to

/******************* MQTT Configuration *****************************/
const char* myMQTT_Server = "192.168.0.41";           // IP of Odroid XU4 server (or whatever device is being used as 'server'
const char* myMQTT_Topic0 = "Temperature";            // define a topic (temperature, humidity, etc.)
const char* myMQTT_Topic1 = "Humidity";            // define a topic (temperature, humidity, etc.)
const char* myMQTT_Username = "gregsroom";            // for security, something unique but shared between all devices
const char* myMQTT_Password = "warbucks";             // for security, something unique but shared between all devices
const char* myMQTT_clientID = "Sensor";             // define an identification for the device (this will be different for each device on the network, if using multiple ESP8266...)

int sensorID = 0;

// Initialize the MQTT server and client objects
WiFiClient server;       //Service Port
PubSubClient client(myMQTT_Server, 1883, server);  // make 1883 is the listener port for the Broker

void reconnect() {
  // Connect to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
   
  WiFi.begin(ssid, wifi_password);
  
  // Set a static IP (each ESP8266 in the MQTT network will need its own IP...) 
  IPAddress ip(192, 168, 0, 55);
  IPAddress gateway(192, 168, 0, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.config(ip, gateway, subnet);
  
  
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.println("");
  Serial.println("WiFi connected");
   
  // Start the client
  WiFi.begin();
  Serial.println("Client started");
 
  // Print the IP address
  Serial.print(WiFi.localIP());   // should be the static IP defined above....

}

void setup() {
  Serial.begin(115200);   // serial port configuration, really only used for debugging purposes
  delay(10);            // give a couple seconds, just to be clean
  reconnect();          // connect to the Wifi on first boot (called later if we lose comm...)

  // No authentication by default
  ArduinoOTA.setPassword("admin");
  
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

unsigned long timeSinceLastRead = 0;
void loop() {

  ArduinoOTA.handle();

  // Report every 2 seconds.
  if(timeSinceLastRead < millis() ) {
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    // We are going to wait for every 30 seconds to log a new value... this can be changed...
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    String temp = (String)t;
    String hum = (String)h;
    delay(250);
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      timeSinceLastRead = millis() + 30000;  // every 30 seconds do another grab of data
      return;
    }
         // publish the temperature to the MQTT broker (our ODROID-XU4 in this example)
    if (client.publish(myMQTT_Topic0, String(temp).c_str(),true) && client.publish(myMQTT_Topic1, String(hum).c_str() ,true)) {                  // client.publish returns a boolean value indicating success/failure, if we got here then it sent
      Serial.println("Temp and humidity sent to server");
    }
    else {                                                            // the message didnt send, keep trying (we do this forever
      Serial.println("Did not send temp to broker. Trying again....");
      client.connect(myMQTT_clientID, myMQTT_Username, myMQTT_Password);
      delay(10);                                                       // This delay ensures that client.publish doesn't clash with the client.connect call
      client.publish(myMQTT_clientID, String(sensorID).c_str() ,true);                      // sensorID
      client.publish(myMQTT_Topic1, String(hum).c_str() ,true);                      // humidity shared with the server
      client.publish(myMQTT_Topic0, String(temp).c_str(),true);
    }

    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.print(" *C ");
    
    timeSinceLastRead = millis() + 300000;  // every 5 minutes do another grab of data
  }  


}
