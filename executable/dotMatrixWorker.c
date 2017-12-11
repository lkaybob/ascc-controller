#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h> 
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

extern pthread_mutex_t thread_mutex;

typedef struct {
	int fd;
	int value;
} dmInit;

dmInit dmInitInfo;
pthread_t dotid;

void *dotMatrixThreadFunc(void *arg) {
	
	while(1) {
		// pthread_mutex_lock(&thread_mutex);
		// printf("Locked @ DM\n");
		write(dmInitInfo.fd, &dmInitInfo.value, 4);
		// pthread_mutex_unlock(&thread_mutex);
		// printf("Unlocked @ DM\n");
	}

	pthread_exit(NULL);
}

pthread_t dotMatrixWorker(int number) {

	dmInitInfo.fd = open("/dev/dmsw", O_RDWR);
	dmInitInfo.value = number;
	pthread_create(&dotid, NULL, dotMatrixThreadFunc, NULL);
	return dotid;
}

void dotMatrixChanger(int number) {
	dmInitInfo.value = number;
}

void dotMatrixCancel(void) {
	pthread_cancel(dotid);
        close(dmInitInfo.fd);
}
