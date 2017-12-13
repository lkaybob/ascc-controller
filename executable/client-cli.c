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
#include "client-cli.h"

#define BLE_ADDR "192.168.0.26"
#define KM_ENTER 128+8
#define KM_ROW   16

extern int tlcdfd;
extern int oledfd;
extern int dipswfd;

pthread_mutex_t thread_mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char **argv) {
   int km, buzzer;
   int authInfo;

   int i,j,k = 0;

   const char* denom = "Connection: close";

   char path[30] = {0};
   char value[100] = {0};
   char response[4096] = {0};
   char* jsonData;
   jsmn_parser parser;
   jsmntok_t tokens[100];
   int toklen;

   char filename[30] = {0};

   time_t rawtime;
   struct tm* timeinfo;

   timeinfo = localtime(&rawtime);
   
   // sevenSeg = open(SVNSEGDRV, O_RDWR);
   km = open(KMDRV, O_RDWR);
   oledfd = open(OLEDDRV, O_RDWR);
   tlcdfd = open(TLCDDRV, O_RDWR);
   // dotMatrix = open(DMDRV, O_RDWR);
   buzzer = open(BUZZDRV, O_RDWR);
   dipswfd = open(DIPSWDRV, O_RDWR);

   do {
      do{
         // system("./testApp/bitmap image/main.bmp");
         bleCommand(BLE_ADDR, 8080, "AT+NAMEIDLE", response);
         clearBitmap();
         setScreen("image/main.bmp", 0, 0);
         initializeTlcd(0);
         oledInit();
   
         // Main code goes here
         printf("Please enter your credential: ");
         bleCommand(BLE_ADDR, 8080, "AT+RESET", response);

         authInfo  = keyMatrixInput(km, buzzer);
         // system("./testApp/bitmap testApp/main-loading.bmp");
         // printf("Passed?\n");
         setScreen("image/main-loading.bmp", 0, 0);
         // printf("Passed!\n");
         sprintf(path, AUTH_PATH, authInfo);
         apiRequest(HOST, PORT, path, METHOD_GET, response);

         jsonData = strstr(response, denom);
         // printf("JSON Data: %x\n", jsonData);
   
         if(jsonData != NULL) {
            jsonData += strlen(denom);
            
            jsmn_init(&parser);
            toklen = jsmn_parse(&parser, jsonData, strlen(jsonData), tokens, 100);

            printf("Raw Response: %s\n", jsonData);

            // processJson(jsonData, tokens, toklen, "si_type", value);
            // printf("User Type: %s\n", value);

            // processJson(jsonData, tokens, toklen, "ci_code", value);
            // printf("Lecture Code: %s\n", value);

            // processJson(jsonData, tokens, toklen, "si_name", value);
            // printf("User Name: %s\n", value);
         } 
      
         if(authInfo == 1) {
            int dotMatrixValue = 31;
            int buzzerOff = 0;
            char* spacePos = value;
            char* space = " ";

            // system("./testApp/bitmap testApp/lecture-info.bmp");
            processJson(jsonData, tokens, toklen, "ci_code", value);
            printf("Class Code: %s\n", value);
            value[(strlen(value) - 1)] = '\0';
            sprintf(filename, "image/%s-ready.bmp", value);
            setScreen(filename, 0, 0);

            printf("Logged In\n");
            // write(buzzer, &dotMatrixValue, 4);
            // usleep(3000000);
            // write(buzzer, &buzzerOff, 4);
            
            writeTlcd(1, "Logged_in_as:");

            processJson(jsonData, tokens, toklen, "si_name", value);
            printf("User Name: %s\n", value);

            spacePos = strstr(spacePos, space);

            do {
               *spacePos = '_';
               printf("Changed: %s\n", value);
               spacePos = strstr(spacePos, space);
            }while(spacePos != NULL);

            value[(strlen(value) - 1)] = '\0';

            writeTlcd(2, value);
            // bleCommand("192.168.0.26", 8080, "AT+NAMEPAL309", response);

            sprintf(filename, "image/%s.img", value);
            oledLoadImage(filename);
            // setScreen("image/title.bmp", 38, 1196);

            sevenSegWorker(1);
            dotMatrixWorker(0);

            usleep(5000000);
            break;
         }
         else {
            printf("Wrong credential. Check your credential again\n");
         }
      }while(1);
      
      i = 0;

      do {
         j = checkLectureProgress();
         dotMatrixChanger(k++);
         

         if(j == 0) {
            bleCommand(BLE_ADDR, 8080, "AT+NAMEPAL309", response);

            printf("Lecture Started\n");
            processJson(jsonData, tokens, toklen, "ci_code", value);
            printf("Class Code: %s\n", value);
            value[(strlen(value) - 1)] = '\0';
            sprintf(filename, "image/%s-ing.bmp", value);

            setScreen(filename, 0, 0);
            startLectureBuzzer(buzzer);
            bleCommand(BLE_ADDR, 8080, "AT+RESET", response);
            break;
         }

         usleep(1000000);
      }while(1);

      do {
         j = checkLectureProgress();

         if(j == 1) {
            printf("Lecture Finished\n");
            // bleCommand("192.168.0.26", 8080, "AT+NAMEIDLE", response);
         break;
         } else {
            int inClassNum = 0;
            sprintf(path, IN_CLRM_NUM);
            apiRequest(HOST, PORT, path, METHOD_GET, response);
            jsonData = strstr(response, denom);
            jsonData += strlen(denom);
            printf("Response: %s\n", jsonData);
            
            jsmn_init(&parser);
            toklen = jsmn_parse(&parser, jsonData, strlen(jsonData), tokens, 10);

            processJson(jsonData, tokens, toklen, "count", value);
            printf("InClass?: %s\n", value);
            inClassNum = atoi(value);
            // dotMatrixChanger(inClassNum);
            dotMatrixChanger(k++);
         }
         
         usleep(1000000);
      }while(1);

      sevenSegCancel();
      dotMatrixCancel();
      // Main code end


   }while(1);

//   close(sevenSeg);
   close(km);
   close(oledfd);
   close(tlcdfd);
//   close(dotMatrix);

}

int processJson(char* original, jsmntok_t t[], int len, char* key, char value[]) {
int i;
for (i = 1; i < len; i++) {
   if (jsoneq(original, &t[i], key) == 0) {
         // printf("Value Found: %.*s\n", t[i+1].end-t[i+1].start, original + t[i+1].start);
         sprintf(value, "%.*s\n", t[i+1].end-t[i+1].start, original + t[i+1].start);
      } 
   }
   return -1;
}

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
        if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
                        strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
                return 0;
        }
        return -1;
}

void startLectureBuzzer(int fd) {
	int values[10] = {17, 15, 17, 15, 17, 12, 15, 13, 10, 0};
	int i;

	for (i = 0; i < 10; i++) {
		write(fd, values+i, 4);
		usleep(300000);
	}
}
