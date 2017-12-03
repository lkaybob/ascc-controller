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
#include <time.h>

#include "requestModule.h"
#include "tlcdWrapper.h"
#include "oledWrapper.h"
#include "dotMatrixWorker.h"
#include "7segWorker.h"
#include "jsmn.h"

#define SVNSEGDRV "/dev/7segdrv"
#define KMDRV	"/dev/kmsw"
#define OLEDDRV "/dev/cnoled"
#define TLCDDRV "/dev/cntlcd"
#define DMDRV	"/dev/dmsw"
#define BUZZDRV	"/dev/cnbuzzer"

#define KM_ENTER 128+8
#define KM_ROW	16

extern int tlcdfd;
extern int oledfd;

int keyMatrixInput(int fd);
int processJson(char* original, jsmntok_t t[], int len, char* key);
void printJsonValue(char* original, jsmntok_t t[], int i);
static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
        if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
                        strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
                return 0;
        }
        return -1;
}

int main(int argc, char **argv) {
	int sevenSeg, km, dotMatrix, buzzer;
	int authInfo;

	int i,j,k;

	const char* denom = "Connection: close";

	char authPath[30] = {0};
	char response[4096] = {0};
	char* jsonData;
	jsmn_parser parser;
	jsmntok_t tokens[100];
	int toklen;

	time_t rawtime;
	struct tm* timeinfo;

	timeinfo = localtime(&rawtime);
	
//	sevenSeg = open(SVNSEGDRV, O_RDWR);
	km = open(KMDRV, O_RDWR);
	oledfd = open(OLEDDRV, O_RDWR);
	tlcdfd = open(TLCDDRV, O_RDWR);
//	dotMatrix = open(DMDRV, O_RDWR);
	buzzer = open(BUZZDRV, O_RDWR);

	jsmn_init(&parser);

	system("./testApp/bitmap testApp/main.bmp");
	initializeTlcd(0);
	// Main code goes here
	printf("Please enter your credential: ");
	authInfo  = keyMatrixInput(km);
	system("./testApp/bitmap testApp/main-loading.bmp");
	sprintf(authPath, AUTH_PATH, authInfo);
	apiRequest(HOST, PORT, authPath, METHOD_GET, response);

	jsonData = strstr(response, denom);
	
	if(jsonData != NULL) {
		jsonData += strlen(denom);

		toklen = jsmn_parse(&parser, jsonData, strlen(jsonData), tokens, 100);
		processJson(jsonData, tokens, toklen, "si_name");
		processJson(jsonData, tokens, toklen, "si_type");
		processJson(jsonData, tokens, toklen, "si_software");
		processJson(jsonData, tokens, toklen, "ci_name");
		processJson(jsonData, tokens, toklen, "ci_code");
	}
		
	if(authInfo == 4) {
		int dotMatrixValue = 31;
		int buzzerOff = 0;
		system("./testApp/bitmap testApp/lecture-info.bmp");
		printf("Welcome Professor Ko!\n");
		// write(buzzer, &dotMatrixValue, 4);
		// usleep(3000000);
		// write(buzzer, &buzzerOff, 4);
		
		oledInit();
		writeTlcd(1, "Logged_in_as:");
		writeTlcd(2, "Ko,_Young_Bae");
		bleCommand("192.168.0.26", 8080, "123123/F066", response);
		dotMatrixWorker(0);
		oledLoadImage("youngko.img");
		sevenSegWorker(1);

		usleep(5000000);
		// TODO : TLCD, 7 Segment Initialize
	}
	else {
		printf("Wrong credential. Check your credential again\n");
	}

	i = 0;

	do {
	 	usleep(1000000);
		i++;
	}while(i<10);

	// Main code end

	dotMatrixCancel();
	sevenSegCancel();

//	close(sevenSeg);
	close(km);
	close(oledfd);
	close(tlcdfd);
//	close(dotMatrix);

}
int processJson(char* original, jsmntok_t t[], int len, char* key) {
int i;
for (i = 1; i < len; i++) {
	if (jsoneq(original, &t[i], key) == 0) {
			/* We may use strndup() to fetch string value */
//			return i;
			printf("- %s: %.*s\n", key, t[i+1].end-t[i+1].start,
					original + t[i+1].start);
		} 
	}
	return -1;
}

void printJsonValue(char* original, jsmntok_t t[], int i) {
	printf("%.s", t[i+1].end - t[i+1].start, original + t[i+1].start);
}
