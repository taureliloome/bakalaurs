#ifndef _MESSENGER_H_
#define _MESSENGER_H_

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
using namespace std;

typedef enum msg_severity{
	MSG_NONE,
	MSG_DEBUG3,
	MSG_DEBUG2,
	MSG_DEBUG,
	MSG_INFO,
	MSG_NOTICE,
	MSG_ERROR,
	MSG_CRITICAL,
	MSG_MAX,
}msg_severity_t;

class MsgColors{
public:
    const char *ANSI_COLOR_CYAN(){
    	return "\x1b[36m";   //debug3
    }
    const char *ANSI_COLOR_GREEN(){
    	return "\x1b[32m";   //debug3
    }
    const char *ANSI_COLOR_YELLOW(){
    	return "\x1b[33m";   //debug3
    }
    const char *ANSI_COLOR_BLUE(){
    	return "\x1b[34m";   //debug3
    }
    const char *ANSI_COLOR_RED(){
    	return "\x1b[31m";   //debug3
    }
    const char *ANSI_COLOR_MAGENTA(){
    	return "\x1b[35m";   //debug3
    }
    const char *ANSI_COLOR_RESET(){
    	return "\x1b[0m";   //debug3
    }
};

class Messenger : MsgColors{
private:
    FILE *output;
	msg_severity_t msgLevel;
public:
    char msgBuffer[512];
	Messenger(msg_severity_t severity);
	~Messenger();

	void setSeverity(msg_severity_t severity);
	void setOutputType(FILE *type);

	void debug3(const char *msg);
	void debug2(const char *msg);
	void debug(const char *msg);
	void info(const char *msg);
	void notice(const char *msg);
	void error(const char *msg);
	void critical(const char *msg);

	void debug3(const string msg);
	void debug2(const string msg);
	void debug(const string msg);
	void info(const string msg);
	void notice(const string msg);
	void error(const string msg);
	void critical(const string msg);
private:
	void message(msg_severity_t severity, const char *msg);
};

#endif /* _MESSENGER_H_*/
