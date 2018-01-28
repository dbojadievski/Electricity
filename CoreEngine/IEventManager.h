/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*	@File IEventManager.h									   */
/*	@Author Dino 'Sourcery' Bojadjievski					   */
/*													           */
/*  An event dispatcher for use as infrastructure.             */
/*  Graciously based on the excellent work by Mike McShaffy.   */
/*  Graciously uses the astounding FastDelegate library.       */
/*  FastDelegate courtesy of Don Clugston                      */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma once

#include "CoreTypes.h"
#include "IEventData.h"

class IEventManager
{
public:
	enum EConstants { kInfinite = 0xffffffff};

	virtual CORE_BOOLEAN VAddListener(const EventListenerDelegate& eventDelegate, const EventType type) = 0;
	virtual CORE_BOOLEAN VRemoveListener(const EventListenerDelegate& eventDelegate, const EventType type) = 0;

	virtual CORE_BOOLEAN VTriggerEvent(IEventData * pEvent) const = 0;
	virtual CORE_BOOLEAN VQueueEvent(const IEventData * pEvent) = 0;
	virtual CORE_BOOLEAN VAbortEvent(const EventType type, CORE_BOOLEAN abortAllOfType = false) = 0;

	virtual CORE_BOOLEAN VTickUpdate(CORE_ULONG maxMs = kInfinite) = 0;


	static IEventManager* GetGlobalEventManager(void);
};	