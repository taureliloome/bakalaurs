/**
 *  @author    Jānis Knets
 *  @version   0.1
 *  @date      2013-03-27
 *  @copyright GNU Public License.
 *
 *  @brief     Organism class.
 *  @details   This class is used to create and launch a new instance of organism
 *              that will attempt to solve the given problem.
 * */

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
