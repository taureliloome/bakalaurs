#ifndef _ORGANISM_H_
#define _ORGANISM_H_

#include <string>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

class Organism{
public:
	Organism();
	~Organism();
	void code(const char *str);
private:
	string createCompilationString(const char *input);
	string createCompilationString(const string input);
	string outputCode;
};

#endif /* _ORGANISM_H_ */
