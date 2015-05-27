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
#include <stack>
#include <signal.h>
#include "myio.h"

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
		temp[j] = NULL;
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
void mychdir(int n){
	if(n == 0){
		const char *homedir = getenv("HOME");
		if(homedir == NULL){
			perror("getenv");
			return;
		}
		if(chdir(homedir) == -1){
			perror("chdir");
			return;
		}
	}
	else if(n == 1){
		char *currpath = getenv("PWD");
		unsigned i=0;
		unsigned lastdash = 0;
		for(;currpath[i]!='\0';++i){
			if(currpath[i] == '/') lastdash = i;
		}
		char prevdir[lastdash + 1];
		unsigned j=0;
		for(;j<lastdash;++j){
			prevdir[j]=currpath[j];
		}
		if(chdir(prevdir) == -1){
			perror("chdir");
			return;
		}
		return;
	}
}

int iscd(char **c){
	if(c[0][0] == 'c' && c[0][1] == 'd'
	&& c[0][2] == '\0'){
		if(c[1] == NULL) mychdir(0);
		else if(c[1][0] == '-' && c[1][1] == '\0') mychdir(1);
		else std::cout << "cd PATH\n";
		return 0;
	}
	return 1;
}

int execute_cmd(char **c){
	if(isExit(c[0])) return 2;
	int cdret = iscd(c);
	if(cdret == 0) return 0;
	int pid = fork();
	if(pid == -1){
		perror("fork");
		return -1;
	}
	else if(pid == 0){
		if(c == NULL) exit(0);
		int ret = decipher(c);
		if(ret == 1) exit(1);
		if(ret == 3){
			if(execvp(c[0],c) == -1){
				perror("execvp");
				exit(1);
			}
		}
		exit(0);
	}
	else{
		int ret2;
		waitpid(pid,&ret2,0);
		if(ret2 == -1){
			perror("waitpid");
		}
	}
	return 0;
}

struct sigaction act;
struct sigaction old;
		
std::string token_comment(std::string s){
	std::string ret;
	for(unsigned i=0;s.at(i) != '#' && i < s.size();++i){
		ret += s.at(i);
	}
	
	return ret;
}

void sighandler(int signum, siginfo_t *info, void *ptr){
	if(signum == SIGINT){
		std::cout << "\n\n\n\n\n";
	}
	//exit(1);
}
void print_wd(){
	char cwd[BUFSIZ];
	if(getcwd(cwd,sizeof(cwd)) == NULL){
		perror("getcwd");
	}
	std::cout << cwd;
}

int main(int argc, char *argv[]){
	act.sa_sigaction = sighandler;
	act.sa_flags = SA_SIGINFO;
	if(-1 == sigaction(SIGINT, &act, &old)){
		perror("sigaction");
	}
	while(run){
		std::cin.clear();
		print_info();
		std::cout << ':';
		print_wd();
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
