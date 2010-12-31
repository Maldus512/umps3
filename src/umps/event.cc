/* -*- mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * uMPS - A general purpose computer system simulator
 *
 * Copyright (C) 2004 Mauro Morsiani
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/****************************************************************************
 * 
 * This module implements some classes used from the SystemBus for the
 * scheduling of device events (such as device operations completion and
 * interrupts generation).  They are: Event class, to keep track of single
 * events, as required by devices; and EventQueue class, to organize the
 * Events into a time-ordered queue.
 *
 ***************************************************************************/

#include "umps/event.h"

#include <stdio.h>
#include <stdlib.h>

#include "umps/const.h"
#include "umps/utility.h"
#include "umps/time_stamp.h"

// This method creates a new Event object and initalizes it
Event::Event(TimeStamp * ts, Word inc, unsigned int il, unsigned int dev)
{
    time = new TimeStamp(ts, inc);
    intL = il;
    devNum = dev;
    next = NULL;
}

// This method deletes a Event object and its internal structures
Event::~Event()
{
    delete time;
}

// This method returns the interrupt line of the device requiring the Event
unsigned int Event::getIntLine()
{
    return (intL);
}

// This method returns the device number of the device requiring the Event
unsigned int Event::getDevNum()
{
    return (devNum);
}

// This method returns the TimeStamp access pointer
TimeStamp *Event::getTS()
{
    return (time);
}

// This method links an Event to its successor in a structure
void Event::AddBefore(Event * ev)
{
    next = ev;
}

// This method inserts an Event after another, linking the former to the 
// successor of the latter
void Event::InsAfter(Event * ev)
{
    next = ev->next;
    ev->next = this;
}

// This method returns the pointer to the successor of an Event
Event *Event::Next()
{
    return (next);
}


// This method creates a new (empty) queue
EventQueue::EventQueue()
{
    head = NULL;
    lastIns = NULL;
}

// This method deletes the queue and its associated structures
EventQueue::~EventQueue()
{
    Event *p, *q;

    p = head;
    q = NULL;
    while (p != NULL) {
        q = p->Next();
        delete p;
        p = q;
    }
}

// This method returns TRUE if the queue is empty, FALSE otherwise
bool EventQueue::IsEmptyQ()
{
    if (head == NULL)
        return (true);
    else
        return (false);
}

// This method returns a pointer to the timestamp of the EventQueue head, 
// if queue is not empty, and NULL otherwise
TimeStamp *EventQueue::getHTS()
{
    if (!IsEmptyQ())
        return (head->getTS());
    else
        return (NULL);
}


// This method returns the interrupt line of the EventQueue head, 
// if not empty (0 otherwise)
unsigned int EventQueue::getHIntLine()
{
    if (!IsEmptyQ())
        return (head->getIntLine());
    else
        return (0);
}


// This method returns the device number of the EventQueue head, 
// if not empty (0 otherwise)
unsigned int EventQueue::getHDevNum()
{
    if (!IsEmptyQ())
        return (head->getDevNum());
    else
        return (0);
}


// This method creates a new Event object and inserts it in the
// EventQueue; EventQueue is sorted on ascending time order
TimeStamp *EventQueue::InsertQ(TimeStamp * ts, Word inc, unsigned int il,
                               unsigned int dev)
{
    Event *ins, *p, *q;

    ins = new Event(ts, inc, il, dev);
    if (IsEmptyQ()) {
        head = ins;
    } else if ((ins->getTS())->LessEq(head->getTS())) {
        // "ins" has to happen before that at the head of the queue;
        // should be put before it
        ins->AddBefore(head);
        head = ins;
    } else {
        // should find place in queue: check lastIns to shorten search time 
        if (lastIns != NULL && !((ins->getTS())->LessEq(lastIns->getTS())))
            // can start from lastIns
            p = lastIns;
        else
            // must start from the head
            p = head;

        q = p;
        while (p != NULL && (p->getTS())->LessEq(ins->getTS())) {
            // traverse the queue 
            q = p;
            p = p->Next();
        }
        // place found: insert after q and before p
        ins->InsAfter(q);
    }
    lastIns = ins;
    return (ins->getTS());
}

// This method removes the head of a (not empty) queue and sets it to the
// following Event
void EventQueue::RemoveHead()
{
    Event *p;

    if (!IsEmptyQ()) {
        p = head;
        head = head->Next();

        if (p == lastIns)
	    // reposition lastIns to the new head  
	    lastIns = head;

	delete p;
    }
}
