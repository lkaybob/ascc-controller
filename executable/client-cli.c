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
#include <pthread.h>

#include "requestModule.h"
#include "tlcdWrapper.h"
#include "oledWrapper.h"
#include "dotMatrixWorker.h"
#include "7segWorker.h"
#include "dipswitch.h"
#include "jsmn.h"
#include "bitmapRuntime.h"

#define SVNSEGDRV "/dev/7segdrv"
#define KMDRV   "/dev/kmsw"
#define OLEDDRV "/dev/cnoled"
#define TLCDDRV "/dev/cntlcd"
#define DMDRV   "/dev/dmsw"
#define BUZZDRV   "/dev/cnbuzzer"
#define DIPSWDRV "/dev/dipsw"

#define KM_ENTER 128+8
#define KM_ROW   16

extern int tlcdfd;
extern int oledfd;
extern int dipswfd;

pthread_mutex_t thread_mutex = PTHREAD_MUTEX_INITIALIZER;

int keyMatrixInput(int fd);
void activateBuzzer(int fd);
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
   int km, buzzer;
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
   
//   sevenSeg = open(SVNSEGDRV, O_RDWR);
   km = open(KMDRV, O_RDWR);
   oledfd = open(OLEDDRV, O_RDWR);
   tlcdfd = open(TLCDDRV, O_RDWR);
//   dotMatrix = open(DMDRV, O_RDWR);
   buzzer = open(BUZZDRV, O_RDWR);
   dipswfd = open(DIPSWDRV, O_RDWR);
   

   jsmn_init(&parser);



   do {
	do{
      // system("./testApp/bitmap image/main.bmp");
		clearBitmap();
		setScreen("image/main.bmp", 0, 0);
      initializeTlcd(0);
      oledInit();
   
      // Main code goes here
      printf("Please enter your credential: ");
      authInfo  = keyMatrixInput(km);
      // system("./testApp/bitmap testApp/main-loading.bmp");
// printf("Passed?\n");
		setScreen("image/main-loading.bmp", 0, 0);
printf("Passed!\n");
      sprintf(authPath, AUTH_PATH, authInfo);
      apiRequest(HOST, PORT, authPath, METHOD_GET, response);

      jsonData = strstr(response, denom);
      // printf("JSON Data: %x\n", jsonData);
   
      if(jsonData != NULL) {
         jsonData += strlen(denom);

         toklen = jsmn_parse(&parser, jsonData, strlen(jsonData), tokens, 100);
         processJson(jsonData, tokens, toklen, "si_name");
         processJson(jsonData, tokens, toklen, "si_type");
         processJson(jsonData, tokens, toklen, "si_software");
         processJson(jsonData, tokens, toklen, "ci_name");
         processJson(jsonData, tokens, toklen, "ci_code");
      } 
      
      if(authInfo == 1) {
         int dotMatrixValue = 31;
         int buzzerOff = 0;
	// printf("Response: %s\n", response);
        // system("./testApp/bitmap testApp/lecture-info.bmp");
		setScreen("image/lecture-info.bmp", 0, 0);

        printf("Welcome Professor Ko!\n");
        // write(buzzer, &dotMatrixValue, 4);
        // usleep(3000000);
        // write(buzzer, &buzzerOff, 4);
         
         writeTlcd(1, "Logged_in_as:");
         writeTlcd(2, "Ko,_Young_Bae");
         // bleCommand("192.168.0.26", 8080, "123123/F066", response);
         oledLoadImage("youngko.img");

        sevenSegWorker(1);
   		dotMatrixWorker(0);

         usleep(5000000);
	break;
         // TODO : TLCD, 7 Segment Initialize
      }
      else {
         printf("Wrong credential. Check your credential again\n");
      }
	}while(1);
      i = 0;


      do {
         j = checkLectureProgress();
         if(j == 0) {
            printf("Lecture Started\n");
            setScreen("imgae/title.bmp", 38, 31);
			// activateBuzzer(buzzer);
         break;
         }

         usleep(1000000);
      }while(1);


      do {
         j = checkLectureProgress();
         if(j == 1) {
            printf("Lecture Finished\n");
         break;
         }
         
         usleep(1000000);
      }while(1);

      sevenSegCancel();
      dotMatrixCancel();
      // Main code end
   }while(1);
	printf("Canceled?\n");
   sevenSegCancel();
   dotMatrixWorker(0);


//   close(sevenSeg);
   close(km);
   close(oledfd);
   close(tlcdfd);
//   close(dotMatrix);

}

int processJson(char* original, jsmntok_t t[], int len, char* key) {
int i;
for (i = 1; i < len; i++) {
   if (jsoneq(original, &t[i], key) == 0) {
         /* We may use strndup() to fetch string value */
//         return i;
         printf("- %s: %.*s\n", key, t[i+1].end-t[i+1].start,
               original + t[i+1].start);
      } 
   }
   return -1;
}

void printJsonValue(char* original, jsmntok_t t[], int i) {
   printf("%.s", t[i+1].end - t[i+1].start, original + t[i+1].start);
}

void activateBuzzer(int fd) {
	int values[10] = {17, 15, 17, 15, 17, 12, 15, 13, 10, 0};
	int i;

	for (i = 0; i < 10; i++) {
		write(fd, values+i, 4);
		usleep(300000);
	}
}
