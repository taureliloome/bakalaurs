#include "organism.h"

Organism::Organism(){
	 //TODO: change this to be the id code that is assigned during creation
	outputCode = "1";
	printf("A new organism has been born\n");
}

Organism::~Organism(){
	printf("Organism has died\n");
}

string Organism::createCompilationString(const string input){
	string output = "g++ ";
	output.append(input);
	output.append(" -o ./results/");
	output.append(outputCode);
	output.append(".bin");
	return output;
}

string Organism::createCompilationString(const char *input){
	string output = "g++ ";
	output.append(input);
	output.append(" -o ./results/");
	output.append(outputCode);
	output.append(".bin");
	return output;
}

void Organism::code(const char *str){
	string compile = createCompilationString(str);
	system("pwd");
	system(compile.c_str());
}
