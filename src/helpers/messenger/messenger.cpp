#include "messenger.h"

Messenger::Messenger(msg_severity_t severity){
	setSeverity(severity);
}

Messenger::~Messenger(){
}

void Messenger::setSeverity(msg_severity_t severity){
	msgLevel = severity;
}


void Messenger::message(msg_severity_t severity, const char *msg){
	int pid = getpid();
	if ( severity >= msgLevel ) {
		switch(severity){
			case MSG_DEBUG3:
				printf("%s(%05d)DEB3: %s%s\n", ANSI_COLOR_CYAN(), pid,  msg, ANSI_COLOR_RESET());
				break;
			case MSG_DEBUG2:
				printf("%s(%05d)DEB2: %s%s\n", ANSI_COLOR_GREEN(), pid, msg, ANSI_COLOR_RESET());
				break;
			case MSG_DEBUG:
				printf("%s(%05d)DEBG: %s%s\n", ANSI_COLOR_GREEN(), pid, msg, ANSI_COLOR_RESET());
				break;
			case MSG_INFO:
				printf("%s(%05d)INFO: %s%s\n", ANSI_COLOR_YELLOW(), pid, msg, ANSI_COLOR_RESET());
				break;
			case MSG_NOTICE:
				printf("%s(%05d)NTCE: %s%s\n", ANSI_COLOR_BLUE(), pid, msg, ANSI_COLOR_RESET());
				break;
			case MSG_ERROR:
				printf("%s(%05d)ERRR: %s%s\n", ANSI_COLOR_RED(), pid, msg, ANSI_COLOR_RESET());
				break;
			case MSG_CRITICAL:
				printf("%s(%05d)CRIT: %s%s\n", ANSI_COLOR_MAGENTA(), pid, msg, ANSI_COLOR_RESET());
				break;
			default:
				break;
		}
	}
}


void Messenger::debug3(const char *msg){
	message(MSG_DEBUG3, msg);
}

void Messenger::debug2(const char *msg){
	message(MSG_DEBUG2, msg);
}

void Messenger::debug(const char *msg){
	message(MSG_DEBUG, msg);
}

void Messenger::info(const char *msg){
	message(MSG_INFO, msg);
}

void Messenger::notice(const char *msg){
	message(MSG_NOTICE, msg);
}

void Messenger::error(const char *msg){
	message(MSG_ERROR, msg);
}

void Messenger::critical(const char *msg){
	message(MSG_CRITICAL, msg);
}


void Messenger::debug3(const string msg){
	message(MSG_DEBUG3, msg.c_str());
}

void Messenger::debug2(const string msg){
	message(MSG_DEBUG2, msg.c_str());
}

void Messenger::debug(const string msg){
	message(MSG_DEBUG, msg.c_str());
}

void Messenger::info(const string msg){
	message(MSG_INFO, msg.c_str());
}

void Messenger::notice(const string msg){
	message(MSG_NOTICE, msg.c_str());
}

void Messenger::error(const string msg){
	message(MSG_ERROR, msg.c_str());
}

void Messenger::critical(const string msg){
	message(MSG_CRITICAL, msg.c_str());
}
