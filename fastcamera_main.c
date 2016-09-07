
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <sys/types.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <unistd.h>
#include <asm/types.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <math.h>
#include <string.h>
#include <malloc.h>
#include <sys/time.h>
#include <sys/mount.h>
#include <linux/mxcfb.h>
#include <linux/mxc_v4l2.h>
#include <linux/ipu.h>
#include "fastcamera.h"
#include "display.h"
#include "lcd_show_bmp.h"


#define DEV_PATH ""
#define FB_PATH "/dev/fb0"
#define BMP_PATH "/sdcard/20070620224303354.bmp"
#define TFAIL -1
#define TPASS 0

	

#define TFAIL -1
#define TPASS 0

char v4l_capture_dev[100] = "/dev/video0";
char v4l_output_dev[100] = "/dev/video17";


void thread(void *args)
{
	int draw_circle_flag;
	int fd_fb;
	
	
    fd_fb = (int) args;
	printf("fd_fb = %d\n",fd_fb);
	printf("_______Unable to open %s\n", v4l_output_dev);

	draw_circle_flag = 1;
	/*init var */
	fb_init();

	/*open framebuffer */
	fb_open(fd_fb);
	/*clear screen*/
	fb_clean();	
	fb_ui_layout();
//for test 
/*
	fb_draw_char_16x16(100, 100, "1", &g_screen_info,
		       FB_COLOR_YELLOW);
    printf("fb_fd = %d",fd_fb);
	fb_draw_char_32x32(200, 200, "1", &g_screen_info,
		       FB_COLOR_YELLOW);
	fb_draw_char_64x64(300, 300, "1", &g_screen_info,
		       FB_COLOR_YELLOW);

	fb_draw_rect(0, 0, 10, 10, FB_COLOR_YELLOW);

	fb_draw_string16x16(600, 100, "12345678",FB_COLOR_YELLOW);
	fb_draw_string32x32(600, 200, "12345678",FB_COLOR_YELLOW);
	fb_draw_string64x64(600, 300, "12345678", FB_COLOR_YELLOW);

*/
	//int fd_bmp; 
	//fd_bmp	= open(FB_PATH,O_RDWR);
	///	if(fd_bmp < 0)
	//	{
	///		printf("fd_bmp open error %s",__func__);
	//		return TFAIL;
	//	}

		show_picture(fd_fb, BMP_PATH);
		//fb_clean();
	
	while(1)
	{
		;	
	}

}

int main(int argc, char **argv)
{
#ifdef BUILD_FOR_ANDROID
	char fb_device[100] = "/dev/graphics/fb0";
#else
	char fb_device[100] = "/dev/fb0";
#endif
	int fd_fb = 0, i;
	pthread_t id;
	struct mxcfb_gbl_alpha alpha;
	enum v4l2_buf_type type;

	if (process_cmdline(argc, argv) < 0) {
		return TFAIL;
	}

    mount("devtmpfs", "/dev", "devtmpfs", MS_SILENT, 0);
    make_device("/dev/console",666,'c',5,1);
	make_device("/dev/null",666,'c',1,3);
	make_device("/dev/tty",0,'c',5,0);
	make_device("/dev/ttymxc0",0,'c',207,16);	
	make_device("/dev/video0",0,'c',81,2);
	make_device("/dev/video17",0,'c',81,1);	
	make_device("/dev/fb0",0,'c',29,0);
	
	if ((fd_capture_v4l = open(v4l_capture_dev, O_RDWR, 0)) < 0)
	{
		printf("Unable to open %s\n", v4l_capture_dev);
		return TFAIL;
	}

	if ((fd_output_v4l = open(v4l_output_dev, O_RDWR, 0)) < 0)
	{
		printf("Unable to open %s\n", v4l_output_dev);
		return TFAIL;
	}

	if (v4l_capture_setup() < 0) {
		printf("Setup v4l capture failed.\n");
		return TFAIL;
	}

	if (v4l_output_setup() < 0) {
		printf("Setup v4l output failed.\n");
		close(fd_capture_v4l);
		return TFAIL;
	}

	if ((fd_fb = open(fb_device, O_RDWR )) < 0) {
		printf("Unable to open frame buffer\n");
		close(fd_capture_v4l);
		close(fd_output_v4l);
		return TFAIL;
	}

	/* Overlay setting */
	alpha.alpha = 120;
	alpha.enable = 1;
	if (ioctl(fd_fb, MXCFB_SET_GBL_ALPHA, &alpha) < 0) {
		printf("Set global alpha failed\n");
		close(fd_fb);
		close(fd_capture_v4l);
		close(fd_output_v4l);
		return TFAIL;
	}

	
	if(pthread_create(&id,NULL,(void *) thread,(void *)fd_fb) != 0){
		printf ("Create pthread error!n");
		exit (-1);
	}

	mxc_v4l_tvin_test();

	type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ioctl(fd_output_v4l, VIDIOC_STREAMOFF, &type);

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ioctl(fd_capture_v4l, VIDIOC_STREAMOFF, &type);

	for (i = 0; i < g_output_num_buffers; i++)
	{
		munmap(output_buffers[i].start, output_buffers[i].length);
	}
	for (i = 0; i < g_capture_num_buffers; i++)
	{
		munmap(capture_buffers[i].start, capture_buffers[i].length);
	}

	close(fd_capture_v4l);
	close(fd_output_v4l);
	close(fd_fb);
	return 0;
}
