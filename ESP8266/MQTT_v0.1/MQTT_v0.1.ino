

/*
Source code for the ESP8266 <-> Odroid XU4 using the MQTT protocol
Sharing temperature and humidity readings over WiFi to the server
Peter Jett 01/08/2018
Version 0.1
 */

/******************* Includes *****************************/
#include <ESP8266WiFi.h> // for ESP8266 (Huzzah breakout in this instance)
#include <DHT.h>
#include <DHT_U.h>
#include <PubSubClient.h>
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
  
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
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
  Serial.begin(9600);   // serial port configuration, really only used for debugging purposes
  delay(10);            // give a couple seconds, just to be clean
  reconnect();          // connect to the Wifi on first boot (called later if we lose comm...)
}

int timeSinceLastRead = 0;
void loop() {

  // Report every 2 seconds.
  if(timeSinceLastRead > 30000) {
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    // We are going to wait for every 30 seconds to log a new value... this can be changed...
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    String temp = (String)t;
    String hum = (String)h;
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      timeSinceLastRead = 0;
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
      delay(200);
      client.publish(myMQTT_Topic1, String(hum).c_str() ,true);                      // humidity shared with the server
      delay(500); 
      client.publish(myMQTT_Topic0, String(temp).c_str(),true);
    }

    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.print(" *C ");
    
    timeSinceLastRead = 0;
  }
  delay(100);
  
  timeSinceLastRead += 100;


  


}
