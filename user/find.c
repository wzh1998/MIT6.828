/*
* @Author:  zihaowang
* @Email:   zihao.wang1@ucdconnect.ie
* @Website: www.wangzihao.org
* @Date:    2022-01-31 09:21:40
* @Last Modified by:   zihaowang
* @Last Modified time: 2022-02-04 10:00:30
* @Description:
* 	MIT 6.828: Lab util - task find, https://pdos.csail.mit.edu/6.828/2021/labs/util.html
* 	
*	Programed to (recursively) find specific files under given directory.
*/

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char* fmtname_no_padding(char *path) {
	char *p;

	// p指向全局路径最后一个'/'的后一位，即文件名的第一位
	for(p=path+strlen(path); p >= path && *p != '/'; p--)
	;
	p++;

	return p;
}

void find(char *path, char *target_fn) {
	int fd;
	struct dirent de;
	struct stat st;

	if((fd = open(path, 0)) < 0){
		printf("find: cannot open %s\n", path);
		return;
	}

	if(fstat(fd, &st) < 0){
		printf("find: cannot stat %s\n", path);
		close(fd);
		return;
	}

	switch(st.type){
		// path为文件：检查文件名是否匹配
		case T_FILE:
			// 目标文件名匹配
			if(strcmp(fmtname_no_padding(path), target_fn) == 0)
				printf("%s\n", path);
			break;

		// path为路径：递归检查其下所有文件/文件夹
		case T_DIR:
			// 遍历该路径下所有的文件/文件夹以寻找目标文件
			while(read(fd, &de, sizeof(de)) == sizeof(de)){
				// 注意：'.'及'..'包含在当前路径，需排除以避免无限递归
				if(de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) 
					continue;
				// 组装该子目录下所有文件及文件夹的全局路径
				char *new_path = malloc(strlen(path) + 1 + strlen(de.name));
				strcpy(new_path, path);
				strcpy(new_path + strlen(path), "/");
				strcpy(new_path + strlen(path) + 1, de.name);

				// 递归
				find(new_path, target_fn);
				// 释放空间
				free(new_path);
			}
			break;
	}
	close(fd);
}


int main(int argc, char *argv[]) {
	if(argc < 3) {
		printf("Usage: find <dir> <fn>");
		exit(-1);
	}

	int i = 0;
	for(i = 2; i < argc; i++) 
		find(argv[1], argv[i]);
	
	exit(0);
}