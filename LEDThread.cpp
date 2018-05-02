/**
 * Copyright (c) 2017, Autonomous Networks Research Group. All rights reserved.
 * Developed by:
 * Autonomous Networks Research Group (ANRG)
 * University of Southern California
 * http://anrg.usc.edu/
 *
 * Contributors:
 * Jason A. Tran <jasontra@usc.edu>
 * Bhaskar Krishnamachari <bkrishna@usc.edu>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy 
 * of this software and associated documentation files (the "Software"), to deal
 * with the Software without restriction, including without limitation the 
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
 * sell copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 * - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimers.
 * - Redistributions in binary form must reproduce the above copyright notice, 
 *     this list of conditions and the following disclaimers in the 
 *     documentation and/or other materials provided with the distribution.
 * - Neither the names of Autonomous Networks Research Group, nor University of 
 *     Southern California, nor the names of its contributors may be used to 
 *     endorse or promote products derived from this Software without specific 
 *     prior written permission.
 * - A citation to the Autonomous Networks Research Group must be included in 
 *     any publications benefiting from the use of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH 
 * THE SOFTWARE.
 */

/**
 * @file       LEDThread.cpp
 * @brief      Implementation of thread that handles LED requests.
 *
 * @author     Jason Tran <jasontra@usc.edu>
 * @author     Bhaskar Krishnachari <bkrishna@usc.edu>
 */

#include "mbed.h"
#include "m3pi.h"
#include "LEDThread.h"
#include "MQTTmbed.h"
#include "MQTTNetwork.h"

#include "MQTTClient.h"




m3pi m3pi(p23, p9, p10);

Mail<MailMsg, LEDTHREAD_MAILBOX_SIZE> LEDMailbox;

static DigitalOut led2(LED2);

static const char *topic = "anrg-pi14/led-thread";
static const char *topic_speed = "anrg-pi14/speed-thread";

int cindyeats = 0; //definition of the flag





void LEDThread(void *args) 
{
    MQTT::Client<MQTTNetwork, Countdown> *client = (MQTT::Client<MQTTNetwork, Countdown> *)args;
    MailMsg *msg;
    MQTT::Message message;
    osEvent evt;
    char pub_buf[16];
    char pub_buf_speed[16];


    int ten_speed = 0;
    int one_speed = 0;
    char t_char_speed = '0';
    char o_char_speed = '0';
    int speed = 10;

    //for heartbeat
    int hundred = 0;
    int ten = 0;
    int one = 0;
    char h_char = '0';
    char t_char = '0';
    char o_char = '0';
    int heartbeat = 0;

    AnalogIn ain(p20);

    printf("entered LEDThread\n");


    //for speed
    ten_speed = speed / 10;
    one_speed = speed - ten_speed*10;
    t_char_speed = '0' + ten_speed;
    o_char_speed = '0' + one_speed;


    float b_tot = 0;
    float buffer1[10]; //100ms *10 = 1 sec
    float bpm_buf[2]; //average buffer to get rid of the noise
    int index1 = 0;
    int index2 = 5;
    float period = 0; //peak to peak period


    //publish to P2UX
    pub_buf_speed[0] = t_char_speed;
    pub_buf_speed[1] = o_char_speed;
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;
    message.payload = (void*)pub_buf_speed;
    message.payloadlen = 2; //MQTTclient.h takes care of adding null char?
    /* Lock the global MQTT mutex before publishing */
    mqttMtx.lock();
    client->publish(topic_speed, message);
    mqttMtx.unlock();
    m3pi.forward(speed);
    printf("published speed to P2UX\n");


    while(1) {

        printf("entered the while loop\n");
            for(int k = 0; k < 2; k++)
            {
                    for (int j = 0; j < 10; j++) //put 10 raw analog input values into a buffer array
                    {
                            buffer1[j] = static_cast<int>(ain.read()*100);
                            Thread::wait(100); //1000 for 1 sec
                    }


                            //determine the period of max -> min -> max in buffer1 OR min -> max -> min
                    if ((buffer1[4] - buffer1[0]) < 0) //likely to be max -> min -> max
                    {

                            //analog value decreases and increases in this case
                            //toggle the point where it stops increasing, get the index of max1
                            for (int i = 0; i < 5; i++)
                            {
                                    if (buffer1[index1] < buffer1[i])
                                    {
                                        index1 = i;
                                    }
                            }

                            //get the index of max2
                            for (int i = 5; i < 10; i++)
                            {
                                    if (buffer1[index2] < buffer1[i])
                                    {
                                        index2 = i;
                                    }
                            }

                            //each index represents 0.1 sec
                            period = (index2 - index1)*0.1; //unit = sec
                            bpm_buf[k] = 60.0 / period;
                            printf("    bpm %d= %f \n",k, bpm_buf[k]);
                            printf("\n");
                            printf("\n");
                    }
                    else if ((buffer1[4] - buffer1[0]) > 0) //likely to be min -> max -> min
                    {




                            //analog value increases and decreases in this case
                            //toggle the point where it stops decreasing, get the index of min1
                            for (int i = 0; i < 5; i++)
                            {
                                if (buffer1[index1] > buffer1[i] && buffer1[i] > 0)
                                {
                                    index1 = i;
                                }
                            }


                            //get the index of min2
                            for (int i = 5; i < 10; i++)
                            {
                                if (buffer1[index2] > buffer1[i] && buffer1[i] > 0)
                                {
                                   index2 = i;
                                }
                            }

                            //each index represents 0.1 sec
                            period = (index2 - index1)*0.1; //unit = sec
                            bpm_buf[k] = 60 / period;
                            printf("    bpm %d= %f \n", k, bpm_buf[k]);
                            printf("\n");
                            printf("\n");
                    }
            }

            b_tot = bpm_buf[0] + bpm_buf[1];
            printf("BPM = %f\n", b_tot*0.5);
            printf("\n");
            printf("\n");
            printf("\n");
            heartbeat = b_tot/2;
            //for heartbeat sensor
            hundred = heartbeat / 100;
            ten = heartbeat / 10 - hundred*10;
            one = heartbeat - ten*10 - hundred*100;
            t_char = '0' + ten;
            o_char = '0' + one;
            h_char = '0' + hundred;




        //for speed
        ten_speed = speed / 10;
        one_speed = speed - ten_speed*10;
        t_char_speed = '0' + ten_speed;
        o_char_speed = '0' + one_speed;


        printf("about to get mailbox\n");
        evt = LEDMailbox.get();

        printf("speed now = %d\n", speed);

        if(evt.status == osEventMail) 
        {
            msg = (MailMsg *)evt.value.p;

            switch (msg->content[1]) 
            {
                case LED_THR_PUBLISH_MSG:
                    printf("LEDThread: received command to publish to topic"
                           "m3pi-mqtt-example/led-thread\n");
                    //printf("%d\n", heartbeat);
                    //printf("%d, %d, %d\n",hundred, ten, one );
                    pub_buf[0] = h_char;
                    pub_buf[1] = t_char;
                    pub_buf[2] = o_char;
                    message.qos = MQTT::QOS0;
                    message.retained = false;
                    message.dup = false;
                    message.payload = (void*)pub_buf;
                    message.payloadlen = 3; //MQTTclient.h takes care of adding null char?
                    /* Lock the global MQTT mutex before publishing */
                    mqttMtx.lock();
                    client->publish(topic, message);
                    mqttMtx.unlock();
                    break;
                case 51:
                    printf("Speeding up... \n");
                    speed = speed + 3;
                    printf("speed is updated to %d \n",speed);
                    pub_buf_speed[0] = t_char_speed;
                    pub_buf_speed[1] = o_char_speed;
                    message.qos = MQTT::QOS0;
                    message.retained = false;
                    message.dup = false;
                    message.payload = (void*)pub_buf_speed;
                    message.payloadlen = 2; //MQTTclient.h takes care of adding null char?
                    /* Lock the global MQTT mutex before publishing */
                    mqttMtx.lock();
                    client->publish(topic_speed, message);
                    mqttMtx.unlock();
                    break;
                case 50:
                    printf("Slowing down\n");
                    speed = speed - 3;
                    printf("speed is updated to %d \n",speed);
                    pub_buf_speed[0] = t_char_speed;
                    pub_buf_speed[1] = o_char_speed;
                    message.qos = MQTT::QOS0;
                    message.retained = false;
                    message.dup = false;
                    message.payload = (void*)pub_buf_speed;
                    message.payloadlen = 2; //MQTTclient.h takes care of adding null char?
                    /* Lock the global MQTT mutex before publishing */
                    mqttMtx.lock();
                    client->publish(topic_speed, message);
                    mqttMtx.unlock();
                    break;
                default:
                    printf("LEDThread: invalid message\n");
                    break;
            }            

            LEDMailbox.free(msg);
        }
        
        m3pi.forward(speed);




    } /* while */

    /* this should never be reached */
}

Mail<MailMsg, LEDTHREAD_MAILBOX_SIZE> *getLEDThreadMailbox() 
{
    return &LEDMailbox;
}
