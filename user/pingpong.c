#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
	/*
	 * p_parent for parent sending, child receiving
	 * p_child for child sending, parent receiving
	 * remember: 
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
		// close the direction that will not be used before reading
		close(p_parent[1]);
		read(p_parent[0], buf, 4);
		close(p_parent[0]);

		printf("%d: received %s\n", pid, buf);

		// shut down unnecessary fd
		close(p_child[0]);
		write(p_child[1], "pong", 4);
		close(p_child[1]);
	}

	else { // parent
		// close the direction that will not be used before sending
		close(p_parent[0]);
		write(p_parent[1], "ping", 4);
		close(p_parent[1]);

		// shut down unnecessary fd
		close(p_child[1]);
		read(p_child[0], buf, 4);
		close(p_child[0]);

		printf("%d: received %s\n", pid, buf);
	}
	
	exit(0);
}