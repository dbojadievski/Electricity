#include "EventManager.h"
#include <assert.h>

EventManager::EventManager(char const * const pTag, CORE_BOOLEAN isGlobalEventManager)
{
	this->m_pTag = pTag;
	this->m_activeQueueIdx = 0;
}

CORE_BOOLEAN
EventManager::VAddListener(const EventListenerDelegate& eventDelegate, const EventType type)
{
	assert(eventDelegate);

	CORE_BOOLEAN wasRegistered = false;


	EventListenerList listeners = this->m_Listeners[type];
	for (auto iterator = listeners.begin(); iterator != listeners.end(); ++iterator)
	{
		if (eventDelegate == (*iterator))
		{
			assert("Re-registering a delegate attempted.");
			goto end;
		}
	}

	this->m_Listeners[type].push_back(eventDelegate);
	wasRegistered = true;

end:
	return wasRegistered;
}

CORE_BOOLEAN
EventManager::VRemoveListener(const EventListenerDelegate& eventDelegate, const EventType type)
{
	assert(eventDelegate);

	CORE_BOOLEAN wasDetached = false;

	auto findIt = this->m_Listeners.find(type);
	if (findIt != this->m_Listeners.end())
	{
		EventListenerList& listeners = findIt->second;
		for (auto it = listeners.begin(); it != listeners.end(); ++it)
		{
			if (eventDelegate != (*it))
				continue;

			listeners.erase(it);
			wasDetached = true;
			break;
		}
	}

	return wasDetached;
}

CORE_BOOLEAN
EventManager::VTriggerEvent(IEventData * pEvent) const
{
	assert(pEvent);

	CORE_BOOLEAN wasProcessed = false;

	EventType eType = pEvent->VGetEventType();
	auto findIt = this->m_Listeners.find(eType);
	if (findIt != this->m_Listeners.end())
	{
		const EventListenerList& typeListeners = findIt->second;
		for (EventListenerList::const_iterator it = typeListeners.begin(); it != typeListeners.end(); ++it)
		{
			EventListenerDelegate listener = (*it);
			listener( (IEventData *)pEvent);
			wasProcessed = true;
		}
	}

	delete pEvent;
	return wasProcessed;
}

CORE_BOOLEAN
EventManager::VQueueEvent(const IEventData * pEvent)
{
	assert(pEvent);
	assert(this->m_activeQueueIdx < EVENT_MANAGER_NUM_QUEUES);

	CORE_BOOLEAN wasEnqueued = false;

	auto findIt = this->m_Listeners.find(pEvent->VGetEventType());
	
	if (findIt != this->m_Listeners.end())
	{
		this->m_Queues[this->m_activeQueueIdx].push_back((IEventData *)pEvent);
		wasEnqueued = true;
	}
	else
		delete pEvent;
	
	return wasEnqueued;
}

CORE_BOOLEAN
EventManager::VAbortEvent(const EventType inType, CORE_BOOLEAN abortAllOfType)
{
	assert(inType);
	assert(this->m_activeQueueIdx < EVENT_MANAGER_NUM_QUEUES);

	CORE_BOOLEAN wasEventAborted = false;

	EventListenerMap::iterator findIt = this->m_Listeners.find(inType);
	if (findIt != this->m_Listeners.end())
	{
		EventQueue& queue = this->m_Queues[this->m_activeQueueIdx];
		auto it = queue.begin();

		while (it != queue.end())
		{
			auto thisIt = it;
			++it;

			if ((*(thisIt))->VGetEventType() != inType)
				continue;

			queue.erase(thisIt);
			auto val = *thisIt;
			delete val;
			wasEventAborted = true;

			if (!abortAllOfType)
				break;
		}
	}
	
	return wasEventAborted;
}

CORE_BOOLEAN
EventManager::VTickUpdate(CORE_ULONG maxMs)
{
	CORE_BOOLEAN areAnyUnprocessed = false;

	/*Swaps the active queue and clears the old one.*/
	CORE_DWORD queueToProcess = this->m_activeQueueIdx;
	this->m_activeQueueIdx = ((this->m_activeQueueIdx + 1) % EVENT_MANAGER_NUM_QUEUES);
	this->m_Queues[this->m_activeQueueIdx].clear();

	while (!this->m_Queues[queueToProcess].empty())
	{
		IEventData * pEvent = this->m_Queues[queueToProcess].front();
		this->m_Queues[queueToProcess].pop_front();
		const EventType eventType = pEvent->VGetEventType();

		/*Find and notify all delegates subscribed to this event. */
		auto findIt = this->m_Listeners.find(eventType);
		if (findIt != this->m_Listeners.end())
		{
			const EventListenerList& listeners = findIt->second;
			for (auto it = listeners.begin(); it != listeners.end(); ++it)
			{
				EventListenerDelegate listener = (*it);
				listener(pEvent);
			}
		}
		delete pEvent;
	}

	areAnyUnprocessed = !this->m_Queues[queueToProcess].empty();
	if (areAnyUnprocessed)
	{
		while (!this->m_Queues[queueToProcess].empty())
		{
			IEventData * pEvent = this->m_Queues[queueToProcess].back();
			this->m_Queues[queueToProcess].pop_back();
			this->m_Queues[this->m_activeQueueIdx].push_front(pEvent);
		}
	}

	return areAnyUnprocessed;
}