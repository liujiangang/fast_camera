#include "lcd_show_bmp.h"
#include "display.h"


int fbfd = 0;
static void fb_update(struct fb_var_screeninfo *vi)  
{  
    vi->yoffset = 1;  
    ioctl(fbfd, FBIOPUT_VSCREENINFO, vi);  
    vi->yoffset = 0;  
    ioctl(fbfd, FBIOPUT_VSCREENINFO, vi);  
}  

int width, height;

static int cursor_bitmpa_format_convert(char *dst,char *src){
	int i ,j ;
	char *psrc = src ;
	char *pdst = dst;
	char *p = psrc;
	int value = 0x00;
	
	pdst += (width * height * 4);
	for(i=0;i<height;i++){
		p = psrc + (i+1) * width * 3;
		for(j=0;j<width;j++){
			pdst -= 4;
			p -= 3;
			pdst[0] = p[0];
			pdst[1] = p[1];
			pdst[2] = p[2];
			//pdst[3] = 0x00;

			value = *((int*)pdst);
			value = pdst[0];
			if(value == 0x00){
				pdst[3] = 0x00;
			}else{
				pdst[3] = 0xff;
			}
		}
	}

	return 0;
}

int show_bmp(char *path)
{
	FILE *fp;
	int rc;
	int line_x, line_y;
	long int location = 0, BytesPerLine = 0;
	char *bmp_buf = NULL;
	char *bmp_buf_dst = NULL;
	char * buf = NULL;
	int flen = 0;
	int ret = -1;
	int total_length = 0;
	
	//printf("into show_bmp function_____________________________________________________________________________________\n");
	if(path == NULL)
		{
			printf("path Error,return");
			return -1;
		}
	printf("path = %s", path);
	fp = fopen( path, "rb" );
	if(fp == NULL){
		printf("load > cursor file open failed");
		return -1;
	}
	
	fseek(fp,0,SEEK_SET);
	fseek(fp,0,SEEK_END);
	flen = ftell(fp);

	bmp_buf = (char*)calloc(1,flen - 54);
	if(bmp_buf == NULL){
		printf("load > malloc bmp out of memory!");
		return -1;
	}
	//printf("2into show_bmp function_____________________________________________________________________________________\n");

	
	fseek(fp,0,SEEK_SET);
	
	rc = fread(&FileHead, sizeof(BITMAPFILEHEADER),1, fp);
	if ( rc != 1)
	{
		printf("read header error!\n");
		fclose( fp );
		return( -2 );
	}
	

	if (memcmp(FileHead.cfType, "BM", 2) != 0)
	{
		printf("it's not a BMP file\n");
		fclose( fp );
		return( -3 );
	}
	rc = fread( (char *)&InfoHead, sizeof(BITMAPINFOHEADER),1, fp );
	if ( rc != 1)
	{
		printf("read infoheader error!\n");
		fclose( fp );
		return( -4 );
	}
	//printf("3into show_bmp function_____________________________________________________________________________________\n");

	width = InfoHead.ciWidth;
	height = InfoHead.ciHeight;
	printf("FileHead.cfSize =%d byte\n",FileHead.cfSize);
	printf("flen = %d", flen);	
	printf("width = %d, height = %d", width, height);
	total_length = width * height *3;
	
	printf("total_length = %d", total_length);
	
	fseek(fp, FileHead.cfoffBits, SEEK_SET);
	printf(" FileHead.cfoffBits = %d\n",  FileHead.cfoffBits);
	printf(" InfoHead.ciBitCount = %d\n",  InfoHead.ciBitCount);	
	
	buf = bmp_buf;
	while ((ret = fread(buf,1,total_length,fp)) >= 0) {
		if (ret == 0) {
			usleep(100);
			//printf("4into show_bmp function_____________________________________________________________________________________\n");
			continue;
		}
		//printf("5into show_bmp function_____________________________________________________________________________________\n");
		printf("ret = %d", ret);
		buf = ((char*) buf) + ret;
		total_length = total_length - ret;
		if(total_length == 0)break;
	}
	total_length = width * height * 4;
	printf("total_length = %d", total_length);
	bmp_buf_dst = (char*)calloc(1,total_length);
	if(bmp_buf_dst == NULL){
		printf("load > malloc bmp out of memory!");
		return -1;
	}
	
	cursor_bitmpa_format_convert(bmp_buf_dst, bmp_buf);
	memcpy(fbp,bmp_buf_dst,total_length);
	
	//fb_drawPicture(0, 0, 192, 144, bmp_buf_dst);

	//printf("show logo return 0");
	return 0;
}
int show_picture(int fd, char *path)
{
	
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	long int screensize = 0;
	struct fb_bitfield red;
	struct fb_bitfield green;
	struct fb_bitfield blue;
	printf("Enter show_logo");
	retry1:
	fbfd = fd;//open("/dev/fb0", O_RDWR);
	printf("fbfd = %d", fbfd);
	if (fbfd == -1)
	{
		printf("Error opening frame buffer errno=%d (%s)",
             errno, strerror(errno));
		goto retry1;
	}

	if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo))
	{
		printf("Error£ºreading fixed information.\n");
		return -1;
	}

	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo))
	{
		printf("Error: reading variable information.\n");
		return -1;
	}

	printf("R:%d,G:%d,B:%d \n", vinfo.red, vinfo.green, vinfo.blue );

	printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel );
	xres = vinfo.xres;
	yres = vinfo.yres;
	bits_per_pixel = vinfo.bits_per_pixel;

	screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
	printf("screensize=%d byte\n",screensize);

	fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if ((int)fbp == -1)
	{
		printf("Error: failed to map framebuffer device to memory.\n");
		return -1;
	}


	printf("sizeof file header=%d\n", sizeof(BITMAPFILEHEADER));

	
	show_bmp(path);
	//fb_update(&vinfo);

	munmap(fbp, screensize);
	//close(fbfd);
	printf("Exit show_logo");
	return 0;
}
