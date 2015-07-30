#ifndef PROCESS_H
#define PROCESS_H

#include <memory>

#include "utilities/logger.h"

namespace genesis {

class Process;
typedef std::shared_ptr<Process> StrongProcessPtr;
typedef std::weak_ptr<Process> WeakProcessPtr;

//---------------------------------------------------------------------------------------------------------------------
// Process class
//
// Processes are ended by one of three methods: Success, Failure, or Aborted.
//              - Success means the process completed successfully.  If the process has a child, it will be attached to
//                the process mgr.
//              - Failure means the process started but failed in some way.  If the process has a child, it will be
//                aborted.
//              - Aborted processes are processes that are canceled while not submitted to the process mgr.  Depending
//                on the circumstances, they may or may not have gotten an OnInit() call.  For example, a process can
//                spawn another process and call AttachToParent() on itself.  If the new process fails, the child will
//                get an Abort() call on it, even though its status is RUNNING.
//---------------------------------------------------------------------------------------------------------------------
class Process
{
	friend class ProcessManager;

public:
	enum State {
		// Processes that are neither dead nor alive
		UNINITIALIZED = 0,  // created but not running
		REMOVED,  // removed from the process list but not destroyed; this can happen when a process that is already running is parented to another process

		// Living processes
		RUNNING,  // initialized and running
		PAUSED,  // initialized but paused

		// Dead processes
		SUCCEEDED,  // completed successfully
		FAILED,  // failed to complete
		ABORTED,  // aborted; may not have started
	};

private:
	State				m_state;  // the current state of the process
	StrongProcessPtr	m_pChild;  // the child process, if any

public:
	// construction
	Process();
	virtual ~Process();

protected:
	// interface; these functions should be overridden by the subclass as needed
	virtual void onInit() { m_state = RUNNING; }  // called during the first update; responsible for setting the initial state (typically RUNNING)
	virtual void onUpdate( unsigned long deltaMs ) = 0;  // called every frame
	virtual void onSuccess() { }  // called if the process succeeds (see below)
	virtual void onFail() { }  // called if the process fails (see below)
	virtual void onAbort() { }  // called if the process is aborted (see below)

public:
	// Functions for ending the process.
	inline void succeed();
	inline void fail();

	// pause
	inline void pause();
	inline void unPause();

	// accessors
	State getState() const { return m_state; }
	bool isAlive() const { return (m_state == RUNNING || m_state == PAUSED); }
	bool isDead() const { return (m_state == SUCCEEDED || m_state == FAILED || m_state == ABORTED); }
	bool isRemoved() const { return (m_state == REMOVED); }
	bool isPaused() const { return m_state == PAUSED; }

	// child functions
	inline void attachChild( StrongProcessPtr pChild );
	StrongProcessPtr removeChild();  // releases ownership of the child
	StrongProcessPtr peekChild() { return m_pChild; }  // doesn't release ownership of the child

private:
	void setState( State newState ) { m_state = newState; }
};


//---------------------------------------------------------------------------------------------------------------------
// Inline function definitions
//---------------------------------------------------------------------------------------------------------------------
inline void Process::succeed() {
	GEN_ASSERT(m_state == RUNNING || m_state == PAUSED);
	m_state = SUCCEEDED;
}//Process::succeed

inline void Process::fail() {
	GEN_ASSERT(m_state == RUNNING || m_state == PAUSED);
	m_state = FAILED;
}//Process::fail

inline void Process::attachChild( StrongProcessPtr pChild ) {
	if( m_pChild )
		m_pChild->attachChild(pChild);
	else
		m_pChild = pChild;
}//Process::attachChild

inline void Process::pause() {
	if( m_state == RUNNING )
		m_state = PAUSED;
	else
		GEN_WARNING("Attempting to pause a process that isn't running");
}//Process::pause

inline void Process::unPause() {
	if( m_state == PAUSED )
		m_state = RUNNING;
	else
		GEN_WARNING("Attempting to unpause a process that isn't paused");
}//Process::unPause

}

#endif /* EVENT_H */
