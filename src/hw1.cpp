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
#include <time.h>
#include <stdlib.h>
#include "ls.h"


int main(int argc, const char *argv[])
{
	v1.clear();
	p.push_dir(".");
	feed_v1(argv, argc);
	if(v1.size() == 0) v1.push_back(".");
	sort_v1();
	get_flags(argv, argc);
	for(unsigned i=0;i<v1.size();++i){
		exec_0(v1.at(i));
		if(i != v1.size() - 1) std::cout << '\n';
	}




	return 0;
}
