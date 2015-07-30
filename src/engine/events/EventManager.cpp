#include "EventManager.h"
#include "utilities/logger.h"

namespace genesis {

static IEventManager* g_pEventManager = NULL;
//GenericObjectFactory<IEventData, EventType> g_eventFactory;

IEventManager::IEventManager( const std::string name, bool global ) {
	if( global ) {
		if( g_pEventManager ) {
			GEN_ERROR("Attempting to create two global event managers! The old one will be destroyed and overwritten with this one.");
			delete g_pEventManager;
		}

		g_pEventManager = this;
	}
}//IEventManager::IEventManager

IEventManager::~IEventManager() {
	if( g_pEventManager == this )
		g_pEventManager = NULL;
}//IEventManager::~IEventManager

IEventManager* IEventManager::Get() {
	GEN_ASSERT(g_pEventManager);
	return g_pEventManager;
}//IEventManager::Get

}
