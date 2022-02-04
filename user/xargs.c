/*
* @Author:  zihaowang
* @Email:   zihao.wang1@ucdconnect.ie
* @Website: www.wangzihao.org
* @Date:    2022-02-04 08:24:39
* @Last Modified by:   zihaowang
* @Last Modified time: 2022-02-04 14:02:15
*/

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

char* parse_stdout() {
	// printf("stdin: ");
	char *stdout;
	stdout = malloc(128);
	memset(stdout, 0, 128);

	char *pt = stdout;

	// 在stdout读取左侧命令运行结果
	while(read(0, pt, 1) && *pt != '\n') 
		pt++;
	
	*pt = 0;

	// printf("stdin: %s\n", stdout);

	return stdout;
}

// additional args from stdout
void parse_xargs(char *stdout, int argc, char *argv[]) {
	char *exec_file = argv[1];
	
	char *arg_concat[argc + 1];
	// printf("sizeof argv: %d\n", argc * sizeof(char *));
	memcpy(arg_concat, &argv[1], (argc - 1) * sizeof(char *));
	arg_concat[argc - 1] = stdout;
	arg_concat[argc] = 0;	
	
	int pid = fork();
	if(pid == 0) {
		exec(exec_file, arg_concat);
		// close(0);
		
	}
	else {
		wait(0);
		return;
	}
}

int main(int argc, char *argv[]) {

	// parse input
	// close(1);
	char *output;
	while(1) {
		output = parse_stdout();
		if(*output == 0) break;
		parse_xargs(output, argc, argv);
	}

	exit(0);
}