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



/*
==========================================================================
SYSCALLS
-------------------------------------------------------------------------- 
CALL: int execvp(const char *path, char *const argv[])
RETURN: -1 if error occured, anything else if no error occured
--------------------------------------------------------------------------
CALL: pid_t fork(void)
RETURN: Returns child process's PID when in the Parent
		Returns value 0 when in the Child
		Returns -1 on failure inside the Parent (no Child created)
--------------------------------------------------------------------------
CALL: pid_t waitpid(pid_t pid, int *status, int options)
RETURN: Returns PID of terminated Child on success
		Returns -1 on failure
--------------------------------------------------------------------------
CALL: void perror(const char *s)
RETURN: Prints error message related to the command name given by 'char *s'
--------------------------------------------------------------------------
CALL: char *strtok(char *str, const char *delim)
RETURN: Returns pointer to the next token
		Returns NULL if there are no more tokens
==========================================================================
ARGUMENT ORAGNIZATION
--------------------------------------------------------------------------
1. Get user input as string
2. Convert string to NULL terminated cstring
3. Tokenize '#' (comment) out of the cstring
4. Initialize 2D char array
4. Tokenize connectors ';' '||' '&&'
6. Fill 2D char array with tokenized segments (each segment is an arg line)
==========================================================================
EXTRA CREDIT FUNCTIONS
--------------------------------------------------------------------------
CALL: char *getlogin(void)
RETURN: Returns char pointer to the username on success
		Returns NULL on failure
--------------------------------------------------------------------------
CALL: int gethostname(char *name, size_t len)
RETURN: char *name contains the NULL terminated host-name cstring
		size_t len contains length of the host-name
==========================================================================
*/
/*
=========================================================================
TOKEN_SPACES
-------------------------------------------------------------------------
Parameters:
	argv: 2D char array that holds the individual arguments that existed
		  between any connectors.
argv_arr: A vector of 2D arrays, a single element is meant to contain one
		  argument's tokened words. Each element is a different argument.
	argc: The number of arguments that exist based on user input.
-------------------------------------------------------------------------
Description:
	Create temporary char** which will recieve a single tokened argument
	Assign vector.at(i) = temp
	Deallocate temp
	Repeat for all arguments
	Vector<char**> is now contains a single, tokened argument per each
		of its elements
========================================================================
*/

/*
==========================================================================
EXECUTE_CMDS
--------------------------------------------------------------------------
Parameter:
	argv_arr: Vector of 2D arrays, each element is a unique, tokened argument
--------------------------------------------------------------------------
Description:
	For-loop(while i < # of arguments){
		Fork process
		Create char **temp = argv_arr.at(i)
		Execute command pointed to by temp inside the Child process
		Wait inside the Parent until the Child terminates
		Take return value of terminated process: 0 = SUCCESS, 1 = FAILURE
		Utilize user given connectors appropriately
		++i
	}
==========================================================================
*/

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
	int pid = fork();
	if(pid == -1){
		perror("fork");
		return -1;
	}
	else if(pid == 0){
		if(c == NULL) exit(0);
		int cmd = execvp(c[0],c);
		if(cmd == -1){
			perror("execvp");
			return -1;
		}
		else exit(0);
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
	/*
	bool run = true;
	while(run){
		int size = input_str.size() + 1;
		const char *input_cstr = input_str.c_str();
	}
	*/
	





	return 0;
}
