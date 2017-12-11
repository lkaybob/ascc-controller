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

#define KM_ENTER	128+8
#define KM_CANCEL	128+4
#define KM_CLEAR	128+1
#define KM_ZERO		128+4
#define KM_ROW		16

int keyMatrixInput(int fd) {
	int data = 0;
	int result = 0;
	
	printf("KMI In!\n");
	do {
		int col;
		int row;
		int input;

		read(fd, &data, 2);
		col = data / KM_ROW;
		row = data % KM_ROW;

		if(col == 4) 
			col = 3;
		
		if(row == 4)
			row = 3;
		else if(row == 8)
			row = 4;
		
		switch(data) {
			case KM_CLEAR:
				result = 0;
				break;
			case KM_CANCEL:
				result /= 10;
				break;
			default:
		                if(data != 0 && data != KM_ENTER) {
                		        if(data != KM_ZERO)
                                		input = (row - 1) * 3 + col;
		                        result *= 10;
		                        result += input;
		                }
				break;

		}
		usleep(700000);

	
	} while(data != KM_ENTER);
	
	printf("Password : %d\n", result);

	return result;
}

