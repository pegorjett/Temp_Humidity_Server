"""
Python MQTT Subscription client
Thomas Varnish (https://github.com/tvarnish), (https://www.instructables.com/member/Tango172)
Written for my Instructable - "How to use MQTT with the Raspberry Pi and ESP8266"
"""
import paho.mqtt.client as mqtt
from store_Sensor_Data_to_DB import sensor_Data_Handler
import json
import random, threading, json
from datetime import datetime
from Write_DB_to_CSV import CSV_export
# Don't forget to change the variables for the MQTT broker!
mqtt_username = "gregsroom"
mqtt_password = "warbucks"
mqtt_topic0 = "Temperature"
mqtt_topic1 = "Humidity"
mqtt_broker_ip = "192.168.0.41"

client = mqtt.Client()
# Set the username and password for the MQTT client
client.username_pw_set(mqtt_username, mqtt_password)

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    # print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("Sensor")
    client.subscribe("Temperature")
    client.subscribe("Humidity")
    
def on_message(client, userdata, msg):
    # This function is called everytime the topic is published to.
    # If you want to check each message, and do something depending on
    # the content, the code to do this should be run in this function    
    # print "Topic: ", msg.topic + ": " + str(msg.payload)
    if msg.topic == "Temperature":
	    Temperature_Data = {}
	    Temperature_Data['Sensor_ID'] = '1'
	    Temperature_Data['Date'] = (datetime.today()).strftime("%d-%b-%Y %H:%M:%S:%f")
            Temperature_Data['Temperature'] = str(msg.payload)
	    temperature_json_data = json.dumps(Temperature_Data)
            sensor_Data_Handler(msg.topic, temperature_json_data)
	    CSV_export()
    if msg.topic == "Humidity":
	    Humidity_Data = {}
	    Humidity_Data['Sensor_ID'] = '1'
            Humidity_Data['Date'] = (datetime.today()).strftime("%d-%b-%Y %H:%M:%S:%f")
            Humidity_Data['Humidity'] = str(msg.payload)
            humidity_json_data = json.dumps(Humidity_Data)
            sensor_Data_Handler(msg.topic, humidity_json_data)	
            CSV_export()			
# Here, we are telling the client which functions are to be run
# on connecting, and on receiving a message
client.on_connect = on_connect
client.on_message = on_message

# Once everything has been set up, we can (finally) connect to the broker
# 1883 is the listener port that the MQTT broker is using
client.connect(mqtt_broker_ip, 1883)

# Once we have told the client to connect, let the client object run itself
client.loop_forever()
client.disconnect()
