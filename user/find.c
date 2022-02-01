#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char* fmtname_no_padding(char *path) {
	char *p;

	// Find first character after last slash.
	for(p=path+strlen(path); p >= path && *p != '/'; p--)
	;
	p++;

	// Return no-padding name.
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
	// printf("st.type = %d\n", st.type);
	// printf("T_DIR = %d\n", T_DIR);
	switch(st.type){
		// 类型为文件：检查文件名是否匹配
		
		case T_FILE:
			// 目标文件名匹配
			// printf("file: %s\t", fmtname_no_padding(path));
			// printf("compare: %d\n", strcmp(fmtname_no_padding(path), target_fn));
			if(strcmp(fmtname_no_padding(path), target_fn) == 0)
				printf("%s\n", path);
			break;

		// 类型为路径：Recursion
		case T_DIR:
			// 遍历该路径下所有的文件及文件夹以寻找目标文件
			while(read(fd, &de, sizeof(de)) == sizeof(de)){
				if(de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) 
					continue;
				// 组装该子目录下所有文件及文件夹的全局路径
				char *new_path = malloc(strlen(path) + 1 + strlen(de.name));
				// printf("new len: %d\n", strlen(path) + 1 + strlen(de.name));
				// printf("de.name: %s\n", de.name);
				strcpy(new_path, path);
				strcpy(new_path + strlen(path), "/");
				strcpy(new_path + strlen(path) + 1, de.name);
				// printf("%s\n", new_path);
				// exit(-2);
				// recursion
				find(new_path, target_fn);
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
	// printf("argv 0: %s\n", argv[0]);
	// printf("argv 1: %s\n", argv[1]);
	// printf("argv 2: %s\n", argv[2]);
	// int i;
	// for(i = 2; i < argc; i++) 
	find(argv[1], argv[2]);
	
	exit(0);
}