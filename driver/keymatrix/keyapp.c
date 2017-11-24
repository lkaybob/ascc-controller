#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv) {
	int fd;
	int retvalue;
	int i;
	int data = 0;
  	fd = open("/dev/kmsw", O_RDWR);
	if(fd < 0) {
		perror("Driver open error.\n");
		return 1;
	}
	for (;;) {
		read(fd, &data, 2);
		
		int col = data/16;
		if(col==4){
			col = 3;
		}
		if(col==8){
			col = 4;
		}
		int row = data%16;
		if(row==4){
			row = 3;
		}
		if(row==8){
			row = 4;
		}
		printf(" col : %d,row : %d\n",col, row);
		
		sleep(1);
	}
	close(fd);
	return retvalue;
}
