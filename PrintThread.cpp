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
 * @file       PrintThread.cpp
 * @brief      Implementation of thread that handles print requests.
 *
 * @author     Jason Tran <jasontra@usc.edu>
 * @author     Bhaskar Krishnachari <bkrishna@usc.edu>
 */

#include "PrintThread.h"
#include "MQTTmbed.h"
#include "MQTTNetwork.h"
#include "mbed.h"

Mail<MailMsg, PRINTTHREAD_MAILBOX_SIZE> PrintThreadMailbox;

static const char *topic = "anrg-pi14/led-thread";

/* When you read any .c or .cpp files, you often want to open their 
   corresponding header file and read them simultaneously. */
void printThread() 
{
    MailMsg *msg; // see MailMsg.h for this type
    osEvent evt; 

    while(1) {
        /* Get anything from the PrintThread's mailbox. If it's empty, this 
           call will block. Once something is put inside the mailbox, mbed OS
           will wake this thread up and resume. This structure is what makes 
           this thread event-based. In the current structure, the PrintThread 
           is waiting to receive mail from the MQTT callback messageArrived()
           defined in main.cpp */
        evt = PrintThreadMailbox.get();

        /* Double check if the event type is a new piece of mail */
        if(evt.status == osEventMail) {
            msg = (MailMsg *)evt.value.p;

            /* the second byte in the the content of the message tells us what
               action to "dispatch." The message types are defined in 
               MQTTNetwork.h */

            while (msg->content[1] == 99) {
              printf("printThread: MESSAGE PRINTED SUCCESS!\n");
              Thread::wait(200);
            }

            /* You must always free the message after you're done. Not doing so
               will eventually fill up your mailbox and freeze your entire 
               program. */
            PrintThreadMailbox.free(msg);
        }
    } /* while */

    /* this should never be reached */
}

Mail<MailMsg, PRINTTHREAD_MAILBOX_SIZE> *getPrintThreadMailbox() 
{
    return &PrintThreadMailbox;
}
