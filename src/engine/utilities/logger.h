#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <map>
#include <list>

namespace genesis {

// the log filename
static const char* ERRORLOG_FILENAME = "error.log";

// display flags
const unsigned char LOGFLAG_WRITE_TO_LOG_FILE =		1 << 0;
const unsigned char LOGFLAG_WRITE_TO_DEBUGGER =		1 << 1;

// default display flags
#ifdef DEBUG
	const unsigned char ERRORFLAG_DEFAULT =			LOGFLAG_WRITE_TO_DEBUGGER;
	const unsigned char WARNINGFLAG_DEFAULT =		LOGFLAG_WRITE_TO_DEBUGGER;
	const unsigned char LOGFLAG_DEFAULT =			LOGFLAG_WRITE_TO_DEBUGGER;
#elif NDEBUG
	const unsigned char ERRORFLAG_DEFAULT =			LOGFLAG_WRITE_TO_LOG_FILE;
	const unsigned char WARNINGFLAG_DEFAULT =		LOGFLAG_WRITE_TO_LOG_FILE;
	const unsigned char LOGFLAG_DEFAULT =			LOGFLAG_WRITE_TO_LOG_FILE;
#else
	const unsigned char ERRORFLAG_DEFAULT =			(LOGFLAG_WRITE_TO_DEBUGGER | LOGFLAG_WRITE_TO_LOG_FILE);
	const unsigned char WARNINGFLAG_DEFAULT =		(LOGFLAG_WRITE_TO_DEBUGGER | LOGFLAG_WRITE_TO_LOG_FILE);
	const unsigned char LOGFLAG_DEFAULT =			(LOGFLAG_WRITE_TO_DEBUGGER | LOGFLAG_WRITE_TO_LOG_FILE);
#endif

class ErrorMessenger {
protected:
	bool m_enabled;

public:
	ErrorMessenger();
	void show( const std::string& errorMessage, bool isFatal, const char* funcName, const char* sourceFile, unsigned int lineNum );
};

class Logger
{
public:
	enum ErrorDialogResult {
		LOGGER_ERROR_ABORT,
		LOGGER_ERROR_RETRY,
		LOGGER_ERROR_IGNORE
	};

protected:
	typedef std::map<std::string, unsigned char> Tags;
	typedef std::list<ErrorMessenger*> ErrorMessengerList;

	Tags m_tags;
	ErrorMessengerList m_errorMessengers;

	// TODO look into utilizing thread safety
	// thread safety
	//CriticalSection m_tagCriticalSection;
	//CriticalSection m_messengerCriticalSection;

public:
	// construction
	Logger();
	~Logger();

	static void init( const char* loggingConfigFilename );
	static void log(const std::string& tag, const std::string& message, const char* funcName, const char* sourceFile, unsigned int lineNum );
	static void setDisplayFlags( const std::string& tag, unsigned char flags );
	static void destroy();

	void initInternal( const char* loggingConfigFilename );
	void logInternal(const std::string& tag, const std::string& message, const char* funcName, const char* sourceFile, unsigned int lineNum );
	void setDisplayFlagsInternal( const std::string& tag, unsigned char flags );

	// error messengers
	void addErrorMessenger( ErrorMessenger* pMessenger );
	ErrorDialogResult error( const std::string& errorMessage, bool isFatal, const char* funcName, const char* sourceFile, unsigned int lineNum );

private:
	// log helpers
	void outputFinalBufferToLogs( const std::string& finalBuffer, unsigned char flags );
	void writeToLogFile( const std::string& data );
	void getOutputBuffer( std::string& outOutputBuffer, const std::string& tag, const std::string& message, const char* funcName, const char* sourceFile, unsigned int lineNum );
};

//---------------------------------------------------------------------------------------------------------------------
// Debug macros
//---------------------------------------------------------------------------------------------------------------------

// Fatal Errors are fatal and are always presented to the user.
#define GEN_FATAL(str) \
		do \
		{ \
				static ErrorMessenger* pErrorMessenger = new ErrorMessenger; \
				std::string s((str)); \
				pErrorMessenger->show(s, true, __FUNCTION__, __FILE__, __LINE__); \
		} \
		while (0)\

#ifndef NDEBUG

// Errors are bad and potentially fatal.  They are presented as a dialog with Abort, Retry, and Ignore.  Abort will
// break into the debugger, retry will continue the game, and ignore will continue the game and ignore every subsequent
// call to this specific error.  They are ignored completely in release mode.
#define GEN_ERROR(str) \
		do \
		{ \
				static ErrorMessenger* pErrorMessenger = new ErrorMessenger; \
				std::string s((str)); \
				pErrorMessenger->show(s, false, __FUNCTION__, __FILE__, __LINE__); \
		} \
		while (0)\

// Warnings are recoverable.  They are just logs with the "WARNING" tag that displays calling information.  The flags
// are initially set to WARNINGFLAG_DEFAULT (defined in debugger.cpp), but they can be overridden normally.
#define GEN_WARNING(str) \
		do \
		{ \
				std::string s((str)); \
				Logger::log("WARNING", s, __FUNCTION__, __FILE__, __LINE__); \
		}\
		while (0)\

// This is just a convenient macro for logging if you don't feel like dealing with tags.  It calls Log() with a tag
// of "INFO".  The flags are initially set to LOGFLAG_DEFAULT (defined in debugger.cpp), but they can be overridden
// normally.
#define GEN_INFO(str) \
		do \
		{ \
				std::string s((str)); \
				Logger::log("INFO", s, NULL, NULL, 0); \
		} \
		while (0) \

// This macro is used for logging and should be the preferred method of "printf debugging".  You can use any tag
// string you want, just make sure to enabled the ones you want somewhere in your initialization.
#define GEN_LOG(tag, str) \
		do \
		{ \
				std::string s((str)); \
				Logger::log(tag, s, NULL, NULL, 0); \
		} \
		while (0) \

// This macro replaces GCC_ASSERT().
#define GEN_ASSERT(expr) \
		do \
		{ \
				if (!(expr)) \
				{ \
						static ErrorMessenger* pErrorMessenger = new ErrorMessenger; \
						pErrorMessenger->show(#expr, false, __FUNCTION__, __FILE__, __LINE__); \
				} \
		} \
		while (0) \

#else  // NDEBUG is defined

// These are the release mode definitions for the macros above.  They are all defined in such a way as to be
// ignored completely by the compiler.
#define GEN_ERROR(str) do { (void)sizeof(str); } while(0)
#define GEN_WARNING(str) do { (void)sizeof(str); } while(0)
#define GEN_INFO(str) do { (void)sizeof(str); } while(0)
#define GEN_LOG(tag, str) do { (void)sizeof(tag); (void)sizeof(str); } while(0)
#define GEN_ASSERT(expr) do { (void)sizeof(expr); } while(0)

#endif  // !defined NDEBUG

}

#endif // LOGGER_H
