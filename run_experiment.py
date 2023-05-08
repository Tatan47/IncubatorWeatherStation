#!/usr/bin/env python3
import os
import csv
import paho.mqtt.client as mqtt
from datetime import datetime
from collections import defaultdict

mqtt_server = "192.168.0.101"
mqtt_topic = "esp32/temperature/+"

device_read_count = defaultdict(int)

def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe(mqtt_topic)

def on_message(client, userdata, msg):
    global device_read_count

    topic_segments = msg.topic.split("/")
    device_id = int(topic_segments[-1].split("_")[-1])

    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    temperature = msg.payload.decode()

    device_read_count[device_id] += 1
    read_count = device_read_count[device_id]
    print(f"ESP32 {device_id}: {temperature} °C at {timestamp}")

    with open(os.path.join(experiment_dir, 'data.csv'), mode='a') as csv_file:
        csv_writer = csv.writer(csv_file)
        csv_writer.writerow([experiment_name, timestamp, device_id, read_count, temperature])

experiment_name = input("Enter the experiment name: ")
scientist_name = input("Enter the scientist name: ")

now = datetime.now()
date_time_str = now.strftime("%Y-%m-%d_%H-%M-%S")
experiment_dir = f"{experiment_name}_{date_time_str}"

os.makedirs(experiment_dir)

with open(os.path.join(experiment_dir, 'metadata.txt'), mode='w') as metadata_file:
    metadata_file.write(f"Experiment Name: {experiment_name}\n")
    metadata_file.write(f"Scientist Name: {scientist_name}\n")
    metadata_file.write(f"Date: {now.strftime('%Y-%m-%d')}\n")
    metadata_file.write(f"Time: {now.strftime('%H:%M:%S')}\n")

with open(os.path.join(experiment_dir, 'data.csv'), mode='w') as csv_file:
    csv_writer = csv.writer(csv_file)
    csv_writer.writerow(["Experiment Name", "Date", "ESP32 Number", "Number of Reads", "Temperature"])

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect(mqtt_server, 1883, 60)
client.loop_forever()
