#include <stdio.h>
#include <stdlib.h>     // for exit
#include <unistd.h>     // for open/close
#include <fcntl.h>      // for O_RDWR
#include <sys/ioctl.h>  // for ioctl
#include <sys/mman.h>
#include <linux/fb.h>   // for fb_var_screeninfo, FBIOGET_VSCREENINFO
#include "bitmap.h"

#define FBDEV_FILE  "/dev/fb0"
#define BIT_VALUE_24BIT   24

extern pthread_mutex_t thread_mutex;

typedef struct {
    int fbfd;
    int screen_width;
    int screen_height;
    int bits_per_pixel;
    int line_length;
    int mem_size;

    char    *pData, *data;
    unsigned long   bmpdata[1280*800];
    unsigned char   *pfbmap;
    struct  fb_var_screeninfo fbvar;
    struct  fb_fix_screeninfo fbfix;
} bitmapDef;

int bitmapfd;

void read_bmp(char *filename, char **pDib, char **data, int *cols, int *rows)
{
    BITMAPFILEHEADER    bmpHeader;
    BITMAPINFOHEADER    *bmpInfoHeader;
    unsigned int    size;
    unsigned char   magicNum[2];
    int     nread;
    FILE    *fp;

    fp  =  fopen(filename, "rb");
    if(fp == NULL) {
        printf("ERROR\n");
        return;
    }

    // identify bmp file
    magicNum[0]   =   fgetc(fp);
    magicNum[1]   =   fgetc(fp);
    // printf("magicNum : %c%c\n", magicNum[0], magicNum[1]);

    if(magicNum[0] != 'B' && magicNum[1] != 'M') {
        printf("It's not a bmp file!\n");
        fclose(fp);
        return;
    }

    nread   =   fread(&bmpHeader.bfSize, 1, sizeof(BITMAPFILEHEADER), fp);
    size    =   bmpHeader.bfSize - sizeof(BITMAPFILEHEADER);
    *pDib   =   (unsigned char *)malloc(size);      // DIB Header(Image Header)
    fread(*pDib, 1, size, fp);
    bmpInfoHeader   =   (BITMAPINFOHEADER *)*pDib;

    printf("nread : %d\n", nread);
    printf("size : %d\n", size);

    // check 24bit
    if(BIT_VALUE_24BIT != (bmpInfoHeader->biBitCount))     // bit value
    {
        printf("It supports only 24bit bmp!\n");
        fclose(fp);
        return;
    }

    *cols   =   bmpInfoHeader->biWidth;
    *rows   =   bmpInfoHeader->biHeight;
    *data   =   (char *)(*pDib + bmpHeader.bfOffBits - sizeof(bmpHeader) - 2);
    fclose(fp);
}

void openScreen(bitmapDef *info) {
    if( (bitmapfd = open(FBDEV_FILE, O_RDWR)) < 0)
    {
        printf("%s: open error\n", FBDEV_FILE);
        exit(1);
    }

    if( ioctl(bitmapfd, FBIOGET_VSCREENINFO, &(info->fbvar)) )
    {
        printf("%s: ioctl error - FBIOGET_VSCREENINFO \n", FBDEV_FILE);
        exit(1);
    }

    if( ioctl(bitmapfd, FBIOGET_FSCREENINFO, &(info->fbfix)) )
    {
        printf("%s: ioctl error - FBIOGET_FSCREENINFO \n", FBDEV_FILE);
        exit(1);
    }

    if (info->fbvar.bits_per_pixel != 32)
    {
        fprintf(stderr, "bpp is not 32\n");
        exit(1);
    }

    info->screen_width    =   info->fbvar.xres;
    info->screen_height   =   info->fbvar.yres;
    info->bits_per_pixel  =   info->fbvar.bits_per_pixel;
    info->line_length     =   info->fbfix.line_length;
    info->mem_size    =   info->line_length * info->screen_height;

    printf("fbvar? : %x %x %x %x %x\n", info->screen_height, info->screen_width, info->bits_per_pixel, info->line_length, info->mem_size);

    info->pfbmap  =   (unsigned char *)
    mmap(0, info->mem_size, PROT_READ|PROT_WRITE, MAP_SHARED, bitmapfd, 0);

    if ((unsigned)info->pfbmap == (unsigned)-1)
    {
        perror("fbdev mmap\n");
        exit(1);
    }
}

void close_bmp(char **pDib)     // DIB(Device Independent Bitmap)
{
    free(*pDib);
}

void clearBitmap() {
	bitmapDef info;
    unsigned long   *ptr;
    int coor_x, coor_y;

	openScreen(&info);

    for(coor_y = 0; coor_y < info.screen_height; coor_y++) {
        ptr =   (unsigned long *)info.pfbmap + (info.screen_width * coor_y);
        for(coor_x = 0; coor_x < info.screen_width; coor_x++)
        {
            *ptr++  =   0x000000;
        }
    }
}

void setScreen(char* filename, int offsetX, int offsetY) {
	bitmapDef info;

    int i, j, k, t;
    int coor_x, coor_y;
    int cols = 0, rows = 0;
    char    r, g, b;
    unsigned long   *ptr;
    unsigned long   pixel;

    // printf("Read? : %s\n", filename);
    // pthread_mutex_lock(&thread_mutex);
    read_bmp(filename, &(info.pData), &(info.data), &cols, &rows);
    // pthread_mutex_unlock(&thread_mutex);
    // printf("Read!\n");

    // printf("Compose?\n");
    for(j = 0; j < rows; j++)
    {
        k   =   j * cols * 3;
        t   =   (rows - 1 - j) * cols;

        for(i = 0; i < cols; i++)
        {
            b   =   *(info.data + (k + i * 3));
            g   =   *(info.data + (k + i * 3 + 1));
            r   =   *(info.data + (k + i * 3 + 2));

            pixel = ((r<<16) | (g<<8) | b);
            info.bmpdata[t+i]    =   pixel;          // save bitmap data bottom up
        }
    }
    close_bmp(&(info.pData));
    // printf("Composed!\n");

    // printf("Opened?\n");
    // pthread_mutex_lock(&thread_mutex);
	openScreen(&info);
    // printf("Opened!\n");

    // printf("Inserted?\n");
    for(coor_y = 0; coor_y < rows; coor_y++) {
        ptr =   (unsigned long*)info.pfbmap + (info.screen_width * (coor_y + offsetX)) + offsetY;
        // printf("prt : %x\n", ptr);

        for (coor_x = 0; coor_x < cols; coor_x++) {
            // if(offsetX > 0)
                // printf("x: %d y: %d data: %d\n", coor_y, coor_x, info.bmpdata[coor_x + coor_y * cols]);
            *ptr++  =   info.bmpdata[coor_x + coor_y*cols];
        }
    }
    // printf("Inserted!\n");

    // printf("Unmapped?\n");
    munmap(info.pfbmap, info.mem_size);
    close(bitmapfd);
    usleep(1000000);
    // pthread_mutex_unlock(&thread_mutex);
    // printf("Unmapped!\n");

}