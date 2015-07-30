#ifndef EVENT_MANAGER_IMP_H
#define EVENT_MANAGER_IMP_H

#include <list>
#include <map>

#include "EventManager.h"

namespace genesis {

const unsigned int EVENTMANAGER_NUM_QUEUES = 2;

class EventManager : public IEventManager {
protected:
	typedef std::list<EventListenerDelegate> EventListenerList;
	typedef std::map<EventType, EventListenerList> EventListenerMap;
	typedef std::list<IEventDataPtr> EventQueue;

	EventListenerMap		m_eventListeners;
	EventQueue				m_queues[EVENTMANAGER_NUM_QUEUES];
	int						m_activeQueue;
	ThreadSafeEventQueue	m_realtimeEventQueue;

public:
	explicit EventManager( const std::string name, bool global );
	virtual ~EventManager();

	virtual bool addListener( const EventListenerDelegate& eventDelegate, const EventType& type );
	virtual bool removeListener( const EventListenerDelegate& eventDelegate, const EventType& type );

	virtual bool instantEvent( const IEventDataPtr& event ) const;
	virtual bool queueEvent( const IEventDataPtr& event );
	virtual bool queueEventThreadSafe( const IEventDataPtr& event );
	virtual bool abortEvent( const EventType& type, bool allOfType = false );

	virtual bool update( unsigned long maxMillis = kINFINITE );
};

}

#endif /* EVENT_MANAGER_IMP_H */
