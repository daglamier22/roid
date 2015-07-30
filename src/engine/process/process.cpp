#include "process.h"

namespace genesis {

Process::Process() {
	m_state = UNINITIALIZED;
}//Process::Process

Process::~Process() {
	if( m_pChild ) {
		m_pChild->onAbort();
	}
}//Process::~Process

StrongProcessPtr Process::removeChild() {
	if( m_pChild ) {
		StrongProcessPtr pChild = m_pChild;  // this keeps the child from getting destroyed when we clear it
		m_pChild.reset();
		return pChild;
	}

	return StrongProcessPtr();
}//Process::removeChild

}
