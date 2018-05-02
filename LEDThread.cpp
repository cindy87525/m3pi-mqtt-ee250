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

static const char *topic = "m3pi-mqtt-ee250/led-thread";

int cindyeats = 0; //definition of the flag



/*

void movement(char command, char speed, int delta_t)
{
    if (command == 's')
    {
        m3pi.forward(speed);
        Thread::wait(delta_t);
        m3pi.stop();
    }    
    else if (command == 'a')
    {
        m3pi.left(speed);
        Thread::wait(delta_t);
        m3pi.stop();
    }   
    else if (command == 'w')
    {
        m3pi.backward(speed);
        Thread::wait(delta_t);
        m3pi.stop();
    }
    else if (command == 'd')
    {
        m3pi.right(speed);
        Thread::wait(delta_t);
        m3pi.stop();
    }
}

*/




void LEDThread(void *args) 
{
    MQTT::Client<MQTTNetwork, Countdown> *client = (MQTT::Client<MQTTNetwork, Countdown> *)args;
    MailMsg *msg;
    MQTT::Message message;
    osEvent evt;
    char pub_buf[16];

    int ten = 0;
    int one = 0;
    char t_char = '0';
    char o_char = '0';
    int heartbeat = 0;
    int speed = 10;
    AnalogIn ain(p20);

    printf("entered LEDThread\n");
/*
        int buf_count = 0;
    float b_tot = 0;
    float buffer1[10]; //100ms *10 = 1 sec
    float bpm_buf[2]; //average buffer to get rid of the noise
    int index1 = 0;
    int index2 = 5;
    float period = 0; //peak to peak period



    while (1)
    {

        //printf("%d   ", i++);
        for (int j = 0; j < 10; j++) //put 10 raw analog input values into a buffer array
        {
            buffer1[j] = ain.read()*100;
            //printf("ratebeat: %f\n", buffer1[j]);
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
            bpm_buf[buf_count] = 60 / period;
            buf_count = buf_count + 1;
            printf("    bpm %d= %f \n",buf_count, bpm_buf[buf_count]);
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
            bpm_buf[buf_count] = 60 / period;
            buf_count = buf_count + 1;
            printf("    bpm %d= %f \n", buf_count, bpm_buf[buf_count]);
        }
        if (buf_count == 2)
        {
            b_tot = bpm_buf[0] + bpm_buf[1];
            printf("BPM = %f\n", b_tot*0.5);
            buf_count = 0;
        }

    }

*/







    while(1) {

        heartbeat = static_cast<int>(ain.read()*100);


        ten = heartbeat / 10;
        one = (heartbeat) - ten*10;
        t_char = '0' + ten;
        o_char = '0' + one;



        evt = LEDMailbox.get();



        printf("speed now = %d\n", speed);

        if(evt.status == osEventMail) 
        {
            msg = (MailMsg *)evt.value.p;

            /* the second byte in the message denotes the action type */
            switch (msg->content[1]) 
            {
                case LED_THR_PUBLISH_MSG:
                    printf("LEDThread: received command to publish to topic"
                           "m3pi-mqtt-example/led-thread\n");
                    pub_buf[0] = t_char;
                    pub_buf[1] = o_char;
                    message.qos = MQTT::QOS0;
                    message.retained = false;
                    message.dup = false;
                    message.payload = (void*)pub_buf;
                    message.payloadlen = 2; //MQTTclient.h takes care of adding null char?
                    /* Lock the global MQTT mutex before publishing */
                    mqttMtx.lock();
                    client->publish(topic, message);
                    mqttMtx.unlock();
                    break;
                case 51:
                    printf("Speeding up... \n");
                    speed = speed + 3;
                    printf("speed is updated to %d \n",speed);
                    break;
                case 50:
                    printf("Slowing down\n");
                    speed = speed - 3;
                    printf("speed is updated to %d \n",speed);
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
