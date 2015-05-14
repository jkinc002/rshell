#include <iostream>
#include <cstdlib>				//use exit()
#include <vector>
#include <unistd.h>				//use fork(), execvp()
#include <sys/types.h>			//use waitpid()
#include <sys/wait.h>			//use waitpid()
#include <stdio.h>				//use perror()
#include <errno.h>				//use perror()
#include <string.h>				//use strtok()
#include <queue>
#include <algorithm>
#include <utility>
#include <fcntl.h>
#include <sys/stat.h>
#include <fstream>
#include <istream>

std::queue<char> connectors;

std::string encrypt(std::string s){
	std::string ret;
	for(unsigned i=0;i<s.size();++i){
		if(s.at(i) == ';') ret += "A-X-01";
		else if(i < s.size() - 1 && (s.at(i) == '&' && s.at(i+1) == '&')){
			ret += "A-X-02";
			++i;
		}
		else if(i < s.size() - 1 && (s.at(i) == '|' && s.at(i+1) == '|')){
			ret += "A-X-03";
			++i;
		}
		else if( s.at(i) == '>'){
			ret += ' ';
			ret += s.at(i);
			if(i < s.size() - 1 && s.at(i+1) == '>'){
				++i;
				ret += '>';
			}
			ret += ' ';
		}
		else if(s.at(i) == '<'){
			ret += ' ';
			ret += s.at(i);
			ret += ' ';
		}
		else ret += s.at(i);
	}
	return ret;
}

std::vector<std::string>token_connectors(std::string s){
	std::vector<std::string>ret;
	std::string temp;
	for(unsigned i=0;i<s.size();++i){
		if(s.at(i)=='A' && s.at(i+1)=='-'
		&& s.at(i+2)=='X' && s.at(i+3)=='-'
		&& s.at(i+4)=='0' && s.at(i+5)=='1'
		&& i < s.size() - 5){
			ret.push_back(temp);
			temp.clear();
			i += 5;
			connectors.push(';');
		}
		else if(s.at(i)=='A' && s.at(i+1)=='-'
		&& s.at(i+2)=='X' && s.at(i+3)=='-'
		&& s.at(i+4)=='0' && s.at(i+5)=='2'
		&& i < s.size() - 5){
			ret.push_back(temp);
			temp.clear();
			i += 5;
			connectors.push('&');
		}
		else if(s.at(i)=='A' && s.at(i+1)=='-'
		&& s.at(i+2)=='X' && s.at(i+3)=='-'
		&& s.at(i+4)=='0' && s.at(i+5)=='3'
		&& i < s.size() - 5){
			ret.push_back(temp);
			temp.clear();
			i += 5;
			connectors.push('|');
		}
		else {
			temp += s.at(i);
		}
	}
	ret.push_back(temp);
	return ret;
}

void print_info(){
	char *login = getlogin();
	if(login == NULL){
		perror("getlogin");
		std::cout << "unknown_user";
	}
	else for(int i=0;login[i]!='\0';++i)std::cout << login[i];
	std::cout << '@';
	char hostname[64];
	int host_status = gethostname(hostname, sizeof(hostname));
	if(host_status == -1){
		perror("gethostname");
		std::cout << "unknown_host";
	}
	else for(int j=0;hostname[j]!='\0';++j)std::cout << hostname[j];
}

std::vector<char*>convert_vec(std::vector<std::string> v){
	std::vector<char*> ret;
	for(unsigned i=0;i<v.size();++i){
		char *temp = new char [v.at(i).size() + 1];
		strcpy(temp, v.at(i).c_str());
		ret.push_back(temp);
		temp = NULL;
		delete temp;
	}
	return ret;
}

std::vector<char **>convert_vec2(std::vector<char*> v){
	std::vector<char**> ret;
	unsigned j=0;
	for(unsigned i=0;i<v.size();++i){
		char **temp = new char *[1024];
		temp[j] = strtok(v.at(i)," ");
		while(temp[j] != NULL){
			++j;
			temp[j] = strtok(NULL, " ");
		}
		if(temp[0]!=NULL) ret.push_back(temp);
		else{
			char c[1];
			c[0] = ' ';
			temp[0] = c;
			temp[1] = NULL;
		}
		temp = NULL;
		delete temp;
		j = 0;
	}
	return ret;
}


bool run = true;

int execute_cmd(char **c){
	if(c[0][0] == 'e' && c[0][1] == 'x'
	&& c[0][2] == 'i' && c[0][3] == 't'
	&& c[0][4] == '\0') return 2;

	int fdin = 0;
	int fdout = 1;
	int fd[2];
	if(pipe(fd) == -1){
		perror("pipe");
		return 1;
	}

	int pid = fork();
	if(pid == -1){
		perror("fork");
		return -1;
	}
	else if(pid == 0){
		if(c == NULL) exit(0);
		char *command[64];
		char *inputFile = NULL;
		char *outputFile = NULL;

		std::queue<std::string> order;

		int leftCount = 0;
		int RightCount = 0;
		int RightCount2 = 0;
		
		for(unsigned i=0;c[i]!=NULL;++i){
			if(c[i][0] == '<' && c[i][1] == '\0'){
				++leftCount;
				order.push("<");
				if(leftCount > 1){
					std::cout << "ERROR: Multiple '<' cases.\n";
					exit(1);
				}
				else if(RightCount + RightCount2 > 0){
					std::cout << "ERROR: Case of '>' or '>>' preceding '<'.\n";
					exit(1);
				}
				unsigned j=0;
				for(;j<i;++j){
					command[j] = c[j];
				}
				command[j] = NULL;
				++j;
				if(c[j]!=NULL && leftCount == 1)inputFile = c[j];
			}
			else if(c[i][0] == '>' && c[i][1] == '\0'){
				++RightCount;
				order.push(">");
				if(RightCount + RightCount2 > 1){
					std::cout << "ERROR: Multiple '>' and/or '>>' cases.\n";
					exit(1);
				}
				unsigned l=0;
				if(leftCount == 0){
					for(;l<i;++l){
						command[l] = c[l];
					}
					command[l] = NULL;
				}
				++i;
				if(c[i]!=NULL && RightCount == 1)outputFile = c[i];
			}
			else if(c[i][0] == '>' && c[i][1] == '>' && c[i][2] == '\0'){
				++RightCount2;
				order.push(">>");
				if(RightCount + RightCount2 > 1){
					std::cout << "ERROR: Multiple '>' and/or '>>' cases.\n";
					exit(1);
				}
				unsigned m=0;
				if(leftCount == 0){
					for(;m<i;++m){
						command[m] = c[m];
					}
				command[m] = NULL;
				}
				++i;
				if(c[i]!=NULL && RightCount2 == 1)outputFile = c[i];
			}
		}
		if(order.size() > 0)c = command;
		while(!(order.empty())){
			if(order.front() == "<"){
				int fd1 = open(inputFile,O_RDONLY);
				if(fd1 == -1){
					perror("open");
					exit(1);
				}
				int saveIn = dup(0);
				if(saveIn == -1){
					perror("dup");
					exit(1);
				}
				if(dup2(fd1,0) == -1){
					perror("dup2");
					exit(1);
				}
				if(close(fd1) == -1){
					perror("close");
					exit(1);
				}
				order.pop();
				if(order.empty()){
					if(execvp(c[0],c) == -1){
						perror("execvp");
						exit(1);
					}
					if(dup2(saveIn, 1) == -1){
						perror("dup2");
						exit(1);
					}
					if(close(saveIn) == -1){
						perror("close");
						exit(1);
					}
				}
				else if(order.front() == ">" || order.front() == ">>"){
					int fd3;
					if(order.front() == ">")
						fd3 = open(outputFile,O_CREAT | O_WRONLY | O_TRUNC);
					if(order.front() == ">>")
						fd3 = open(outputFile,O_CREAT | O_WRONLY | O_APPEND);
					if(fd3 == -1){
						perror("open");
						exit(1);
					}
					int saveOut2 = dup(1);
					if(saveOut2 == -1){
						perror("dup");
						exit(1);
					}
					if(dup2(fd3,1) == -1){
						perror("dup2");
						exit(1);
					}
					if(close(fd3) == -1){
						perror("close");
						exit(1);
					}
					if(execvp(c[0],c) == -1){
						perror("execvp");
						exit(1);
					}
					if(dup2(saveOut2,1) == -1){
						perror("dup2");
						exit(1);
					}
					if(close(saveOut2) == -1){
						perror("close");
						exit(1);
					}
					order.pop();
				}
			}
			else if(order.front() == ">" || order.front() == ">>"){
				int fd2;
				if(order.front() == ">")
					fd2 = open(outputFile,O_CREAT | O_WRONLY | O_TRUNC);
				else if(order.front() == ">>")
					fd2 = open(outputFile,O_CREAT | O_WRONLY | O_APPEND);
				if(fd2 == -1){	
					perror("open");
					exit(1);
				}
				int saveOut = dup(1);
				if(saveOut == -1){ perror("dup");
					exit(1);
				}
				if(dup2(fd2,1) == -1){	
					perror("dup2");
					exit(1);
				}
				if(close(fd2) == -1){	
					perror("close");
					exit(1);
				}
				if(execvp(c[0],c) == -1){
					perror("execvp");
					exit(1);
				}
				if(dup2(saveOut,1) == -1){
					perror("dup2");
					exit(1);
				}
				if(close(saveOut) == -1){
					perror("close");
					exit(1);
				}
			}
		}
		if(order.size() == 0){
			int cmd = execvp(c[0],c);
			if(cmd == -1){
				perror("execvp");
				return -1;
			}
			else exit(0);
		}
		exit(0);
	}
	else{
		int ret;
		waitpid(pid,&ret,0);
		if(ret == -1){
			perror("waitpid");
		}
		if(connectors.size() != 0){
			if(ret == 0){
				if(connectors.front() == '|')return 1;
			}
			else{
				if(connectors.front() == '&')return 1;
			}
			connectors.pop();
		}
	}
	return 0;
	
}

std::string token_comment(std::string s){
	std::string ret;
	for(unsigned i=0;s.at(i) != '#' && i < s.size();++i){
		ret += s.at(i);
	}
	return ret;
}

int main(int argc, char *argv[]){
	while(run){
		print_info();
		std::cout << "$ ";
		std::string input_str;
		getline(std::cin, input_str);
		//input_str = token_comment(input_str);
		input_str = encrypt(input_str);
		std::vector<std::string> input_vec = token_connectors(input_str);
		std::vector<char*>input_vec2 = convert_vec(input_vec);
		std::vector<char **> input_vec3 = convert_vec2(input_vec2);
		bool run2 = true;
		for(unsigned i=0; run2 && i < input_vec3.size();++i){
			int cmp = execute_cmd(input_vec3.at(i));
			if(cmp == 1) run2 = false;
			else if(cmp == 2) run = false;
		}
		std::queue<char> empty;
		std::swap(connectors, empty);
	}
	return 0;
}
