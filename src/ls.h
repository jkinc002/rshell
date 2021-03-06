#include <iostream>
#include <stdio.h>
#include <cstdio>
#include <vector>
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <utility>
#include <queue>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <stdlib.h>
#include <stack>

#ifndef _LS_H_
#define _LS_H_

//======================================================================
std::vector<const char*> v1;

void feed_v1(const char **c, int a){
	if(a == 1)return;
	for(int i=1;i<a;++i){
		if(c[i][0] != '-')v1.push_back(c[i]);
	}
}
void sort_v1(){
	if(v1.size() <= 1)return;
	for(unsigned i=0;i<v1.size();++i){	
		for(unsigned j=i;j<v1.size();++j){
			if(strcmp(v1.at(i),v1.at(j)) > 0)std::swap(v1.at(i),v1.at(j));
		}
	}
}
//======================================================================
//======================================================================
std::vector<std::string> v2;
int v2_size = 0;
//======================================================================
struct flags {
	bool a;
	bool l;
	bool R;
	flags() : a(false),l(false),R(false){}
};

flags f;

void get_flags(const char **c, int a){
	if(a == 1) return;
	for(int i=1;i<a;++i){
		if(c[i][0] == '-'){
			for(int j=1;c[i][j] != '\0';++j){
				if(c[i][j] == 'a') f.a = true;
				else if(c[i][j] == 'l') f.l = true;
				else if(c[i][j] == 'R') f.R = true;
			}
		}
	}
}
//======================================================================
struct path {
	std::string p;
	std::vector<std::string> v;
	void push_dir(const char* c){
		std::string temp = c;
		this->v.push_back(temp);
	}
	void pop_dir(){
		if(this->v.size() == 1) return;
		this->v.pop_back();
	}
	const char *get_path(){
		std::string temp;
		unsigned i=0;
		for(;i<this->v.size() - 1;++i){
			temp += this->v.at(i);
			temp += '/';
		}
		temp += this->v.at(i);
		const char *ret = temp.c_str();
		return ret;
	}
	void print(){
		unsigned i=1;
		for(;i<this->v.size() - 1;++i){
			std::cout << this->v.at(i);
			if(i != 1) std::cout << '/';
		}
		if(i < this->v.size() && i > 1) std::cout << this->v.at(i) << ":\n";
		else std::cout << this->v.at(0) << ":\n";
	}
};
path p;
//======================================================================
bool is_dot(const char* c){
	unsigned i = 0;
	for(;c[i]!='\0';++i){}
	--i;
	if(c[i] == '.') return true;
	return false;
}
//======================================================================
int it = 0;
struct myfile {
	std::string permissions;
	int space1;
	nlink_t hard_links;
	int link_length;
	std::string owner;
	std::string group;
	int space2;
	int size_length;
	off_t size;
	std::string month;
	int space3;
	int day;
	int space4;
	int hours;
	int minutes;
	int space6;
	const char* name;
	int space5;
	int iteration;
	bool hidden;
	bool directory;
	bool executable;
	void clear(){
		this->permissions.clear();
		this->space1 = 0;
		this->hard_links = 0;
		this->link_length = 0;
		this->owner.clear();
		this->group.clear();
		this->space2 = 0;
		this->size_length = 0;
		this->size = 0;
		this->month.clear();
		this->space3 = 0;
		this->day = 0;
		this->hours = 0;
		this->minutes = 0;
		this->name = NULL;
		this->hidden = false;
		this->directory = false;
		this->executable = false;
		this->space5 = 0;
		p.pop_dir();
	}
	int name_size(){
		int i=0;
		for(;this->name[i]!='\0';++i){}
		return i;
	}
			
	void assign(dirent *drt){
		p.push_dir(drt->d_name);
		const char* c = drt->d_name;
		const char* y = p.get_path();
		struct stat s;
		int a = stat(y, &s);
		if(a == -1)perror("stat");
		if(s.st_mode & S_IFDIR){
			this->permissions += 'd';
			this->directory = true;
			if(!is_dot(c)){
				v2.push_back(c);
				++v2_size;
			}
		}
		else{
			this->permissions += '-';
			this->directory = false;
		}
		if(s.st_mode & S_IRUSR) this->permissions += 'r';
		else this->permissions += '-';
		if(s.st_mode & S_IWUSR) this->permissions += 'w';
		else this->permissions += '-';
		if(s.st_mode & S_IXUSR) this->permissions += 'x';
		else this->permissions += '-';
		if(s.st_mode & S_IRGRP) this->permissions += 'r';
		else this->permissions += '-';
		if(s.st_mode & S_IWGRP) this->permissions += 'w';
		else this->permissions += '-';
		if(s.st_mode & S_IXGRP) this->permissions += 'x';
		else this->permissions += '-';
		if(s.st_mode & S_IROTH) this->permissions += 'r';
		else this->permissions += '-';
		if(s.st_mode & S_IWOTH) this->permissions += 'w';
		else this->permissions += '-';
		if(s.st_mode & S_IXOTH) this->permissions += 'x';
		else this->permissions += '-';
		
		this->hard_links = s.st_nlink;
		if(this->hard_links >= 1000) this->link_length = 4;
		else if(this->hard_links >= 100) this->link_length = 3;
		else if(this->hard_links >= 10) this->link_length = 2;
		else this->link_length = 1;

		struct passwd *pw = getpwuid(s.st_uid);
		if(pw == NULL) perror("getpwuid");
		struct group *gr = getgrgid(s.st_gid);
		if(gr == NULL) perror("getgrgid");
		this->owner = pw->pw_name;
		this->group = gr->gr_name;

		this->size = s.st_size;
		if(this->size >= 1000000) this->size_length = 7;
		else if(this->size >= 100000) this->size_length = 6;
		else if(this->size >= 10000) this->size_length = 5;
		else if(this->size >= 1000) this->size_length = 4;
		else if(this->size >= 100) this->size_length = 3;
		else if(this->size >= 10) this->size_length = 2;
		else this->size_length = 1;
		
		const time_t *b = &s.st_mtime;
		struct tm *t = localtime(b);
		if(t == NULL) perror("localtime");
		if(t->tm_mon == 0) this->month = "Jan";
		else if(t->tm_mon == 1) this->month = "Feb";
		else if(t->tm_mon == 2) this->month = "Mar";
		else if(t->tm_mon == 3) this->month = "Apr";
		else if(t->tm_mon == 4) this->month = "May";
		else if(t->tm_mon == 5) this->month = "Jun";
		else if(t->tm_mon == 6) this->month = "Jul";
		else if(t->tm_mon == 7) this->month = "Aug";
		else if(t->tm_mon == 8) this->month = "Sep";
		else if(t->tm_mon == 9) this->month = "Oct";
		else if(t->tm_mon == 10) this->month = "Nov";
		if(t->tm_mon == 11) this->month = "Dec";

		this->day = t->tm_mday;
		if(t->tm_mday >=10) this->space3 = 0;
		else this->space3 = 1;

		if(t->tm_hour >= 10) this->space4 = 0;
		else this->space4 = 1;

		if(t->tm_min >10) this->space6 = 0;
		else this->space6 = 1;

		this->hours = t->tm_hour;
		this->minutes = t->tm_min;

		if(c[0] == '.')this->hidden = true;
		else this->hidden = false;
		if(s.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))this->executable = true;
		else this->executable = false;
		this->name = c;
		this->space5 = 0;
	}
};
//======================================================================
struct mydir {
	std::vector<myfile> v;
	std::vector<std::string>v1;
	std::vector<std::string>v2;
	std::vector<std::string>v3;
	std::vector<std::string>v4;
	int link_length;
	int size_length;
	void format_iters(){
		int s1 = 0;
		int s2 = 0;
		int s3 = 0;
		int s4 = 0;
		for(unsigned i=0;i<this->v.size();++i){
			if(this->v.at(i).iteration == 0)
				if(s1 < this->v.at(i).name_size()) s1 = this->v.at(i).name_size();
			if(this->v.at(i).iteration == 1)
				if(s2 < this->v.at(i).name_size()) s2 = this->v.at(i).name_size();
			if(this->v.at(i).iteration == 2)
				if(s3 < this->v.at(i).name_size()) s3 = this->v.at(i).name_size();
			if(this->v.at(i).iteration == 3)
				if(s4 < this->v.at(i).name_size()) s4 = this->v.at(i).name_size();
		}
		for(unsigned i=0;i<this->v.size();++i){
			if(this->v.at(i).iteration == 0)
				if(s1 > this->v.at(i).name_size())
					this->v.at(i).space5 = s1 - this->v.at(i).name_size();
			if(this->v.at(i).iteration == 1)
				if(s2 > this->v.at(i).name_size())
					this->v.at(i).space5 = s2 - this->v.at(i).name_size();
			if(this->v.at(i).iteration == 2)
				if(s3 > this->v.at(i).name_size())
					this->v.at(i).space5 = s3 - this->v.at(i).name_size();
			if(this->v.at(i).iteration == 3)
				if(s4 > this->v.at(i).name_size())
					this->v.at(i).space5 = s4 - this->v.at(i).name_size();
		}
	}
	void clear(){
		this->v.clear();
		this->link_length = 0;
		this->size_length = 0;
	}
	void get_lengths(){
		this->link_length = 0;
		this->size_length = 0;
		for(unsigned i=0;i<this->v.size();++i){
			if(this->v.at(i).link_length > this->link_length)
				this->link_length = this->v.at(i).link_length;
			if(this->v.at(i).size_length > this->size_length)
				this->size_length = this->v.at(i).size_length;
		}
	}
	void get_iters(){
		for(unsigned i=0;i<this->v.size();++i){
			if(!f.a){
				if(this->v.at(i).hidden);
				else{
					this->v.at(i).iteration = it;
					++it;
				}
			}
			else{
				this->v.at(i).iteration = it;
				++it;
			}
			if(it > 3) it = 0;
		}
	}
	void sort_files(){
		for(unsigned i=0;i<this->v.size();++i){
			for(unsigned j=i;j<this->v.size();++j){
				if(strcmp(this->v.at(i).name,this->v.at(j).name) > 0)
					std::swap(this->v.at(i),this->v.at(j));
			}
		}
		this->get_iters();
	}
	void format_files(){
		for(unsigned i=0;i<this->v.size();++i){
			if(this->v.at(i).link_length < this->link_length){
				this->v.at(i).space1 = this->link_length -
				this->v.at(i).link_length;
			}
			else this->v.at(i).space1 = 0;
			if(this->v.at(i).size_length < this->size_length){
				this->v.at(i).space2 = this->size_length -
				this->v.at(i).size_length;
			}
			else this->v.at(i).space2 = 0;
		}
	}
	void print_file(unsigned i){
		
		if(this->v.at(i).directory){
			std::cout << "\033[1;34m";
		}
		else if(this->v.at(i).executable){
			std::cout << "\033[1;32m";
		}
		if(this->v.at(i).hidden){
			std::cout << "\033[40m";
		}
		
		std::cout << this->v.at(i).name;
		std::cout << "\033[0m";
		if(this->v.at(i).iteration == 0){
			for(int j=0;j<this->v.at(i).space5;++j){
				std::cout << ' ';
			}
			std::cout << "  ";
		}
		if(this->v.at(i).iteration == 1){
			for(int k=0;k<this->v.at(i).space5;++k){
				std::cout << ' ';
			}
			std::cout << "  ";
		}
		if(this->v.at(i).iteration == 2){
			for(int l=0;l<this->v.at(i).space5;++l){
				std::cout << ' ';
			}
			std::cout << "  ";
		}
		if(this->v.at(i).iteration == 3){
			for(int m=0;m<this->v.at(i).space5;++m){
				std::cout << ' ';
			}
			std::cout << '\n';
		}
	}
	void print_file_l(unsigned i){
		std::cout << this->v.at(i).permissions
		<< ' ';
		for(int a=0;a<this->v.at(i).space1;++a){
			std::cout << ' ';
		}
		std::cout << this->v.at(i).hard_links
		<< ' '
		<< this->v.at(i).owner
		<< ' '
		<< this->v.at(i).group
		<< ' ';
		for(int b=0;b<this->v.at(i).space2;++b){
			std::cout << ' ';
		}
		std::cout << this->v.at(i).size
		<< ' '
		<< this->v.at(i).month
		<< ' ';
		for(int k=0;k<this->v.at(i).space3;++k){
			std::cout << ' ';
		}
		std::cout << this->v.at(i).day
		<< ' ';
		for(int l=0;l<this->v.at(i).space4;++l){
			std::cout << '0';
		}
		std::cout << this->v.at(i).hours
		<< ':'
		<< this->v.at(i).minutes;
		for(int m=0;m<this->v.at(i).space6;++m){
			std::cout << '0';
		}
		std::cout << ' ';
		if(this->v.at(i).directory){
			std::cout << "\033[1;34m";
		}
		else if(this->v.at(i).executable){
			std::cout << "\033[1;32m";
		}
		if(this->v.at(i).hidden){
			std::cout << "\033[40m";
		}
		std::cout << this->v.at(i).name;
		std::cout << "\033[0m"
		<< '\n';
	}
	void print_all(){
		this->format_iters();
		for(unsigned i=0;i<this->v.size();++i){
			if(!f.a){
				if(this->v.at(i).hidden == true);
				else{
					if(f.l) this->print_file_l(i);
					else this->print_file(i);
				}
			}
			else{
				if(f.l) this->print_file_l(i);
				else this->print_file(i);
			}
		}
		std::cout << '\n';
	}
};
//======================================================================
//======================================================================
DIR *my_opendir(const char *c){
	DIR *a = opendir(c);
	if(a == NULL) perror("my_opendir");
	return a;
}

int my_closedir(DIR *d){
	int ret = closedir(d);
	if(ret == -1) perror("my_closedir");
	return ret;
}
dirent *my_readdir(DIR *d){
	dirent *a = readdir(d);
	return a;
}
//======================================================================

//======================================================================
void exec_1(const char *c){
	if(c[0] == '.' && c[1] == '\0');
	else p.push_dir(c);
	DIR *dir = my_opendir(p.get_path());
	dirent *drt = my_readdir(dir);
	myfile a;
	mydir b;
	while(drt != NULL){
		a.assign(drt);
		b.v.push_back(a);
		drt = my_readdir(dir);
		a.clear();
	}
	my_closedir(dir);
	b.get_lengths();
	b.format_files();
	b.sort_files();
	if(v1.size() > 1) p.print();
	b.print_all();
	p.pop_dir();

}
void exec_0(const char *c){
	p.push_dir(c);
	DIR *dir = my_opendir(c);
	if(dir == NULL) return;
	dirent *drt = my_readdir(dir);
	myfile a;
	mydir b;
	while(drt != NULL){
		a.assign(drt);
		b.v.push_back(a);
		drt = my_readdir(dir);
		a.clear();
	}
	my_closedir(dir);
	b.get_lengths();
	b.format_files();
	b.sort_files();
	if(v1.size() > 1) p.print();
	b.print_all();
	if(f.R){
		while(!v2.empty()){
			const char* c1 = v2.at(0).c_str();
			v2.erase(v2.begin());
			exec_1(c1);
		}
	}

	p.pop_dir();
	b.clear();

}
	
//======================================================================


#endif
