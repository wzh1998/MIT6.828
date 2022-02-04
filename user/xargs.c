/*
* @Author:  zihaowang
* @Email:   zihao.wang1@ucdconnect.ie
* @Website: www.wangzihao.org
* @Date:    2022-02-04 08:24:39
* @Last Modified by:   zihaowang
* @Last Modified time: 2022-02-04 19:46:49
* @Description: 
*	MIT 6.828: Lab util - task xargs, https://pdos.csail.mit.edu/6.828/2021/labs/util.html
*
* 	This file implemented UNIX xargs tools, 
* 	see https://en.wikipedia.org/wiki/Xargs for more info.
*/

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define OUTPUT_MAXLEN 512

/* 
 * parse_stdout: 处理管道符('|')左侧的输出结果，每次调用处理一行。
 */
void parse_stdout(char *stdout) {	
	// 初始化动态数组空间
	memset(stdout, 0, 128);
	char *pt = stdout;

	// 从stdout读取管道符左侧命令的输出结果，每次处理一行，遇到换行符终止
	while(read(0, pt, 1) && *pt != '\n') 
		pt++;

	// 在末尾用'\0'封装处理好结果
	*pt = 0;

	return;
}

/*
 * parse_xargs: 将管道符左侧命令的输出结果拼接到右侧指令，利用子进程执行拼接好的命令。
 */
void parse_xargs(char *stdout, int argc, char *argv[]) {
	if(argc < 2) {
		printf("usage: xargs [-n number] [utility [argument ...]]\n");
		exit(-1);
	}
	// exec_file: xargs右侧的命令（不含参数）
	char *exec_file = argv[1];
	// arg_concat: xargs右侧的命令（含参数）
	char *arg_concat[argc + 1];
	memcpy(arg_concat, &argv[1], (argc - 1) * sizeof(char *));
	// 拼接&封装
	arg_concat[argc - 1] = stdout;
	arg_concat[argc] = 0;	
	
	int pid = fork();
	if(pid == 0) 
		exec(exec_file, arg_concat);
	else {
		wait(0);
		return;
	}
}

int main(int argc, char *argv[]) {

	char *output;
	output = malloc(OUTPUT_MAXLEN);
	// 逐行拼接并执行指令
	while(1) {
		parse_stdout(output);
		if(*output == 0) break;
		parse_xargs(output, argc, argv);
	}

	free(output);
	exit(0);
}