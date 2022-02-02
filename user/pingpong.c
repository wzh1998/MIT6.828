/*
* @Author:  zihaowang
* @Email:   zihao.wang1@ucdconnect.ie
* @Website: www.wangzihao.org
* @Date:    2022-01-31 13:53:02
* @Last Modified by:   zihaowang
* @Last Modified time: 2022-02-02 08:36:42
*/
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
	/*
	 * p_parent represent parent sending, child receiving
	 * p_child represent child sending, parent receiving
	 * note: 
	 * 		p[0] always represents the reading end
	 * 		p[1] always represents the writing end
	 */
	int p_parent[2];
	int p_child[2];

	pipe(p_parent);
	pipe(p_child);

	char buf[4];

	int pid = fork();

	if(pid == 0) { // child
		// 读取前关闭不必要的（写入方向的）file descriptor
		close(p_parent[1]);
		read(p_parent[0], buf, 4);
		close(p_parent[0]);

		printf("%d: received %s\n", pid, buf);

		// 同理
		close(p_child[0]);
		write(p_child[1], "pong", 4);
		close(p_child[1]);
	}

	else { // parent
		// 写入前关闭不必要的（读取方向的）file descriptor
		close(p_parent[0]);
		write(p_parent[1], "ping", 4);
		close(p_parent[1]);

		//同理
		close(p_child[1]);
		read(p_child[0], buf, 4);
		close(p_child[0]);

		printf("%d: received %s\n", pid, buf);
	}
	
	exit(0);
}