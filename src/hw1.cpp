#include <iostream>
#include <stdio.h>				//perror()
#include <cstdio>
#include <vector>
#include <string>
#include <sys/types.h>			//opendir(), closedir()
#include <dirent.h>				//opendir(), closedir(), readdir()
#include <sys/types.h>			//stat()
#include <sys/stat.h>			//stat()
#include <unistd.h>
#include <cstring>



/*
===========================================================================
SYSCALLS
---------------------------------------------------------------------------
OPENDIR
---------------------------------------------------------------------------
CALL  : DIR *opendir(const char *name)
RETURN: A pointer to the directory stream on SUCCESS
		NULL on FAILURE
---------------------------------------------------------------------------
CLOSEDIR
---------------------------------------------------------------------------
CALL  : int closedir(DIR *dirp)
RETURN: 0 on SUCCESS
		-1 on FAILURE
---------------------------------------------------------------------------
READDIR	
---------------------------------------------------------------------------
CALL  : struct dirent *readdir(DIR *dirp)
RETURN: A pointer to a dirent structure
		NULL upon reaching the end of the directory stream
		NULL on FAILURE
---------------------------------------------------------------------------
STAT
---------------------------------------------------------------------------
CALL  : int stat(const char *path, struct stat *buf)
RETURN: 0 on SUCCESS
		-1 on FAILURE
===========================================================================

===========================================================================
STRUCTURES
---------------------------------------------------------------------------
STAT
---------------------------------------------------------------------------
struct stat {
	dev_t		st_dev
	ino_t		st_ino
	mode_t		st_mode
	nlink_t		st_nlink
	uid_t		st_uid
	gid_t		st_gid
	dev_t		st_rdev
	off_t		st_size
	blksize_t	st_blksize
	time_t		st_atime
	time_t		st_mtime
	time_t		st_ctime
}
---------------------------------------------------------------------------
DIRENT
---------------------------------------------------------------------------
struct dirent {
	ino_t			d_ino		//inode number
	off_t			d_off		//offset to next dirent
	unsigned short	d_reclen	//lenght of this record
	unsigned char	d_type		//type of file
	char			d_name[256]	//filename
}
======================================================================
*/

std::vector<const char*> dirPath;
std::vector<dirent*> file_vector;

struct flags {
	public:
		bool a_flag;
		bool l_flag;
		bool R_flag;
		bool unknown;
		std::vector<char> bad_flags;
	public:
		flags() : a_flag(false), l_flag(false), R_flag(false), unknown(false) {}
		void push_bad_flag(char c){
			if(bad_flags.size() == 0){
				bad_flags.push_back(c);
				return;
			}
			for(unsigned i=0;i<bad_flags.size();++i){
				if(bad_flags.at(i) == c) return;
			}
			bad_flags.push_back(c);
			return;
		}
		void print(){
			std::cout << "a = " << a_flag << '\n';
			std::cout << "l = " << l_flag << '\n';
			std::cout << "R = " << R_flag << '\n';
		}
};

struct dirs {
	public:
		std::vector<const char*> names;
		int name_count;
	public:
		dirs() : name_count(0) {}
		void push_name(const char *c){
			names.push_back(c);
			return;
		}
};

flags f;
dirs d;

void read_args(int argc, char *argv[]){
	for(unsigned i=1;i<argc;++i){
		if(argv[i][0]=='-'){
			for(unsigned k=1;argv[i][k]!='\0';++k){
				if(argv[i][k] == 'a') f.a_flag = true;
				if(argv[i][k] == 'l') f.l_flag = true;
				if(argv[i][k] == 'R') f.R_flag = true;
				else f.push_bad_flag(argv[i][k]);
			}
		}
		else{
			d.push_name(argv[i]);
			++d.name_count;
		}
	}
}

void print_cstr(const char* c){
	for(unsigned i=0;c[i]!='\0';++i){
		std::cout << c[i];
	}
}

void print_norm(){
	if(file_vector.size() == 0) return;
	unsigned j = 0;
	if(!f.a_flag){
		for(;file_vector.at(j)->d_name[0] == '.';++j){}
		if(j != file_vector.size()) print_cstr(file_vector.at(j)->d_name);
	}
	else print_cstr(file_vector.at(0)->d_name);
	for(unsigned i=++j;i<file_vector.size();++i){
		if(file_vector.at(i)->d_name[0] == '.'){
			if(f.a_flag){
				std::cout << "  ";
				print_cstr(file_vector.at(i)->d_name);
			}
		}
		else{
			std::cout << "  ";
			print_cstr(file_vector.at(i)->d_name);
		}
	}
	std::cout << '\n';
}
		

void feed_vector(dirent *drt, DIR *o){
	while(drt != NULL){
		file_vector.push_back(drt);
		drt = readdir(o);
		if(drt == NULL) perror("readdir");
	}
}

void sort_vector(){
	if(file_vector.size() <= 1) return;
	for(unsigned i=0;i<file_vector.size();++i){
		for(unsigned j=i;j<file_vector.size();++j){
			int cmp = strcmp(file_vector.at(i)->d_name,file_vector.at(j)->d_name);
			if(cmp < 0);
			else if(cmp == 0);
			else std::swap(file_vector.at(i), file_vector.at(j));
		}
	}
}

void sort_R(){}

void print_R(dirent *drt, DIR *o){}

void print_start(dirent *drt, DIR *o){
	if(drt == NULL) return;
	else{
		if(f.R_flag){
			sort_R();
		}
		else{
			feed_vector(drt, o);
			sort_vector();
			print_norm();
		}
	}
	return;
}

void execute_ls(){
	if(d.name_count == 0){
		d.push_name(".");
		++d.name_count;
	}
	if(d.names.size() != 0){
		for(unsigned i=0;i<d.names.size();++i){
		DIR *isOpen = opendir(d.names.at(i));
		if(isOpen == NULL){
			perror("opendir");
		}
		else{
			dirent *beingRead = readdir(isOpen);
			if(beingRead == NULL){
				perror("readdir");
			}
			else{
				print_start(beingRead, isOpen);
			}
		}
	}
	}
}

int main(int argc, char *argv[])
{
	read_args(argc, argv);
	execute_ls();




	return 0;
}








