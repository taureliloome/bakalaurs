#include "organism.h"

Organism::Organism(): Messenger(MSG_INFO){
	 //TODO: change this to be the id code that is assigned during creation
	childPid = 0;
	outputCode = "1";
	m_compile = true;
	info("A new organism has been born");
}
Organism::Organism(msg_severity_t severity) : Messenger(severity){
	 //TODO: change this to be the id code that is assigned during creation
	childPid = 0;
	outputCode = "1";
	m_compile = true;
	info("A new organism has been born");
}
Organism::~Organism(){
	info("Organism has died");
}

string Organism::createCompilationString(const string input){
	string output = "g++ ";
	output.append(input);
	output.append(" -o ./results/");
	output.append(outputCode);
	output.append(".bin");
	debug3(output);
	return output;
}

string Organism::createCompilationString(const char *input){
	string output = "g++ ";
	output.append(input);
	output.append(" -o ./results/");
	output.append(outputCode);
	output.append(".bin");
	debug3(output);
	return output;
}

string Organism::getCompiledBinaryPath(){
	string output = "./results/";
	output.append(outputCode);
	output.append(".bin");
	return output;
}

void Organism::code(const char *str){
	string compileStr = createCompilationString(str);
	int compileResult = 0;
	if (m_compile) {
		debug3("compile true");
		compileResult = system(compileStr.c_str());
		if (!compileResult){
			debug3("compile result");
			childPid = fork();
			if ( childPid >= 0 ) {
				debug3("is parent/child");
				if (childPid == 0) {
					execve(getCompiledBinaryPath().c_str(), NULL, NULL);
				} else {
					info("I'm the parent");
					waitpid(-1, NULL, 0);
				}
			} else {
				error("failed to initiate fork");
			}
		} else {
			error("unable to compile requested file");
		}
	}
	else {
		debug3("compile false");
		info(compileStr);
	}
}
