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

void token_spaces(char **argv, std::vector<char**>& argv_arr, int argc){
	int argument = 0;
	int word = 0;
	while(argument < argc){
		char **temp = new char *[1024];
		temp[word] = strtok(argv[argument], " ");
		while(temp[word] != NULL){
			++word;
			temp[word] = strtok(NULL, " ");
		}
		argv_arr.at(argument) = temp;
		++argument;
		word = 0;
	}
	return;
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
void execute_cmds(std::vector<char**> &argv_arr){
	unsigned i=0;
	for(;i < argv_arr.size();++i){
		int fork_pid = fork();	
		char **temp = argv_arr.at(i);
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
					if(conn_vector.at(i) == "||") return;
				}
				else{
					if(conn_vector.at(i) == "&&") return;
				}
			}
		}
	}
	return;
}

int main()
{
	while(1){
	std::cout << "$ ";
	std::string input_string;
	getline(std::cin, input_string);
	if(input_string == "exit") return 0;
	if(input_string.at(0) != '#'){

		int size = input_string.size() + 1;

		char *input_cstring = new char [size];

		strcpy(input_cstring, input_string.c_str());

		token_comment(input_cstring);

		find_connectors(input_string);

		char **argv = new char *[size];

		int argc = token_connectors(input_cstring, argv);

		std::vector<char **> argv_arr(argc);

		token_spaces(argv, argv_arr, argc);

		execute_cmds(argv_arr);

		conn_vector.clear();

		delete input_cstring;
		delete argv;

//		disp_3d_arr(argv_arr, argc);
	}

	}

return 0;
}
