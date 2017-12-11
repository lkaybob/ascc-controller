#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "tlcdDefine.h"


int tlcdfd;

int IsBusy(void)
{
	unsigned short wdata, rdata;

//	printf("InBusy In: %d\n", fd);

	wdata = SIG_BIT_RW;
	write(tlcdfd ,&wdata,2);

	wdata = SIG_BIT_RW | SIG_BIT_E;
	write(tlcdfd ,&wdata,2);

	read(tlcdfd,&rdata ,2);

	wdata = SIG_BIT_RW;
	write(tlcdfd,&wdata,2);

	if (rdata &  BUSY_BIT)
		return TRUE;

	return FALSE;
}
int writeCmdTlcd(unsigned short cmd)
{
	unsigned short wdata ;

//	printf("writeCmdTlcd IN\n");
	if ( IsBusy())
		return FALSE;

	wdata = cmd;
	write(tlcdfd ,&wdata,2);

	wdata = cmd | SIG_BIT_E;
	write(tlcdfd ,&wdata,2);

	wdata = cmd ;
	write(tlcdfd ,&wdata,2);
//	printf("writeCmdTlcd END\n");
	return TRUE;
}

int setDDRAMAddr(int x , int y)
{
	unsigned short cmd = 0;
//	printf("x :%d , y:%d \n",x,y);
	if(IsBusy())
	{
		perror("setDDRAMAddr busy error.\n");
		return FALSE;

	}

	if ( y == 1 )
	{
		cmd = DDRAM_ADDR_LINE_1 +x;
	}
	else if(y == 2 )
	{
		cmd = DDRAM_ADDR_LINE_2 +x;
	}
	else
		return FALSE;

	if ( cmd >= 0x80)
		return FALSE;

	
//	printf("setDDRAMAddr w1 :0x%X\n",cmd);

	if (!writeCmdTlcd(cmd | SET_DDRAM_ADD_DEF))
	{
		perror("setDDRAMAddr error\n");
		return FALSE;
	}
//	printf("setDDRAMAddr w :0x%X\n",cmd|SET_DDRAM_ADD_DEF);
	usleep(1000);
	return TRUE;
}

int displayMode(int bCursor, int bCursorblink, int blcd  )
{
	unsigned short cmd  = 0;

	if ( bCursor)
	{
		cmd = DIS_CURSOR;
	}

	if (bCursorblink )
	{
		cmd |= DIS_CUR_BLINK;
	}

	if ( blcd )
	{
		cmd |= DIS_LCD;
	}

	if (!writeCmdTlcd(cmd | DIS_DEF))
		return FALSE;

	return TRUE;
}

int writeCh(unsigned short ch)
{
	unsigned short wdata =0;

	if ( IsBusy())
		return FALSE;

	wdata = SIG_BIT_RS | ch;
	write(tlcdfd ,&wdata,2);

	wdata = SIG_BIT_RS | ch | SIG_BIT_E;
	write(tlcdfd ,&wdata,2);

	wdata = SIG_BIT_RS | ch;
	write(tlcdfd ,&wdata,2);
	usleep(1000);
	return TRUE;

}


int setCursorMode(int bMove , int bRightDir)
{
	unsigned short cmd = MODE_SET_DEF;

	if (bMove)
		cmd |=  MODE_SET_SHIFT;

	if (bRightDir)
		cmd |= MODE_SET_DIR_RIGHT;

	if (!writeCmdTlcd(cmd))
		return FALSE;
	return TRUE;
}

int functionSet(void)
{
	unsigned short cmd = 0x0038; // 5*8 dot charater , 8bit interface , 2 line
	
//	printf("functionSet IN\n");
	if (!writeCmdTlcd(cmd))
		return FALSE;
	return TRUE;
}

int writeStr(char* str)
{
	unsigned char wdata;
	int i;
	for(i =0; i < strlen(str) ;i++ )
	{
		if (str[i] == '_')
			wdata = (unsigned char)' ';
		else
			wdata = str[i];
		writeCh(wdata);
	}
	return TRUE;

}

#define LINE_NUM			2
#define COLUMN_NUM			16			
int clearScreen(int nline)
{
	int i;
	if (nline == 0)
	{
		if(IsBusy())
		{	
			perror("clearScreen error\n");
			return FALSE;
		}
		if (!writeCmdTlcd(CLEAR_DISPLAY))
			return FALSE;
		return TRUE;
	}
	else if (nline == 1)
	{	
		setDDRAMAddr(0,1);
		for(i = 0; i <= COLUMN_NUM ;i++ )
		{
			writeCh((unsigned char)' ');
		}	
		setDDRAMAddr(0,1);

	}
	else if (nline == 2)
	{	
		setDDRAMAddr(0,2);
		for(i = 0; i <= COLUMN_NUM ;i++ )
		{
			writeCh((unsigned char)' ');
		}	
		setDDRAMAddr(0,2);
	}
	return TRUE;
}

void initializeTlcd(int line) {
	// [line] 0: All, 1: Line 1, 2: Line2
	const int bCursorOn = 0;
	const int bBlink = 0;
	// For clean screen, disable cursor!
	functionSet();
	usleep(2000);
	clearScreen(0);
	usleep(2000);
	displayMode(bCursorOn, bBlink, 1);
	setDDRAMAddr(1,15);
	usleep(2000);
}

void writeTlcd(int line, char* str) {
	const int nColumn = 0;
	setDDRAMAddr(nColumn, line);
	usleep(2000);
	writeStr(str);
	usleep(1000000);
}
