import datetime
import paho.mqtt.client as mqtt
import time
import argparse

global mqtt_broker_ip

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    # client.subscribe("$SYS/#")
    
# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))

def mqtt_loop():
    # global mqtt_broker_ip
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    # client.connect("192.168.1.2", 1883, 60)
    client.connect(mqtt_broker_ip, 1883, 60)

    client.loop_start()

    while True:
        time.sleep(5)
        print("loop")
        client.publish("esp32/time", '{}'.format(datetime.datetime.now()))
        
    
def parse_arg():
    global mqtt_broker_ip
    parser = argparse.ArgumentParser(description='Process some integers.')
    parser.add_argument('--ip', type=str, help='IP address of MQTT broker')
    args = parser.parse_args()
    if(args.ip):
        mqtt_broker_ip = args.ip
        print("MQTT broker IP:"+mqtt_broker_ip)

def main():
    print ('Current date/time: {}'.format(datetime.datetime.now()))
    mqtt_loop()

if __name__ == "__main__":
    parse_arg()
    # execute only if run as a script
    main()