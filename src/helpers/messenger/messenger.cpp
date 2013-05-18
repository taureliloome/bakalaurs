#include "messenger.h"

Messenger::Messenger(msg_severity_t severity = MSG_DEBUG3, const char *setName = "UNNAMED"){
    if ( strlen(setName) < 16 ){
        strcpy(name, setName);
    } else {
        error("Max length of messenger name is 15 characters");
        strcpy(name, "UNNAMED");
    }
    output = NULL;
	setSeverity(severity);
    setOutputType(stderr);
}

Messenger::Messenger(msg_severity_t severity = MSG_DEBUG3){
    strcpy(name, "UNNAMED");
    output = NULL;
    setSeverity(severity);
    setOutputType(stderr);
}

Messenger::~Messenger(){
}

void Messenger::setSeverity(msg_severity_t severity){
	msgLevel = severity;
}

void Messenger::setOutputType(FILE *type) {
    output = type;
    debug("Output type changed");
}


void Messenger::message(msg_severity_t severity, const char *msg){
	int pid = getpid();
	if ( severity >= msgLevel ) {
		switch(severity){
			case MSG_DEBUG3:
				fprintf(output, "%s<%s> (%05d) DEB3: %s%s\n", ANSI_COLOR_CYAN(), name, pid,  msg, ANSI_COLOR_RESET());
				break;
			case MSG_DEBUG2:
				fprintf(output, "%s<%s> (%05d) DEB2: %s%s\n", ANSI_COLOR_GREEN(), name, pid, msg, ANSI_COLOR_RESET());
				break;
			case MSG_DEBUG:
				fprintf(output, "%s<%s> (%05d) DEBG: %s%s\n", ANSI_COLOR_GREEN(), name, pid, msg, ANSI_COLOR_RESET());
				break;
			case MSG_INFO:
				fprintf(output, "%s<%s> (%05d) INFO: %s%s\n", ANSI_COLOR_YELLOW(), name, pid, msg, ANSI_COLOR_RESET());
				break;
			case MSG_NOTICE:
				fprintf(output, "%s<%s> (%05d) NTCE: %s%s\n", ANSI_COLOR_BLUE(), name, pid, msg, ANSI_COLOR_RESET());
				break;
			case MSG_ERROR:
				fprintf(output, "%s<%s> (%05d) ERRR: %s%s\n", ANSI_COLOR_RED(), name, pid, msg, ANSI_COLOR_RESET());
				break;
			case MSG_CRITICAL:
				fprintf(output, "%s<%s> (%05d) CRIT: %s%s\n", ANSI_COLOR_MAGENTA(), name, pid, msg, ANSI_COLOR_RESET());
				break;
			default:
				break;
		}
	}
}


void Messenger::_debug3(const char *msg){
	message(MSG_DEBUG3, msg);
}

void Messenger::_debug2(const char *msg){
	message(MSG_DEBUG2, msg);
}

void Messenger::_debug(const char *msg){
	message(MSG_DEBUG, msg);
}

void Messenger::_info(const char *msg){
	message(MSG_INFO, msg);
}

void Messenger::_notice(const char *msg){
	message(MSG_NOTICE, msg);
}

void Messenger::_error(const char *msg){
	message(MSG_ERROR, msg);
}

void Messenger::_critical(const char *msg){
	message(MSG_CRITICAL, msg);
}


void Messenger::_debug3(const string msg){
	message(MSG_DEBUG3, msg.c_str());
}

void Messenger::_debug2(const string msg){
	message(MSG_DEBUG2, msg.c_str());
}

void Messenger::_debug(const string msg){
	message(MSG_DEBUG, msg.c_str());
}

void Messenger::_info(const string msg){
	message(MSG_INFO, msg.c_str());
}

void Messenger::_notice(const string msg){
	message(MSG_NOTICE, msg.c_str());
}

void Messenger::_error(const string msg){
	message(MSG_ERROR, msg.c_str());
}

void Messenger::_critical(const string msg){
	message(MSG_CRITICAL, msg.c_str());
}
