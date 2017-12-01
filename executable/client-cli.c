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

#include "requestModule.h"
#include "tlcdWrapper.h"
#include "jsmn.h"

#define SVNSEGDRV "/dev/7segdrv"
#define KMDRV	"/dev/kmsw"
#define OLEDDRV "/dev/cnoled"
#define TLCDDRV "/dev/cntlcd"
#define DMDRV	"/dev/dmsw"
#define BUZZDRV	"/dev/cnbuzzer"

#define KM_ENTER 128+8
#define KM_ROW	16

extern int fd;

int keyMatrixInput(int fd);

int main(int argc, char **argv) {
	int sevenSeg, km, oled, tlcd, dotMatrix, buzzer;
	int authInfo;

	int i,j,k;

	char authPath[30] = {0};
	char response[4096] = {0};

	jsmn_parser parser;
	jsmntok_t tokens[20];
	
	sevenSeg = open(SVNSEGDRV, O_RDWR);
	km = open(KMDRV, O_RDWR);
	oled = open(OLEDDRV, O_RDWR);
	tlcd = open(TLCDDRV, O_RDWR);
	dotMatrix = open(DMDRV, O_RDWR);
	buzzer = open(BUZZDRV, O_RDWR);

	jsmn_init(&parser);

	fd = tlcd;
	printf("fd: %d\n", fd);	
	// Main code goes here
//	do {
		printf("Please enter your credential: ");
		authInfo  = keyMatrixInput(km);
		sprintf(authPath, AUTH_PATH, authInfo);
		// apiRequest(HOST, PORT, authPath, METHOD_GET, response);

		// printf("Response: %s\n", response);
/*
		jsmn_parse(&parser, response, strlen(response), tokens, 20);
	
		for(i = 0; i < 4; i++){
			printf("JSMN Token Check [%d] Start : %d, End: %d, Size: %d, Type: %d\n",
				i, tokens[i].start, tokens[i].end, tokens[i].size, tokens[i].type);
		}
*/
		if(authInfo == 1) {
			int dotMatrixValue = 31;
			int buzzerOff = 0;
			printf("Welcome Professor Ko!\n");
			write(buzzer, &dotMatrixValue, 4);
			usleep(3000000);
			write(buzzer, &buzzerOff, 4);
			
			initializeTlcd(0);
			writeTlcd(1, "Logged_in_as:");
			writeTlcd(2, "Ko,_Young_Bae");
			bleCommand("192.168.0.26", 8080, "123123/F066", response);

			while(1){
				write(dotMatrix, &dotMatrixValue, 4);
			}
			// TODO : TLCD, 7 Segment Initialize
		}
		else {
			printf("Wrong credential. Check your credential again\n");
		}
//	} while(authInfo == 0)

//	printf("No credential. Terminating program...\n\n");
	// Main code end

	close(sevenSeg);
	close(km);
	close(oled);
	close(tlcd);
	close(dotMatrix);

}
