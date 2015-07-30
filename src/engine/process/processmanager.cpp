#include "processmanager.h"

namespace genesis {

ProcessManager::~ProcessManager() {
	clearAllProcesses();
}//ProcessManager::~ProcessManager

//---------------------------------------------------------------------------------------------------------------------
// The process update tick.  Called every logic tick.  This function returns the number of process chains that
// succeeded in the upper 32 bits and the number of process chains that failed or were aborted in the lower 32 bits.
//---------------------------------------------------------------------------------------------------------------------
unsigned int ProcessManager::updateProcesses( unsigned long deltaMs ) {
	unsigned short int successCount = 0;
	unsigned short int failCount = 0;

	ProcessList::iterator it = m_processList.begin();
	while( it != m_processList.end() ) {
		StrongProcessPtr pCurrProcess = (*it);

		// save the iterator and increment the old one in case we need to remove this process from the list
		ProcessList::iterator thisIt = it;
		++it;

		// process is uninitialized, so initialize it
		if( pCurrProcess->getState() == Process::UNINITIALIZED )
			pCurrProcess->onInit();

		// give the process an update tick if it's running
		if( pCurrProcess->getState() == Process::RUNNING )
			pCurrProcess->onUpdate(deltaMs);

		// check to see if the process is dead
		if( pCurrProcess->isDead() ) {
			// run the appropriate exit function
			switch( pCurrProcess->getState() ) {
				case Process::SUCCEEDED :
				{
					pCurrProcess->onSuccess();
					StrongProcessPtr pChild = pCurrProcess->removeChild();
					if( pChild )
						attachProcess(pChild);
					else
						++successCount;  // only counts if the whole chain completed
					break;
				}

				case Process::FAILED :
				{
					pCurrProcess->onFail();
					++failCount;
					break;
				}

				case Process::ABORTED :
				{
					pCurrProcess->onAbort();
					++failCount;
					break;
				}

				default:
				{
					break;
				}
			}

			// remove the process and destroy it
			m_processList.erase(thisIt);
		}
	}

	return ((successCount << 16) | failCount);
}//ProcessManager::updateProcesses

WeakProcessPtr ProcessManager::attachProcess( StrongProcessPtr pProcess ) {
	m_processList.push_front(pProcess);

	return WeakProcessPtr(pProcess);
}//ProcessManager::attachProcess

void ProcessManager::clearAllProcesses() {
	m_processList.clear();
}//ProcessManager::clearAllProcesses

//---------------------------------------------------------------------------------------------------------------------
// Aborts all processes.  If immediate == true, it immediately calls each ones OnAbort() function and destroys all
// the processes.
//---------------------------------------------------------------------------------------------------------------------
void ProcessManager::abortAllProcesses( bool immediate ) {
	ProcessList::iterator it = m_processList.begin();
	while( it != m_processList.end() ) {
		ProcessList::iterator tempIt = it;
		++it;

		StrongProcessPtr pProcess = *tempIt;
		if( pProcess->isAlive() ) {
			pProcess->setState(Process::ABORTED);
			if( immediate ) {
				pProcess->onAbort();
				m_processList.erase(tempIt);
			}
		}
	}
}//ProcessManager::abortAllProcesses

}
