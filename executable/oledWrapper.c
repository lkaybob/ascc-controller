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
#include <pthread.h>

#define TRUE	1
#define FALSE	0

#define DRIVER_NAME		"/dev/cnoled"

int oledfd;

extern pthread_mutex_t thread_mutex;

unsigned long simple_strtoul(char *cp, char **endp,unsigned int base)
{
	unsigned long result = 0,value;
	
	if (*cp == '0') {
		cp++;
		if ((*cp == 'x') && isxdigit(cp[1])) {
			base = 16;
			cp++;
		}
		if (!base) {
			base = 8;
		}
	}
	if (!base) {
		base = 10;
	}
	while (isxdigit(*cp) && (value = isdigit(*cp) ? *cp-'0' : (islower(*cp)
								? toupper(*cp) : *cp)-'A'+10) < base) {
		result = result*base + value;
		cp++;
	}
	if (endp)
		*endp = (char *)cp;
	return result;
}

unsigned long read_hex(const char* str){
	char addr[128];
	strcpy(addr,str);
	return simple_strtoul(addr, NULL, 16);
}


// signal form 
//	12bit	11bit	10bit	9bit	8bit	7bit	6bit	5bit	4bit	3bit	2bit	1bit	0bit
//	RST#	CS#		D/C#	WD#		RD#		D7		D6		D5		D4		D3		D2		D1		D0
// trigger => WD or RD rising edge
/************************************************************************************************





************************************************************************************************/
#define RST_BIT_MASK	0xEFFF		
#define CS_BIT_MASK		0xF7FF
#define DC_BIT_MASK		0xFBFF
#define WD_BIT_MASK		0xFDFF
#define RD_BIT_MASK		0xFEFF
#define DEFAULT_MASK	0xFFFF


#define CMD_SET_COLUMN_ADDR		0x15
#define CMD_SET_ROW_ADDR		0x75
#define CMD_WRITE_RAM			0x5C
#define CMD_READ_RAM			0x5D
#define CMD_LOCK				0xFD

int reset(void)
{
	unsigned short wdata ;

	wdata = RST_BIT_MASK;
	write(oledfd,&wdata , 2 );
	usleep(2000);
	wdata = DEFAULT_MASK;
	write(oledfd,&wdata , 2 );
	return TRUE;
}

int writeCmdOled(int size , unsigned short* cmdArr)
{
	int i ;
	unsigned short wdata;

	//printf("wCmd : [0x%02X]",cmdArr[0]);
	//wdata = CS_BIT_MASK;
	//write(oledfd,&wdata,2);

	wdata = CS_BIT_MASK & DC_BIT_MASK;
	write(oledfd,&wdata,2);

	wdata = CS_BIT_MASK & DC_BIT_MASK & WD_BIT_MASK ;
	write(oledfd,&wdata,2);

	wdata = CS_BIT_MASK & DC_BIT_MASK & WD_BIT_MASK & (cmdArr[0]|0xFF00) ;
	write(oledfd,&wdata,2);
	
	wdata = CS_BIT_MASK & DC_BIT_MASK & (cmdArr[0] | 0xFF00) ;
	write(oledfd,&wdata,2);

	wdata = CS_BIT_MASK & ( cmdArr[0] | 0xFF00);
	write(oledfd,&wdata,2);

	for (i = 1; i < size ; i++ )
	{
	//	wdata = CS_BIT_MASK ;
	//	write(oledfd,&wdata,2);

	//	wdata = CS_BIT_MASK ;
	//	write(oledfd,&wdata,2);

		wdata = CS_BIT_MASK & WD_BIT_MASK ;
		write(oledfd,&wdata,2);

		wdata = CS_BIT_MASK & WD_BIT_MASK & (cmdArr[i] | 0xFF00) ;
		write(oledfd,&wdata,2);

		wdata = CS_BIT_MASK & (cmdArr[i] | 0xFF00);
		write(oledfd,&wdata,2);

	//	wdata = CS_BIT_MASK & (cmdArr[i] | 0xFF00);
	//	write(oledfd,&wdata,2);
	//	printf("[0x%02X]",cmdArr[i]);

	}
	wdata= DEFAULT_MASK;
	write(oledfd,&wdata,2);
	//printf("\n");
	return TRUE;
}

int writeData(int size , unsigned char* dataArr)
{
	int i ;
	unsigned short wdata;
	
	//wdata = CS_BIT_MASK;
	//write(oledfd,&wdata,2);

	wdata = CS_BIT_MASK & DC_BIT_MASK;
	write(oledfd,&wdata,2);

	//wdata = CS_BIT_MASK & DC_BIT_MASK & WD_BIT_MASK ;
	//write(oledfd,&wdata,2);

	wdata = CS_BIT_MASK & DC_BIT_MASK & WD_BIT_MASK & (CMD_WRITE_RAM | 0xFF00) ;
	write(oledfd,&wdata,2);

	wdata = CS_BIT_MASK & DC_BIT_MASK & (CMD_WRITE_RAM | 0xFF00);
	write(oledfd,&wdata,2);

	wdata = CS_BIT_MASK &  (CMD_WRITE_RAM | 0xFF00);
	write(oledfd,&wdata,2);

	for (i = 0; i < size ; i++ )
	{
		wdata = CS_BIT_MASK & WD_BIT_MASK ;
		write(oledfd,&wdata,2);

		wdata = CS_BIT_MASK & WD_BIT_MASK & ((unsigned char)dataArr[i] | 0xFF00 );
		write(oledfd,&wdata,2);

		wdata = CS_BIT_MASK & ( (unsigned char)dataArr[i] | 0xFF00);
		write(oledfd,&wdata,2);


	}
	wdata = DEFAULT_MASK;
	write(oledfd,&wdata,2);

	return TRUE;

}

int readData(int size , unsigned short* dataArr)
{

	int i ;
	unsigned short wdata;

	wdata = CS_BIT_MASK & DC_BIT_MASK;
	write(oledfd,&wdata,2);

	wdata = CS_BIT_MASK & DC_BIT_MASK & ( CMD_READ_RAM| 0xFF00) ;
	write(oledfd,&wdata,2);

	wdata = CS_BIT_MASK & DC_BIT_MASK & WD_BIT_MASK &( CMD_READ_RAM| 0xFF00);
	write(oledfd,&wdata,2);

	wdata = CS_BIT_MASK & DC_BIT_MASK & (CMD_READ_RAM | 0xFF00);
	write(oledfd,&wdata,2);

	wdata = CS_BIT_MASK &  (CMD_READ_RAM | 0xFF00);
	write(oledfd,&wdata,2);


	for (i = 0; i < size ; i++ )
	{
		//wdata = CS_BIT_MASK ;
		//write(oledfd,&wdata,2);

		wdata = CS_BIT_MASK ;
		write(oledfd,&wdata,2);

		wdata = CS_BIT_MASK & RD_BIT_MASK ;
		write(oledfd,&wdata,2);

		wdata = CS_BIT_MASK & RD_BIT_MASK ;
		write(oledfd,&wdata,2);

		wdata = CS_BIT_MASK ;
		write(oledfd,&wdata,2);

		read(oledfd,&dataArr[i],2);

		//wdata = CS_BIT_MASK ;
		//write(oledfd,&wdata,2);

	}
	wdata = DEFAULT_MASK;
	write(oledfd,&wdata ,2);

	return TRUE;
}

int setAddressDefalut(void)
{
	unsigned short  cmd[3];
	cmd[0] = CMD_SET_COLUMN_ADDR;
	cmd[1] = 0;
	cmd[2] = 127;
	writeCmdOled(3,cmd);

	cmd[0] = CMD_SET_ROW_ADDR;
	cmd[1] = 0;
	cmd[2] = 127;
	writeCmdOled(3,cmd);

	return TRUE;
}

// to send cmd  , must unlock
int setCmdLock(int bLock)
{
	unsigned short  cmd[3];
	
	cmd[0] = CMD_LOCK;
	if (bLock)
	{
		cmd[1] = 0x16; // lock
		writeCmdOled(2,cmd);

	}
	else
	{
		cmd[1] = 0x12; // lock
		writeCmdOled(2,cmd);

		// A2,B1,B3,BB,BE accessible
		cmd[1] = 0xB1;
		writeCmdOled(2,cmd);
	}
	return TRUE;
}

int imageLoading(char* fileName)
{
	int imgfile;
	unsigned char* data =NULL;
	int  width , height;

	imgfile = open(fileName , O_RDONLY );
	if ( imgfile < 0 ) 
	{
		printf ("imageloading(%s)  file is not exist . err.\n",fileName);
		return FALSE;
	}
	setCmdLock(FALSE);


	read(imgfile ,&width , sizeof(unsigned char));
	read(imgfile ,&height , sizeof(unsigned char));

	data = malloc( 128 * 128 * 3 );

	read(imgfile, data , 128 * 128 *3 );

	close(imgfile);

	writeData(128 * 128 *3 , data );

	setCmdLock(TRUE);
	return TRUE;
}

static unsigned short gamma[64]= 
{
0xB8,
0x02, 0x03, 0x04, 0x05,
0x06, 0x07, 0x08, 0x09,
0x0A, 0x0B, 0x0C, 0x0D,
0x0E, 0x0F, 0x10, 0x11,
0x12, 0x13, 0x15, 0x17,
0x19, 0x1B, 0x1D, 0x1F,
0x21, 0x23, 0x25, 0x27,
0x2A, 0x2D, 0x30, 0x33,
0x36, 0x39, 0x3C, 0x3F,
0x42, 0x45, 0x48, 0x4C,
0x50, 0x54, 0x58, 0x5C,
0x60, 0x64, 0x68, 0x6C,
0x70, 0x74, 0x78, 0x7D,
0x82, 0x87, 0x8C, 0x91,
0x96, 0x9B, 0xA0, 0xA5,
0xAA, 0xAF, 0xB4

};


int Init(void)
{
	unsigned short wdata[10];
	unsigned char  wcdata[10];
	int i,j;
	wdata[0]= 0xFD;
	wdata[1] = 0x12;
	writeCmdOled(2,wdata);

	
	wdata[0] = 0xFD;
	wdata[1] = 0xB1;
	writeCmdOled(2,wdata);

	wdata[0] = 0xAE;
	writeCmdOled(1,wdata);

	wdata[0] = 0xB3;
	wdata[1] = 0xF1;
	writeCmdOled(2,wdata);

	wdata[0] = 0xCA;
	wdata[1] = 0x7F;
	writeCmdOled(2,wdata);

	wdata[0] = 0xA2;
	wdata[1] = 0x00;
	writeCmdOled(2,wdata);

	wdata[0]= 0xA1;
	wdata[1]=0x00;
	writeCmdOled(2,wdata);

	wdata[0]= 0xA0;
	wdata[1] = 0xB4;
	writeCmdOled(2,wdata);

	wdata[0] = 0xAB;
	wdata[1] = 0x01;
	writeCmdOled(2,wdata);

	wdata[0] = 0xB4;
	wdata[1] = 0xA0;
	wdata[2] = 0xB5;
	wdata[3] = 0x55;
	writeCmdOled(4,wdata);

	wdata[0] = 0xC1;
	wdata[1] = 0xC8;
	wdata[2] = 0x80;
	wdata[3] = 0xC8;
	writeCmdOled(4,wdata);

	wdata[0] = 0xC7;
	wdata[1] = 0x0F;
	writeCmdOled(2,wdata);

	// gamma setting 
	writeCmdOled(64,gamma);


	wdata[0] = 0xB1;
	wdata[1] = 0x32;
	writeCmdOled(2,wdata);

	wdata[0] = 0xB2;
	wdata[1] = 0xA4;
	wdata[2] = 0x00;
	wdata[3] = 0x00;
	writeCmdOled(4,wdata);

	wdata[0] = 0xBB;
	wdata[1] = 0x17;
	writeCmdOled(2,wdata);

	wdata[0] = 0xB6;
	wdata[1] = 0x01;
	writeCmdOled(2, wdata);

	wdata[0]= 0xBE;
	wdata[1] = 0x05;
	writeCmdOled(2, wdata);

	wdata[0] = 0xA6;
	writeCmdOled(1,wdata);
	

	for (i = 0; i < 128;i++ )
	{
		for(j = 0; j < 128; j++ )
		{
			wcdata[0]= 0x3F;
			wcdata[1]= 0;
			wcdata[2] = 0;
			writeData(3,wcdata);
		}
	
	}

	wdata[0] = 0xAF;
	writeCmdOled(1,wdata);

	return TRUE;
}

void oledInit() {
	pthread_mutex_lock(&thread_mutex);
	// printf("Locked @ OLED\n");
	Init();
	usleep(2000);
	pthread_mutex_unlock(&thread_mutex);
	// printf("Unlocked @ OLED\n");
}

void oledLoadImage(char* file) {
	pthread_mutex_lock(&thread_mutex);
	reset();
	usleep(2000);
	Init();
	usleep(20000);
	imageLoading(file);
	usleep(1500000);
	pthread_mutex_unlock(&thread_mutex);
}
