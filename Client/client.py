# Client.py - Subscribe client to sensor data
# 
#   Requires paho-mqtt
#       pip install paho-mqtt

import paho.mqtt.client as mqtt
import time
import secrets
import json
from datetime import datetime
import os


def logToFile(topic, payload):

    try:
        # Convert payload from bytes to dictionary
        data = json.loads(payload.decode('utf-8'))
        
        # Extract required fields
        ts = data['ts']
        dev = data['dev']
        
        # Get current date for filename
        current_date = datetime.now()
        month = str(current_date.month).zfill(2)
        day = str(current_date.day).zfill(2)
        
        # Determine filename and CSV format based on topic
        # Convert Unix timestamp to formatted time
        timestamp_datetime = datetime.fromtimestamp(ts)
        formatted_time = timestamp_datetime.strftime("%I:%M %p")  # 12-hour format with AM/PM
        
        if 'info' in topic:
            filename = f"{dev}-info-{month}-{day}.csv"
            csv_line = f"{ts},{formatted_time}\n"
        elif 'data' in topic:
            filename = f"{dev}-data-{month}-{day}.csv"
            msg = data['msg']
            temp = data['temp']
            rh = data['RH']
            csv_line = f"{ts},{formatted_time},{msg},{temp},{rh}\n"
        else:
            return  # Skip if topic doesn't match expected patterns
        
        # Ensure we have write permissions (creates directory if needed)
        os.makedirs('logs', exist_ok=True)
        filepath = os.path.join('logs', filename)
        
        # Append data to file
        with open(filepath, 'a') as f:
            f.write(csv_line)
            
    except json.JSONDecodeError as e:
        print(f"Error decoding JSON payload: {e}")
    except KeyError as e:
        print(f"Missing required field in payload: {e}")
    except IOError as e:
        print(f"Error writing to file: {e}")
    except Exception as e:
        print(f"Unexpected error: {e}")



############
# Callbacks
############
def on_connect(client, userdata, flags, rc, properties=None):
    print("CONNACK received with code %s." % rc)

def on_message(client, userdata, msg):
    print( str(int(time.time())) + ": " + msg.topic + " " + str(msg.payload))

    logToFile( msg.topic,  msg.payload )


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

