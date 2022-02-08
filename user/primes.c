/*
* @Author:  zihaowang
* @Email:   zihao.wang1@ucdconnect.ie
* @Website: www.wangzihao.org
* @Date:    2022-02-02 08:33:46
* @Last Modified by:   zihaowang
* @Last Modified time: 2022-02-03 11:15:37
* @Description:
* 	MIT 6.828: Lab util - task primes, https://pdos.csail.mit.edu/6.828/2021/labs/util.html
*
*	Programed to fetch primes numbers between 2 and 35
*	on a basis of concurrent pipeline.
*
*	See https://swtch.com/~rsc/thread/ for the idea of the pipeline.
*	
*/

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


// 创建子进程(right entry), 用以接收父进程(left entry)的输入
int add_child(int left_neighbor_fd) {
	int prime, flag;
	int left_neighbor_input;
	// 新entry接收的第一个输入必为prime number
	read(left_neighbor_fd, &prime, 4);
	printf("prime %d\n", prime);

	flag = 0;
	int p[2];
	pipe(p);

	// 持续读取直到写入端关闭
	while(read(left_neighbor_fd, &left_neighbor_input, 4)) {
		if(left_neighbor_input % prime != 0) {
			if(flag) 
				write(p[1], &left_neighbor_input, 4);
			
			else {
				flag = 1;

				int pid = fork();
				if(pid == 0) {
					close(p[1]);
					add_child(p[0]);
				}
				else {
					close(p[0]);
					write(p[1], &left_neighbor_input, 4);
				}
			}
		}
	}
	// 为父/子进程关闭读取及写入端
	close(left_neighbor_fd);
	close(p[1]);

	wait(0);
	// 等待所有子进程结束再退出，否则会产生孤儿进程
	exit(0);
	
}

int main(int argc, char const *argv[]) {

	int p[2];
	pipe(p);

	int pid = fork();
	if(pid == 0) {
		close(p[1]);
		add_child(p[0]);
	}
	else {
		int i;
		close(p[0]);
		for(i = 2; i <= 35; i++) 
			write(p[1], &i, 4);
		close(p[1]);
		wait(0);
	}

	exit(0);
}