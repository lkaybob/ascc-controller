#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0
#define PATH "/dev/dipsw"
#define SWON 0x8000

int dipswfd;

int checkLectureProgress() {
	int retvalue;

	read(dipswfd, &retvalue, 4);
	// printf("[dipsw] %d\n", retvalue);

	if(retvalue == SWON)
		return TRUE;
	else
		return FALSE;	
}


