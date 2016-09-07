
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
//#include <utils/Log.h>
#include <errno.h>


typedef struct
{
	char cfType[2];
	long cfSize;
	long cfReserved;
	long cfoffBits;
}__attribute__((packed)) BITMAPFILEHEADER;

typedef struct
{
	char ciSize[4];
	long ciWidth;
	long ciHeight;
	char ciPlanes[2];
	int ciBitCount;
	char ciCompress[4];
	char ciSizeImage[4];
	char ciXPelsPerMeter[4];
	char ciYPelsPerMeter[4];
	char ciClrUsed[4]; 
	char ciClrImportant[4]; 
}__attribute__((packed)) BITMAPINFOHEADER;

typedef struct
{
	unsigned short blue;
	unsigned short green;
	unsigned short red;
	unsigned short reserved;
}__attribute__((packed)) PIXEL;

BITMAPFILEHEADER FileHead;
BITMAPINFOHEADER InfoHead;

static char *fbp = 0;
static int xres = 0;
static int yres = 0;
static int bits_per_pixel = 0;


extern int show_bmp(char *path);
extern int show_picture(int fd, char *path); //show picture to LCD