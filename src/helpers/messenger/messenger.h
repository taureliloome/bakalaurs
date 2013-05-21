#ifndef _MESSENGER_H_
#define _MESSENGER_H_

#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
using namespace std;

#include "world_types.h"

void _info(const char *msg);
void _notice(const char *msg);
void _error(const char *msg);
void _critical(const char *msg);

#define debug3(STR,...) \
do {\
    memset(msgBuffer, 0, 512); \
    sprintf(msgBuffer, STR "(%s:%d %s)", ##__VA_ARGS__, __FILE__, __LINE__, __FUNCTION__ ); \
    _debug3(msgBuffer); \
} while(0)

#define debug2(STR,...) \
do {\
    memset(msgBuffer, 0, 512); \
    sprintf(msgBuffer, STR "(%s:%d %s)", ##__VA_ARGS__, __FILE__, __LINE__, __FUNCTION__ ); \
    _debug2(msgBuffer); \
} while(0)

#define debug(STR,...) \
do {\
    memset(msgBuffer, 0, 512); \
    sprintf(msgBuffer, STR "(%s:%d %s)", ##__VA_ARGS__, __FILE__, __LINE__, __FUNCTION__ ); \
    _debug(msgBuffer); \
} while(0)

#define info(STR,...) \
do {\
    memset(msgBuffer, 0, 512); \
    sprintf(msgBuffer, STR "(%s:%d %s)", ##__VA_ARGS__, __FILE__, __LINE__, __FUNCTION__ ); \
    _info(msgBuffer); \
} while(0)

#define notice(STR,...) \
do {\
    memset(msgBuffer, 0, 512); \
    sprintf(msgBuffer, STR "(%s:%d %s)", ##__VA_ARGS__, __FILE__, __LINE__, __FUNCTION__ ); \
    _notice(msgBuffer); \
} while(0)

#define error(STR,...) \
do {\
    memset(msgBuffer, 0, 512); \
    sprintf(msgBuffer, STR "(%s:%d %s)", ##__VA_ARGS__, __FILE__, __LINE__, __FUNCTION__ ); \
    _error(msgBuffer); \
} while(0)

#define critical(STR,...) \
do {\
    memset(msgBuffer, 0, 512); \
    sprintf(msgBuffer, STR "(%s:%d %s)", ##__VA_ARGS__, __FILE__, __LINE__, __FUNCTION__ ); \
    _critical(msgBuffer); \
}while(0)

typedef enum msg_severity {
    MSG_NONE,
    MSG_DEBUG3,
    MSG_DEBUG2,
    MSG_DEBUG,
    MSG_INFO,
    MSG_NOTICE,
    MSG_ERROR,
    MSG_CRITICAL,
    MSG_MAX,
} msg_severity_t;

class MsgColors {
public:
    const char *ANSI_COLOR_CYAN() {
        return "\x1b[36m";   //debug3
    }
    const char *ANSI_COLOR_GREEN() {
        return "\x1b[32m";   //debug3
    }
    const char *ANSI_COLOR_YELLOW() {
        return "\x1b[33m";   //debug3
    }
    const char *ANSI_COLOR_BLUE() {
        return "\x1b[34m";   //debug3
    }
    const char *ANSI_COLOR_RED() {
        return "\x1b[31m";   //debug3
    }
    const char *ANSI_COLOR_MAGENTA() {
        return "\x1b[35m";   //debug3
    }
    const char *ANSI_COLOR_RESET() {
        return "\x1b[0m";   //debug3
    }
};

class Messenger: MsgColors {
private:
    FILE *output;
    msg_severity_t msgLevel;
    char unit[16];
public:
    char msgBuffer[512];
    Messenger(msg_severity_t severity, const char *setName = "UNNAMED");
    ~Messenger();

    void setSeverity(msg_severity_t severity);
    void setOutputType(FILE *type);

    void printNucleotide(nucleotide_t *nucleotide);

    void _debug3(const char *msg);
    void _debug2(const char *msg);
    void _debug(const char *msg);
    void _info(const char *msg);
    void _notice(const char *msg);
    void _error(const char *msg);
    void _critical(const char *msg);

    void _debug3(const string msg);
    void _debug2(const string msg);
    void _debug(const string msg);
    void _info(const string msg);
    void _notice(const string msg);
    void _error(const string msg);
    void _critical(const string msg);
private:
    void message(msg_severity_t severity, const char *msg);
};

#endif /* _MESSENGER_H_*/
