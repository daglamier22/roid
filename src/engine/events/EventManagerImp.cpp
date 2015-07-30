#include "EventManagerImp.h"
#include "utilities/logger.h"

namespace genesis {

#ifdef WIN32
#include <windows.h>
#else
unsigned long GetTickCount() {
	struct timespec ts;
	unsigned long tick = 0;
	if( clock_gettime(CLOCK_MONOTONIC,&ts) != 0 ) {
		GEN_ERROR("Unable to get current tick count");
	}
	tick = ts.tv_nsec / 1000000;
	tick += ts.tv_sec * 1000;
	return tick;
}
#endif

EventManager::EventManager( const std::string name, bool global )
	: IEventManager(name, global)
{
	m_activeQueue = 0;
}//EventManager::EventManager

EventManager::~EventManager()
{
}//EventManager::~EventManager

bool EventManager::addListener( const EventListenerDelegate& eventDelegate, const EventType& type ) {
	GEN_LOG("Events", "Attempting to add delegate function for event type: " + std::to_string(type));
	EventListenerList& eventListenerList = m_eventListeners[type]; //this will find or create the entry
	for( auto it = eventListenerList.begin(); it != eventListenerList.end(); ++it ) {
		if( eventDelegate == (*it) ) {
			GEN_WARNING("Attempting to double-register a delegate");
			return false;
		}
	}

	eventListenerList.push_back(eventDelegate);
	GEN_LOG("Events", "Successfully added delegate for event type: " + std::to_string(type));

	return true;
}//EventManager::addListener

bool EventManager::removeListener( const EventListenerDelegate& eventDelegate, const EventType& type ) {
	GEN_LOG("Events", "Attempting to remove delegate function from event type: " + std::to_string(type));
	bool success = false;

	auto findIt = m_eventListeners.find(type);
	if( findIt != m_eventListeners.end() ) {
		EventListenerList& listeners = findIt->second;
		for( auto it = listeners.begin(); it != listeners.end(); ++it ) {
			if( eventDelegate == (*it) ) {
				listeners.erase(it);
				GEN_LOG("Events", "Successfully removed delegate function from event type: " + std::to_string(type));
				success = true;
				break;
			}
		}
	}

	return success;
}//EventManager::removeListener

bool EventManager::instantEvent( const IEventDataPtr& pEvent ) const {
	GEN_LOG("Events", "Attempting to trigger event " + std::string(pEvent->getName()));
	bool processed = false;

	auto findIt = m_eventListeners.find(pEvent->getEventType());
	if( findIt != m_eventListeners.end() ) {
		const EventListenerList& eventListenerList = findIt->second;
		for( EventListenerList::const_iterator it = eventListenerList.begin(); it != eventListenerList.end(); ++it ) {
			EventListenerDelegate listener = (*it);
			GEN_LOG("Events", "Sending Event " + std::string(pEvent->getName()) + " to delegate.");
			listener(pEvent);
			processed = true;
		}
	}

	return processed;
}//EventManager::instantEvent

bool EventManager::queueEvent( const IEventDataPtr& pEvent ) {
	GEN_ASSERT(m_activeQueue >= 0);
	GEN_ASSERT(m_activeQueue < EVENTMANAGER_NUM_QUEUES);
	if( !pEvent ) {
		GEN_ERROR("Invalid event in queueEvent()");
		return false;
	}

	GEN_LOG("Events", "Attempting to queue event: " + std::string(pEvent->getName()));

	auto findIt = m_eventListeners.find(pEvent->getEventType());
	if( findIt != m_eventListeners.end() ) {
		m_queues[m_activeQueue].push_back(pEvent);
		GEN_LOG("Events", "Successfully queued event: " + std::string(pEvent->getName()));
		return true;
	}
	else {
		GEN_LOG("Events", "Skipping event since there are no delegates registered to receive it: " + std::string(pEvent->getName()));
		return false;
	}
}//EventManager::queueEvent

bool EventManager::queueEventThreadSafe( const IEventDataPtr& pEvent ) {
	if( !pEvent ) {
		return false;
	}

	m_realtimeEventQueue.push(pEvent);

	return true;
}//EventManager::queueEventThreadSafe

bool EventManager::abortEvent( const EventType& type, bool allOfType ) {
	GEN_ASSERT(m_activeQueue >= 0);
	GEN_ASSERT(m_activeQueue < EVENTMANAGER_NUM_QUEUES);

	bool success = false;
	EventListenerMap::iterator findIt = m_eventListeners.find(type);

	if( findIt != m_eventListeners.end() ) {
		EventQueue& eventQueue = m_queues[m_activeQueue];
		auto it = eventQueue.begin();
		while( it != eventQueue.end() ) {
			auto thisIt = it;
			++it;

			if( (*thisIt)->getEventType() == type ) {
				eventQueue.erase(thisIt);
				success = true;
				if( !allOfType )
					break;
			}
		}
	}

	return success;
}//EventManager::abortEvent

bool EventManager::update( unsigned long maxMillis ) {
	unsigned long currMs = GetTickCount();
	unsigned long maxMs = ((maxMillis == IEventManager::kINFINITE) ? (IEventManager::kINFINITE) : (currMs + maxMillis));

	// This section added to handle events from other threads.  Check out Chapter 20.
	IEventDataPtr pRealtimeEvent;
	while( m_realtimeEventQueue.try_pop(pRealtimeEvent) ) {
		queueEvent(pRealtimeEvent);

		currMs = GetTickCount();
		if( maxMillis != IEventManager::kINFINITE ) {
			if( currMs >= maxMs ) {
				GEN_ERROR("A realtime process is spamming the event manager!");
			}
		}
	}

	// swap active queues and clear the new queue after the swap
	int queueToProcess = m_activeQueue;
	m_activeQueue = (m_activeQueue + 1) % EVENTMANAGER_NUM_QUEUES;
	m_queues[m_activeQueue].clear();

	GEN_LOG("EventLoop", "Processing Event Queue " + std::to_string(queueToProcess) + "; " + std::to_string((unsigned long)m_queues[queueToProcess].size()) + " events to process");

	// Process the queue
	while( !m_queues[queueToProcess].empty() ) {
		// pop the front of the queue
		IEventDataPtr pEvent = m_queues[queueToProcess].front();
		m_queues[queueToProcess].pop_front();
		GEN_LOG("EventLoop", "\t\tProcessing Event " + std::string(pEvent->getName()));

		const EventType& eventType = pEvent->getEventType();

		// find all the delegate functions registered for this event
		auto findIt = m_eventListeners.find(eventType);
		if( findIt != m_eventListeners.end() ) {
			const EventListenerList& eventListeners = findIt->second;
			GEN_LOG("EventLoop", "\t\tFound " + std::to_string((unsigned long)eventListeners.size()) + " delegates");

			// call each listener
			for( auto it = eventListeners.begin(); it != eventListeners.end(); ++it ) {
				EventListenerDelegate listener = (*it);
				GEN_LOG("EventLoop", "\t\tSending event " + std::string(pEvent->getName()) + " to delegate");
				listener(pEvent);
			}
		}

		// check to see if time ran out
		currMs = GetTickCount();
		if( maxMillis != IEventManager::kINFINITE && currMs >= maxMs ) {
			GEN_LOG("EventLoop", "Aborting event processing; time ran out");
			break;
		}
	}

	// If we couldn't process all of the events, push the remaining events to the new active queue.
	// Note: To preserve sequencing, go back-to-front, inserting them at the head of the active queue
	bool queueFlushed = (m_queues[queueToProcess].empty());
	if( !queueFlushed ) {
		while( !m_queues[queueToProcess].empty() ) {
			IEventDataPtr pEvent = m_queues[queueToProcess].back();
			m_queues[queueToProcess].pop_back();
			m_queues[m_activeQueue].push_front(pEvent);
		}
	}

	return queueFlushed;
}//EventManager::update

}
