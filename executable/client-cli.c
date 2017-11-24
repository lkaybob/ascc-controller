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

#define SVNSEGDRV "/dev/7segdrv"
#define KMDRV	"/dev/kmsw"
#define OLEDDRV "/dev/cnoled"
#define TLCDDRV "/dev/cntlcd"

#define KM_ENTER 128+8
#define KM_ROW	16

int keyMatrixInput(int fd);

int main(int argc, char **argv) {
	int sevenSeg, km, oled, tlcd;
	int authInfo;

	char authPath[30] = {0};
	char response[2048] = {0};
	
	sevenSeg = open(SVNSEGDRV, O_RDWR);
	km = open(KMDRV, O_RDWR);
	oled = open(OLEDDRV, O_RDWR);
	tlcd = open(TLCDDRV, O_RDWR);

	fd = tlcd;
	
	// Main code goes here
//	do {
		printf("Please enter your credential: ");
		authInfo  = keyMatrixInput(km);
		sprintf(authPath, AUTH_PATH, authInfo);
		apiRequest(HOST, PORT, authPath, METHOD_GET);
	
		if(authInfo == 123412341) {
			printf("Welcome Professor Ko!\n");
		//	clearScreen(0);
		//	printf("Cleared\n");
		//	usleep(2000);
		//	displayMode(1, 1, 1);
		//	printf("Display Mode Set\n");
		//	usleep(2000);
		//	setDDRAMAddr(1, 15);
		//	printf("DDRAM Set\n");
		//	usleep(2000);
		//	setDDRAMAddr(15, 1);
		//	printf("DDRAM2 SEt\n");
		//	usleep(2000);
		//	writeStr("Logged in as:");
		//	printf("String 1 Set");
		//	usleep(2000);
		//	setDDRAMAddr(15, 2);
		//	printf("DDRAM3 Set\n");
		//	usleep(2000);
		//	writeStr("Ko, Young Bae");
		//	printf("String 2 Set");
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

}
