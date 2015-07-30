#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <memory>
#include <strstream>
#include <tbb/concurrent_queue.h>

#include "FastDelegate.h"

namespace genesis {

class IEventData;
typedef unsigned long EventType;
typedef std::shared_ptr<IEventData> IEventDataPtr;
typedef fastdelegate::FastDelegate1<IEventDataPtr> EventListenerDelegate;
typedef tbb::concurrent_queue<IEventDataPtr> ThreadSafeEventQueue;

// TODO can i make this work?
// Macro for event registration
//extern GenericObjectFactory<IEventData, EventType> g_eventFactory;
//#define REGISTER_EVENT(eventClass) g_eventFactory.Register<eventClass>(eventClass::sk_EventType)
//#define CREATE_EVENT(eventType) g_eventFactory.Create(eventType)

class IEventData {
public:
	virtual ~IEventData() {}

	virtual const EventType& getEventType() const = 0;
	virtual float getTimeStamp() const = 0;
	virtual IEventDataPtr copy() const = 0;
	virtual const std::string getName() const = 0;

	//for network input/output
	virtual void serialize( std::ostrstream& out ) const = 0;
	virtual void deserialize( std::istrstream& in ) = 0;
};

class BaseEventData : public IEventData {
protected:
	const float		m_TimeStamp;

public:
	explicit BaseEventData( const float timeStamp = 0.0f ) : m_TimeStamp(timeStamp) {}
	virtual ~BaseEventData() {}

	virtual const EventType& getEventType() const = 0;
	virtual float getTimeStamp() const { return m_TimeStamp; }

	virtual void serialize( std::ostrstream& out ) const {}
	virtual void deserialize( std::istrstream& in ) {}
};

class IEventManager {
public:
	enum eConstants { kINFINITE = 0xffffffff };

	explicit IEventManager( const std::string name, bool global );
	virtual ~IEventManager();

	virtual bool addListener( const EventListenerDelegate& eventDelegate, const EventType& type ) = 0;
	virtual bool removeListener( const EventListenerDelegate& eventDelegate, const EventType& type ) = 0;

	virtual bool instantEvent( const IEventDataPtr& event ) = 0;
	virtual bool queueEvent( const IEventDataPtr& event ) = 0;
	virtual bool queueEventThreadSafe( const IEventDataPtr& event ) = 0;
	virtual bool abortEvent( const EventType& type, bool allOfType = false ) = 0;

	virtual bool update( unsigned long maxMillis = kINFINITE ) = 0;

	static IEventManager* Get();
};

}

#endif /* EVENT_MANAGER_H */
