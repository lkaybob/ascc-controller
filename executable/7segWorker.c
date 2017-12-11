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
#include <time.h>

typedef struct {
	int fd;
	int value;
} ssInit;

ssInit ssInitInfo;
pthread_t sstid;

void *sevenSegThreadFunc(void *arg) {
	time_t rawtime;
	struct tm* timeinfo;
	while(1) {
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		ssInitInfo.value = (timeinfo->tm_hour+9)%24 * 10000 + timeinfo->tm_min * 100 + timeinfo->tm_sec;
		write(ssInitInfo.fd, &ssInitInfo.value, 4);
	}

	pthread_exit(NULL);
}

pthread_t sevenSegWorker(int number) {

	ssInitInfo.fd = open("/dev/7segdrv", O_RDWR);
	ssInitInfo.value = number;
	pthread_create(&sstid, NULL, sevenSegThreadFunc, NULL);
	return sstid;
}

void sevenSegChanger(int number) {
	ssInitInfo.value = number;
}

void sevenSegCancel(void) {
	pthread_cancel(sstid);
        close(ssInitInfo.fd);
}
