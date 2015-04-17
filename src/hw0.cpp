#include <iostream>
#include <cstdlib>				//use exit()
#include <vector>
#include <unistd.h>				//use fork(), execvp()
#include <sys/types.h>			//use waitpid()
#include <sys/wait.h>			//use waitpid()
#include <stdio.h>				//use perror()
#include <errno.h>				//use perror()
#include <string.h>				//use strtok()



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

std::vector<std::string> conn_vector;

void find_connectors(std::string &input){
	for(unsigned i=0;i<input.size();++i){
		if(input.at(i) == ';') conn_vector.push_back(";");
		else if(i != input.size() - 1){
			if(input.at(i) == '|' && input.at(i + 1) == '|'){
				conn_vector.push_back("||");
				++i;
			}
			else if(input.at(i) == '&' && input.at(i + 1) == '&'){
				conn_vector.push_back("&&");
				++i;
			}
		}
	}
}

void token_comment(char *arr){
	arr = strtok(arr, "#");
}

int token_connectors(char *cstr, char **argv){
	int argc=0;
	argv[argc] = strtok(cstr, ";|&");
	while(argv[argc] != NULL){
		++argc;
		argv[argc] = strtok(NULL, ";|&");
	}
	return argc;
}

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
int execute_cmd(char** temp, unsigned i){
	int fork_pid = fork();	
	if(temp[0][0] == 'e' && temp[0][1] == 'x' && temp[0][2] == 'i'
	&& temp[0][3] == 't' && temp[0][4] == '\0') return 2;
	if(fork_pid == -1) perror("fork");
	else if(fork_pid == 0){															//If inside Child process branch
		int cmd_status = execvp(temp[0], temp);
		if(cmd_status == -1){
			perror("execvp");
			exit(1);
		}
		else exit(0);
	}
	else{
		int ret;
		waitpid(fork_pid, &ret, 0);
		if(0 < conn_vector.size() && i < conn_vector.size()){
			if(ret == 0){
				if(conn_vector.at(i) == "||") return 1;
			}
			else{
				if(conn_vector.at(i) == "&&") return 1;
			}
		}
	}

return 0;
}

int token_spaces(char **argv, std::vector<char**>& argv_arr, int argc){
	int argument = 0;
	int word = 0;
	while(argument < argc){
		char *temp[1024];
		temp[word] = strtok(argv[argument], " ");
		while(temp[word] != NULL){
			++word;
			temp[word] = strtok(NULL, " ");
		}
		int ret = execute_cmd(temp, argument);
		if(ret == 2) return 1;
		else if(ret == 1) return 0;
		++argument;
		word = 0;
	}
	return 0;
}
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
/*
int execute_cmds(std::vector<char**> &argv_arr){
	unsigned i=0;
	for(;i < argv_arr.size();++i){
		int fork_pid = fork();	
		char **temp = argv_arr.at(i);
		if(temp[0][0] == 'e' && temp[0][1] == 'x' && temp[0][2] == 'i'
		&& temp[0][3] == 't' && temp[0][4] == '\0') return 1;
		if(fork_pid == -1) perror("fork");
		else if(fork_pid == 0){															//If inside Child process branch
			int cmd_status = execvp(temp[0], temp);
			if(cmd_status == -1){
				perror("execvp");
				exit(1);
			}
			else exit(0);
		}
		else{
			int ret;
			waitpid(fork_pid, &ret, 0);
			if(0 < conn_vector.size() && i < conn_vector.size()){
				if(ret == 0){
					if(conn_vector.at(i) == "||") return 0;
				}
				else{
					if(conn_vector.at(i) == "&&") return 0;
				}
			}
		}
	}
	return 0;
}
*/
void print_info(){
	char *login = getlogin();
	if(login == NULL){
		perror("getlogin");
		std::cout << "unknown_user";
	}
	else{
		for(int i=0;login[i]!='\0';++i)std::cout<<login[i];
	}
	std::cout << '@';
	char hostname[64];
	int host_status = gethostname(hostname, sizeof(hostname));
	if(host_status == -1){
		perror("gethostname");
		std::cout << "unknown_host";
	}
	else{
		for(int j=0;hostname[j]!='\0';++j)std::cout<<hostname[j];
	}
}

int main()
{
	while(1){
	print_info();
	std::cout << "$ ";
	std::string input_string;
	getline(std::cin, input_string);
	if(input_string == "exit") return 0;
	if(input_string == "");
	else if(input_string.at(0) != '#'){

		int size = input_string.size() + 1;

		char *input_cstring = new char [size];

		strcpy(input_cstring, input_string.c_str());

		token_comment(input_cstring);

		find_connectors(input_string);

		char **argv = new char *[size];

		int argc = token_connectors(input_cstring, argv);

		std::vector<char **> argv_arr(argc);

		int ret = token_spaces(argv, argv_arr, argc);

		conn_vector.clear();

		delete input_cstring;
		delete argv;

		if(ret == 1)exit(0);


//		disp_3d_arr(argv_arr, argc);
	}

	}

return 0;
}
