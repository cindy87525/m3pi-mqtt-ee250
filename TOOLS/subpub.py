import sys
sys.path.append('../../Software/Python/')
sys.path.append('../GrovePi-EE250/')
import paho.mqtt.client as mqtt
import time
import math
import os


"""def led_callback(client, userdata, message):
    global led
    global flag
    print("custom_callback: " + message.topic + " " + str(message.payload, "utf-8"))
    print("custom_callback: message.payload is of type " + 
          str(type(message.payload)))

    data = str(message.payload, "utf-8")"""



def on_connect(client, userdata, flags, rc):
    print("Connected to server (i.e., broker) with result code "+str(rc))

    #subscribe to topics of interest here
    client.subscribe("m3pi-mqtt-ee250/led-thread")
    #client.subscribe("anrg-pi14/lcd")
    #client.message_callback_add("m3pi-mqtt-ee250/speed-thread", speed_callback)
    #client.message_callback_add("m3pi-mqtt-ee250/led-thread", led_callback)



if __name__ == '__main__':
    client = mqtt.Client()
    client.on_connect = on_connect
    #use the port address for usc eclipse
    client.connect(host="eclipse.usc.edu", port=11000, keepalive=60)
    client.loop_start()


            
    
    #cmd = r'echo -ne "\x01\x00" | mosquitto_pub -h eclipse.usc.edu -p 11000 -t "m3pi-mqtt-ee250" -s'

    while True:
            # This example uses the blue colored sensor. 
            # The first parameter is the port, the second parameter is the type of sensor. 
        os.system(cmd)
        #os.system("mosquitto_pub -h eclipse.usc.edu -p 11000 -t 'm3pi-mqtt-ee250' -s")
        #os.system("mosquitto_pub -h eclipse.usc.edu -p 11000 -t 'm3pi-mqtt-ee250' -s")
        time.sleep(0.5)
