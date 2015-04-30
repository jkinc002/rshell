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
#include <utility>
#include <queue>
#include <grp.h>
#include <pwd.h>



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
void print_cstr(const char* c){
	struct stat s;
	int ret = stat(c, &s);
	if(ret == -1) perror("stat");
	if(s.st_mode & S_IFDIR){
		std::cout << "\033[1;34m";
	}
	else if(s.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)){
		std::cout << "\033[1;32m";
	}
	if(c[0] == '.'){
		std::cout << "\033[40m";
	}
	for(unsigned i=0;c[i]!='\0';++i){
		std::cout << c[i];
	}
		std::cout << "\033[0;39m";
		std::cout << "\033[0;49m";
}

std::vector<const char*> dirPath;
std::vector<dirent*> file_vector;

struct l_disp {
	public:
		std::string permissions;
		std::string spaces1;
		nlink_t link_size;
		std::string name;
		std::string group;
		std::string spaces2;
		off_t file_size;
		time_t date;
		const char* file_name;
	public:
		void print(){
			std::cout << permissions
			<< spaces1
			<< link_size
			<< ' '
			<< name
			<< ' '
			<< group
			<< spaces2
			<< ' '
			<< file_size
			<< ' '
			<< date
			<< ' ';
			print_cstr(file_name);
			std::cout << '\n';
		}
};

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
		void sort_names(){
			if(names.size() == 1) return;
			for(unsigned i=0;i<names.size();++i){
				for(unsigned j=0;j<names.size();++j){
					int cmp = strcmp(names.at(i),names.at(j));
					if(cmp < 0) std::swap(names.at(i),names.at(j));
					else;
				}
			}
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

/*
===========================================================================
-l FLAG FORMAT
---------------------------------------------------------------------------
File Permissions,	Number of Links,	Owner Name,		Owner Group
File Size,			Time of Last Mod,	file/dir Name
===========================================================================
*/
std::queue<l_disp> lvec;
int link_size = 1;
int file_size = 1;
int date_size = 1;

void get_sizes(){
	int l_size = 0;
	int f_size = 0;
	int d_size = 0;
	int ret;
	struct stat s;
	for(unsigned i=0;i<file_vector.size();++i){
		ret = stat(file_vector.at(i)->d_name, &s);
		if(ret == -1){
			perror("stat");
			return;
		}
		if(s.st_nlink >= 1000000) l_size = 7;
		else if(s.st_nlink >= 100000) l_size = 6;
		else if(s.st_nlink >= 10000) l_size = 5;
		else if(s.st_nlink >= 1000) l_size = 4;
		else if(s.st_nlink >= 100) l_size = 3;
		else if(s.st_nlink >= 10) l_size = 2;
		else l_size = 1;

		if(s.st_size >= 1000000) f_size = 7;
		else if(s.st_size >= 100000) f_size = 6;
		else if(s.st_size >= 10000) f_size = 5;	
		else if(s.st_size >= 1000) f_size = 4;
		else if(s.st_size >= 100) f_size = 3;
		else if(s.st_size >= 10) f_size = 2;
		else f_size = 1;

		if(l_size > link_size) link_size = l_size;
		if(f_size > file_size) file_size = f_size;

	}
}

int check_l_size(const char* c){
	struct stat s;
	int ret = stat(c, &s);
	if(ret == -1){
		perror("stat");
		return 0;
	}
	int l_size;
	if(s.st_nlink >= 1000000) l_size = 7;
	else if(s.st_nlink >= 100000) l_size = 6;
	else if(s.st_nlink >= 10000) l_size = 5;
	else if(s.st_nlink >= 1000) l_size = 4;
	else if(s.st_nlink >= 100) l_size = 3;
	else if(s.st_nlink >= 10) l_size = 2;
	else l_size = 1;

	int cmp = link_size - l_size;

	return cmp;
}

int check_f_size(const char *c){
	struct stat s;
	int ret = stat(c, &s);
	if(ret == -1){
		perror("stat");
		return 0;
	}
	int f_size;
	if(s.st_size >= 1000000) f_size = 7;
	else if(s.st_size >= 100000) f_size = 6;
	else if(s.st_size >= 10000) f_size = 5;	
	else if(s.st_size >= 1000) f_size = 4;
	else if(s.st_size >= 100) f_size = 3;
	else if(s.st_size >= 10) f_size = 2;
	else f_size = 1;

	int cmp = file_size - f_size;
	return cmp;
}



void feed_lvec(const char *c){
	std::string temp;
	struct stat s;
	l_disp l;
	int ret = stat(c, &s);
	if(ret == -1) perror("stat");
	else {
		if(s.st_mode & S_IFDIR) temp += 'd';
		else temp += '-';
		if(s.st_mode & S_IRUSR) temp += 'r';
		else temp += '-';
		if(s.st_mode & S_IWUSR) temp += 'w';
		else temp += '-';
		if(s.st_mode & S_IXUSR) temp += 'x';
		else temp += '-';
		if(s.st_mode & S_IRGRP) temp += 'r';
		else temp += '-';
		if(s.st_mode & S_IWGRP) temp += 'w';
		else temp += '-';
		if(s.st_mode & S_IXGRP) temp += 'x';
		else temp += '-';
		if(s.st_mode & S_IROTH) temp += 'r';
		else temp += '-';
		if(s.st_mode & S_IWOTH) temp += 'w';
		else temp += '-';
		if(s.st_mode & S_IXOTH) temp += 'x';
		else temp += '-';

		temp += ' ';
		l.permissions = temp;
		temp.clear();



		int cmp = check_l_size(c);
		for(int i=0;i<cmp;++i){
			temp += ' ';
		}
		l.spaces1 = temp;
		temp.clear();
		l.link_size = s.st_nlink;
		struct passwd *pw = getpwuid(s.st_uid);
		if(pw == 0){
			perror("getpwuid");
			return;
		}
		struct group *gr = getgrgid(s.st_gid);
		if(gr == 0){
			perror("getgrgid");
			return;
		}
		l.name = pw->pw_name;
		l.group = gr->gr_name;

		int cmp2 = check_f_size(c);
		for(int j=0;j<cmp2;++j){
			temp += ' ';
		}
		l.spaces2 = temp;
		l.file_size = s.st_size;
		l.date = s.st_mtime;
		l.file_name = c;
		lvec.push(l);
	}
	
}


void print_l(){
	if(file_vector.size() == 0) return;
	get_sizes();
	unsigned j = 0;
	if(!f.a_flag){
		for(;file_vector.at(j)->d_name[0] == '.';++j){}
		if(j < file_vector.size()){
			feed_lvec(file_vector.at(j)->d_name);
		}
		else return;
	}
	else feed_lvec(file_vector.at(0)->d_name);
	for(unsigned i=++j;i<file_vector.size();++i){
		if(file_vector.at(i)->d_name[0] == '.'){
			if(f.a_flag){
				feed_lvec(file_vector.at(i)->d_name);
			}
		}
		else{
			feed_lvec(file_vector.at(i)->d_name);
		}
	}
	int qSize = lvec.size();
	for(unsigned k=0;k<qSize;++k){
		lvec.front().print();
		lvec.pop();
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
	}
}
/*
void sort_names(){
	if(file_vector.size() <= 1) return;
	for(unsigned i=0;i<d.names.size();++i){
		for(unsigned j=i;j<d.names.size();++j){
			int cmp = strcmp(d.names.at(i),d.names.at(j));
			if(cmp < 0);
			else if(cmp == 0);
			else std::swap(d.names.at(i),d.names.at(j));
		}
	}
}
*/	

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
			if(f.l_flag) print_l();
			else print_norm();
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
		d.sort_names();
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
					if(d.names.size() > 1){
						std::cout << d.names.at(i);
						std::cout << ":\n";
					}
					print_start(beingRead, isOpen);
					file_vector.clear();
					int isClosed = closedir(isOpen);
					if(isClosed == -1) perror("closedir");
					if(i != d.names.size() - 1) std::cout << "\n";
				}
			}
		}
	}
}

int main(int argc, char *argv[])
{
	d.names.empty();
	read_args(argc, argv);
	execute_ls();




	return 0;
}








