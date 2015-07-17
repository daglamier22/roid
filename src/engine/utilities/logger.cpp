#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "utilities/tinyxml.h"
#include "logger.h"

namespace genesis {

static Logger* s_pLogger = 0;

ErrorMessenger::ErrorMessenger() {
	s_pLogger->addErrorMessenger(this);
	m_enabled = true;
}//ErrorMessenger::ErrorMessenger

void ErrorMessenger::show( const std::string& errorMessage, bool isFatal, const char* funcName, const char* sourceFile, unsigned int lineNum ) {
	if( m_enabled ) {
		if( s_pLogger->error(errorMessage, isFatal, funcName, sourceFile, lineNum) == Logger::LOGGER_ERROR_IGNORE )
			m_enabled = false;
	}
}//ErrorMessenger::show

Logger::Logger()
{
	// set up the default log tags
	setDisplayFlagsInternal("ERROR", ERRORFLAG_DEFAULT);
	setDisplayFlagsInternal("WARNING", WARNINGFLAG_DEFAULT);
	setDisplayFlagsInternal("INFO", LOGFLAG_DEFAULT);
}//Logger::Logger

Logger::~Logger() {
	// TODO look into utilizing thread safety
	//m_messengerCriticalSection.Lock();
	for( auto it = m_errorMessengers.begin(); it != m_errorMessengers.end(); ++it ) {
		ErrorMessenger* pMessenger = (*it);
		delete pMessenger;
	}
	//m_errorMessengers.clear();
	//m_messengerCriticalSection.Unlock();
}//Logger::~Logger

void Logger::init( const char* loggingConfigFilename ) {
	if( !s_pLogger ) {
		s_pLogger = new Logger;
		s_pLogger->init(loggingConfigFilename);
	}

}//Logger::init

void Logger::log( const std::string& tag, const std::string& message, const char* funcName, const char* sourceFile, unsigned int lineNum ) {
	GEN_ASSERT(s_pLogger);
	s_pLogger->logInternal(tag, message, funcName, sourceFile, lineNum);
}//Logger::log

void Logger::setDisplayFlags( const std::string& tag, unsigned char flags ) {
	GEN_ASSERT(s_pLogger);
	s_pLogger->setDisplayFlagsInternal(tag, flags);
}//Logger::setDisplayFlags

void Logger::destroy() {
	delete s_pLogger;
	s_pLogger = 0;
}//Logger::destroy

void Logger::initInternal( const char* loggingConfigFilename ) {
	if( loggingConfigFilename ) {
		TiXmlDocument loggingConfigFile(loggingConfigFilename);
		if( loggingConfigFile.LoadFile() ) {
			TiXmlElement* pRoot = loggingConfigFile.RootElement();
			if( !pRoot )
				return;

			// Loop through each child element and load the component
			for( TiXmlElement* pNode = pRoot->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement() ) {
				unsigned char flags = 0;
				std::string tag(pNode->Attribute("tag"));

				int debugger = 0;
				pNode->Attribute("debugger", &debugger);
				if( debugger )
					flags |= LOGFLAG_WRITE_TO_DEBUGGER;

				int logfile = 0;
				pNode->Attribute("file", &logfile);
				if( logfile )
					flags |= LOGFLAG_WRITE_TO_LOG_FILE;

				setDisplayFlags(tag, flags);
			}
		}
	}
	else {
		setDisplayFlags("error",LOGFLAG_DEFAULT);
	}
}//Logger::initInternal

void Logger::logInternal( const std::string& tag, const std::string& message, const char* funcName, const char* sourceFile, unsigned int lineNum ) {
	// TODO look into utilizing thread safety
	//m_tagCriticalSection.Lock();
	Tags::iterator findIt = m_tags.find(tag);
	if( findIt != m_tags.end() ) {
		//m_tagCriticalSection.Unlock();

		std::string buffer;
		getOutputBuffer(buffer, tag, message, funcName, sourceFile, lineNum);
		outputFinalBufferToLogs(buffer, findIt->second);
	}
	else {
		// Critical section is exited in the if statement above, so we need to exit it here if that didn't
		// get executed.
		//m_tagCriticalSection.Unlock();
	}
}//Logger::logInternal

void Logger::setDisplayFlagsInternal( const std::string& tag, unsigned char flags ) {
	// TODO look into utilizing thread safety
	//m_tagCriticalSection.Lock();
	if( flags != 0 ) {
		Tags::iterator findIt = m_tags.find(tag);
		if( findIt == m_tags.end() )
			m_tags.insert(std::make_pair(tag, flags));
		else
			findIt->second = flags;
	}
	else {
		m_tags.erase(tag);
	}
	//m_tagCriticalSection.Unlock();
}//Logger::setDisplayFlagsInternal

void Logger::addErrorMessenger( ErrorMessenger* pMessenger ) {
	// TODO look into utilizing thread safety
	//m_messengerCriticalSection.Lock();
	m_errorMessengers.push_back(pMessenger);
	//m_messengerCriticalSection.Unlock();
}//Logger::addErrorMessenger

Logger::ErrorDialogResult Logger::error( const std::string& errorMessage, bool isFatal, const char* funcName, const char* sourceFile, unsigned int lineNum ) {
	std::string tag = ((isFatal) ? ("FATAL") : ("ERROR"));

	// buffer for our final output string
	std::string buffer;
	getOutputBuffer(buffer, tag, errorMessage, funcName, sourceFile, lineNum);

	// write the final buffer to all the various logs
	// TODO look into utilizing thread safety
	//m_tagCriticalSection.Lock();
	Tags::iterator findIt = m_tags.find(tag);
	if( findIt != m_tags.end() )
		outputFinalBufferToLogs(buffer, findIt->second);
	//m_tagCriticalSection.Unlock();

	//TODO need a linux version of this, QT perhaps?
	/*// show the dialog box
	int result = ::MessageBoxA(NULL, buffer.c_str(), tag.c_str(), MB_ABORTRETRYIGNORE|MB_ICONERROR|MB_DEFBUTTON3);

	// act upon the choice
	switch( result ) {
	case IDIGNORE :	return Logger::LOGGER_ERROR_IGNORE;
	case IDABORT :	//__debugbreak();		// assembly language instruction to break into the debugger
					return Logger::LOGGER_ERROR_RETRY;
	case IDRETRY :	return Logger::LOGGER_ERROR_RETRY;
	default :		return Logger::LOGGER_ERROR_RETRY;
	}*/
	return LOGGER_ERROR_ABORT;
}//Logger::error

void Logger::outputFinalBufferToLogs( const std::string& finalBuffer, unsigned char flags ) {
	// Write the log to each display based on the display flags
	if( (flags & LOGFLAG_WRITE_TO_LOG_FILE) > 0 )  // log file
		writeToLogFile(finalBuffer);
	if( (flags & LOGFLAG_WRITE_TO_DEBUGGER) > 0 )  // debugger output window
		//outputDebugStringA(finalBuffer.c_str());
		std::cerr << finalBuffer;
}//Logger::outputFinalBufferToLogs

void Logger::writeToLogFile( const std::string& data ) {
	FILE* pLogFile = 0;
	pLogFile = fopen(ERRORLOG_FILENAME, "a+");
	if( !pLogFile )
		return;  // can't write to the log file for some reason

	fprintf(pLogFile, data.c_str());

	fclose(pLogFile);
}//Logger::writeToLogFile

void Logger::getOutputBuffer( std::string& outOutputBuffer, const std::string& tag, const std::string& message, const char* funcName, const char* sourceFile, unsigned int lineNum ) {
	if( !tag.empty() )
		outOutputBuffer = "[" + tag + "] " + message;
	else
		outOutputBuffer = message;

	if( funcName != 0 ) {
		outOutputBuffer += "\nFunction: ";
		outOutputBuffer += funcName;
	}
	if( sourceFile != 0 ) {
		outOutputBuffer += "\n";
		outOutputBuffer += sourceFile;
	}
	if( lineNum != 0 ) {
		outOutputBuffer += "\nLine: ";
		char lineNumBuffer[11];
		memset(lineNumBuffer, 0, sizeof(char));
		snprintf(lineNumBuffer, sizeof(char)*10, "%d", lineNum);
		outOutputBuffer += lineNumBuffer;
	}

	outOutputBuffer += "\n";
}//Logger::getOutputBuffer

}
