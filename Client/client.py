# Client.py - Subscribe client to sensor data
# 
#   Requires paho-mqtt
#       pip install paho-mqtt

import paho.mqtt.client as mqtt
import time
import secrets



############
# Callbacks
############
def on_connect(client, userdata, flags, rc, properties=None):
    print("CONNACK received with code %s." % rc)

def on_message(client, userdata, msg):
    print( str(int(time.time())) + ": " + msg.topic + " " + str(msg.payload))


# -Code start- #

client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message

client.tls_set()
client.username_pw_set(secrets.USERNAME, secrets.PASSWORD)
client.connect(secrets.MQTT_URL, secrets.MQTT_PORT)

client.subscribe("#", qos=1)

#client.publish("topic", payload="data_string")


# loop_forever
try:
    client.loop_forever()
except KeyboardInterrupt:
    client.disconnect()
except Exception as e:
    print("Error:", e)

print("Exiting....")

