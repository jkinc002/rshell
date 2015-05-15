#include <queue>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sys/types.h>
#include <sys/wait.h>

#ifndef _MYIO_H_
#define _MYIO_H_

bool isExit(char *c){
	if(c[0] == 'e' && c[1] == 'x'
	&& c[2] == 'i' && c[3] == 't'
	&& c[4] == '\0') return true;
	return false;
}

char *command[64];
std::queue<char **> filters;
char *inputFile = NULL;
char *outputFile = NULL;
int leftCount = 0;
int rightCount = 0;
int pipeCount = 0;

void clear1(){
	command[0] = NULL;
	std::queue<char **> empty;
	std::swap(filters,empty);
	inputFile = NULL;
	outputFile = NULL;
	leftCount = 0;
	rightCount = 0;
	pipeCount = 0;
}

void print1(){
	std::cout << "Command: ";
	for(unsigned i=0;command[i] != NULL;++i){
		std::cout << command[i] << ' ';
	}
	std::cout << '\n';
	std::cout << "Filters: ";
	while(!(filters.empty())){
		for(unsigned j=0;filters.front()[j] != NULL;++j){
			std::cout << filters.front()[j] << ' ';
		}
		filters.pop();
		std::cout << '\n';
	}
	std::cout << '\n';
	if(outputFile != NULL)
		std::cout << "Output File: " << outputFile << '\n';
	if(inputFile != NULL)
		std::cout << "Input File: " << inputFile << '\n';
	std::cout << "Left Count: " << leftCount << '\n'
	<< "Right Count: " << rightCount << '\n'
	<< "Pipe Count: " << pipeCount << '\n';
}

int decipher(char **c){
	bool inputR = false;
	bool outputR = false;
	bool pipesR = false;
	//--------------------
	int fd1 = 0;
	int fd2 = 0;
	int saveOut1 = 0;
	int saveIn1 = 0;
	for(unsigned i=0; c[i]!=NULL; ++i){
		if(c[i][0] == '<' && c[i][1] == '\0'){
			inputR = true;
			++leftCount;
			if(rightCount > 1){
				std::cout << "ERROR: case of '>' and/or '>>' preceding '<'.\n";
				return 1;
			}
			else if(pipeCount > 1){
				std::cout << "ERROR: case of '|' preceding '<'.\n";
				return 1;
			}
			unsigned j=0;
			for(;j<i;++j){
				command[j] = c[j];
			}
			command[j] = NULL;
			++i;
			if(c[i]!=NULL)inputFile = c[i];
			//-----------------------------
			fd1 = open(inputFile,O_RDONLY);
			if(fd1 == -1){
				perror("open");
				return 1;
			}
			saveOut1 = dup(0);
			if(saveOut1 == -1){
				perror("dup");
				return 1;
			}
			if(dup2(fd1,0) == -1){
				perror("dup2");
				return 1;
			}
			if(close(fd1) == -1){
				perror("close");
				return 1;
			}
			//----------------------------
		}
		else if(c[i][0] == '|' && c[i][1] == '\0'){
			pipesR = true;
			++pipeCount;
			if(rightCount > 1){
				std::cout << "ERROR: case of '>' and/or '>>' preceding '|'.\n";
				return 1;
			}
			if(leftCount == 0){
				unsigned k=0;
				for(;k<i;++k){
					command[k] = c[k];
				}
				command[k] = NULL;
			}
			++i;
			char **temp1 = new char*[64];
			unsigned l=0;
			while(c[i][0] != '>' && c[i][0] != '|' && c[i] != NULL){
				temp1[l] = c[i];
				++l;
				++i;
			}
			std::cout << "02\n";
			i -= 1;
			filters.push(temp1);
			temp1 = NULL;
			delete temp1;
		}
		else if((c[i][0] == '>' && c[i][1] == '\0') || (c[i][0] == '>' && c[i][1] == '>' &&
		c[i][2] == '\0')){
			outputR = true;
			bool right1 = false;
			if(c[i][1] == '\0')right1 = true;
			++rightCount;
			if(leftCount + pipeCount == 0){
				unsigned m=0;
				for(;m<i;++m){
					command[m] = c[m];
				}
				command[m] = NULL;
			}
			++i;
			if(c[i] != NULL)outputFile = c[i];
			//--------------------------------
			if(right1) fd2 = open(outputFile, O_CREAT | O_WRONLY | O_TRUNC);
			else fd2 = open(outputFile, O_CREAT | O_WRONLY | O_APPEND);
			if(fd2 == -1){
				perror("open");
				return 1;
			}
			saveIn1 = dup(1);
			if(saveIn1 == -1){
				perror("dup");
				return 1;
			}
			if(dup2(fd2,1) == -1){
				perror("dup2");
				return 1;
			}
			if(close(fd2) == -1){
				perror("close");
				return 1;
			}
			if(!pipesR && !inputR){
				if(execvp(command[0],command) == -1){
					perror("execvp");
					return 1;
				}
				if(dup2(saveIn1,1) == -1){
					perror("dup2");
					return 1;
				}
				if(close(fd2) == -1){
					perror("close");
					return 1;
				}
			}
			else if(!pipesR && inputR){
				if(execvp(command[0],command) == -1){
					perror("execvp");
					return 1;
				}
				if(dup2(saveIn1,1) == -1){
					perror("dup2");
					return 1;
				}
				if(close(fd2) == -1){
					perror("close");
					return 1;
				}
			}
			//------------------------------
		}
	}
	if(inputR && !outputR && !pipesR){
		if(execvp(command[0],command) == -1){
			perror("execvp");
			return 1;
		}
		if(dup2(saveOut1,0) == -1){
			perror("dup2");
			return 1;
		}
		if(close(fd1) == -1){
			perror("close");
			return 1;
		}
	}
	if(!inputR && !outputR && !pipesR)return 3;
		
	return 0;
}
		

















#endif
