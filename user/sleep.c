/*
* @Author:  zihaowang
* @Email:   zihao.wang1@ucdconnect.ie
* @Website: www.wangzihao.org
* @Date:    2022-01-29 11:45:34
* @Last Modified by:   zihaowang
* @Last Modified time: 2022-02-02 08:36:10
*/

#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
	if(argc != 2) {
		printf("Usage: sleep <time>\n");
		exit(-1);
	}
	
	int period = atoi(argv[1]);
	if(period <= 0) {
		printf("Invalid period, has to be bigger than 0\n");
		exit(-2);
	}

	sleep(period);

	exit(0);
}