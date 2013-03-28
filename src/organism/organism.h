#ifndef _ORGANISM_H_
#define _ORGANISM_H_

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "messenger.h"
using namespace std;

class Organism : Messenger{
	//variables
private:
	string outputCode;
	bool m_compile;
	pid_t childPid;

	//methods
public:
	Organism();
	Organism(msg_severity_t severity);
	~Organism();
	void code(const char *str);
private:
	string createCompilationString(const char *input);
	string createCompilationString(const string input);
	string getCompiledBinaryPath();
};

#endif /* _ORGANISM_H_ */
