#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <list>

#include "process.h"

namespace genesis {

class ProcessManager {
	typedef std::list<StrongProcessPtr> ProcessList;

	ProcessList		m_processList;

public:
	~ProcessManager();

	unsigned int updateProcesses( unsigned long deltaMs );  // updates all attached processes
	WeakProcessPtr attachProcess( StrongProcessPtr pProcess );  // attaches a process to the process mgr
	void abortAllProcesses( bool immediate );

	unsigned int getProcessCount() const { return m_processList.size(); }

private:
	void clearAllProcesses();  // should only be called by the destructor
};

}

#endif /* PROCESS_MANAGER_H */
