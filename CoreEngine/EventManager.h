/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*	@File EventManager.h									   */
/*	@Author Dino 'Sourcery' Bojadjievski					   */
/*													           */		
/*  An event dispatcher for use as infrastructure.             */
/*  Graciously based on the excellent work by Mike McShaffy.   */
/*                                                             */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "CoreTypes.h"
#include "IEventData.h"
#include "IEventManager.h"

#include <list>
#include <map>

using namespace std;
const unsigned int EVENT_MANAGER_NUM_QUEUES = 2;

/*
 * NOTE(Dino): 
 * Event processing is loosely based on N-buffer rendering.
 *
 * The system maintains N event queues which it fills up.
 * It processes a number of events from an 'active' queue for a maximum amount of time.
 * Once this time has elapsed, it overflows the rest of the events into the next queue.*
 * On the next frame, processing continues from the queue events were overflown into.
 *
 * An immediate event dispatch, 'VTriggerEvent', is available.
 * Its usage is to be very strictly limited.
 */
class EventManager : public IEventManager
{
	typedef list<EventListenerDelegate> EventListenerList;
	typedef map<EventType, EventListenerList> EventListenerMap;
	typedef list<IEventData *> EventQueue;

	const char * m_pTag;
	EventListenerMap m_Listeners;
	EventQueue m_Queues[EVENT_MANAGER_NUM_QUEUES];
	size_t m_activeQueueIdx; //Denominates the queue currently undergoing processing. Events enqueue to the opposing queue.

public:
	explicit EventManager(const char * pTag, CORE_BOOLEAN isGlobalEventManager);
	virtual ~EventManager(void) {}

	virtual CORE_BOOLEAN VAddListener(const EventListenerDelegate& eventDelegate, const EventType& type);
	virtual CORE_BOOLEAN VRemoveListener(const EventListenerDelegate& eventDelegate, const EventType& type);

	virtual CORE_BOOLEAN VTriggerEvent(const IEventData * pEventData) const;
	virtual CORE_BOOLEAN VQueueEvent(const IEventData * pEventData);
	virtual CORE_BOOLEAN VAbortEvent(const EventType& type, CORE_BOOLEAN abortAllOfType = false);

	virtual CORE_BOOLEAN VTickUpdate(CORE_ULONG maxMs = kInfinite);
};