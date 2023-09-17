import paho.mqtt.client as mqtt
import json
import tkinter as tk
from tkinter import ttk
import time

# Your AWS IoT Core endpoint and certificate paths
AWS_IOT_ENDPOINT = "a2oir44n87lfks-ats.iot.eu-north-1.amazonaws.com"
CA_PATH = "AmazonRootCA1.pem"
CERT_PATH = "client.pem.crt"
KEY_PATH = "client.pem.key"

def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")

def on_disconnect(client, userdata, rc):
    if rc != 0:
        print("Unexpected disconnection; attempting reconnection")
        # Here you can also add delay if needed using time.sleep()
        client.reconnect()

# Initialize MQTT client
client = mqtt.Client()
client.on_connect = on_connect
client.on_disconnect = on_disconnect

# Configure TLS settings
client.tls_set(ca_certs=CA_PATH, certfile=CERT_PATH, keyfile=KEY_PATH)

# Connect to AWS IoT Core
client.connect(AWS_IOT_ENDPOINT, 8883, 60)

# Function to publish message
def publish_message(message):
    payload = json.dumps({"message": message})
    topic = "LED/toggle"
    client.publish(topic, payload)
    print(f"Published message {message} to topic {topic}")

# Tkinter GUI setup
root = tk.Tk()
root.title("MQTT Client")

frame = ttk.Frame(root, padding="10")
frame.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))

# Call Button
call_button = ttk.Button(frame, text="Call", command=lambda: publish_message("call"))
call_button.grid(row=0, column=0, sticky=tk.W)

# Reset Button
reset_button = ttk.Button(frame, text="Reset", command=lambda: publish_message("reset"))
reset_button.grid(row=0, column=1)

# Quit Button
quit_button = ttk.Button(frame, text="Quit", command=root.quit)
quit_button.grid(row=0, column=2)

# Start the loop in a different thread to keep Tkinter responsive
client.loop_start()

root.mainloop()

# Disconnect when the GUI is closed
client.loop_stop()
client.disconnect()
