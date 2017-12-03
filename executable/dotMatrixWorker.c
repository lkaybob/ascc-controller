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

typedef struct {
	int fd;
	int value;
} dmInit;

dmInit dmInitInfo;
pthread_t tid;

void *dotMatrixThreadFunc(void *arg) {
	
	while(1) {
		write(dmInitInfo.fd, &dmInitInfo.value, 4);
	}

	pthread_exit(NULL);
}

pthread_t dotMatrixWorker(int number) {

	dmInitInfo.fd = open("/dev/dmsw", O_RDWR);
	dmInitInfo.value = number;
	pthread_create(&tid, NULL, dotMatrixThreadFunc, NULL);
	return tid;
}

void dotMatrixChanger(int number) {
	dmInitInfo.value = number;
}

void dotMatrixCancel(void) {
	close(dmInitInfo.fd);
	pthread_cancel(tid);
}
