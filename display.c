/*******************************************************************************
    \file         display.c
    \brief        update process display.
    \version      0.1
    \date         2015-04-21
    \author       wangq
    \warning      Copyright (C), 2015, Ample.\n
------------------------------------------------------------------------------\n*/

/*******************************************************************************
    Include Files
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <math.h>
#include <pthread.h>
#include "display.h"

/*******************************************************************************
    Macro Definition
*******************************************************************************/


static struct fb_fix_screeninfo g_finfo;
static struct fb_var_screeninfo g_vinfo;
fb_info_t g_screen_info;
static fb_process_bar_t g_process_bar;

static fb_log_t *g_log;
static int g_log_row = 0;
static int g_log_column = 0;
static int g_log_index = 0;
static int g_circle_index = 0;
static int g_circle_index_x0 = 0;
static int g_circle_index_y0 = 0;

//static int fb_drawPoint_rgb32(int x0, int y0, int color, fb_info_t * scr_info);
//static int fb_draw_rect(int x0, int y0, int width, int height, int color);
/*******************************************************************************
    Function  Definition
*******************************************************************************/
/*******************************************************************************
\fn         int fb_init(void)
\brief      init global var
*******************************************************************************/
int fb_init(void)
{
	memset(&g_process_bar, 0, sizeof(fb_process_bar_t));
	memset(&g_screen_info, 0, sizeof(fb_info_t));
	memset(&g_finfo, 0, sizeof(struct fb_fix_screeninfo));
	memset(&g_vinfo, 0, sizeof(struct fb_var_screeninfo));
	return 0;
}

/*******************************************************************************
\fn         int fb_ui_layout(void)
\brief      UI layout
*******************************************************************************/
int fb_ui_layout(void)
{
    /** screen **/
	fb_draw_rect(0, 0, g_vinfo.xres, g_vinfo.yres, FB_COLOR_SCREEN);
	/*UI layout */
    /** head **/
	g_process_bar.fb_head_x0 = 0 + FB_UI_SPACE_15;
	g_process_bar.fb_head_y0 = 0 + FB_UI_SPACE_15;
	g_process_bar.fb_head_height = FB_UI_SPACE_15 +
	    FB_FONTDATA_64X64_HEIGHT + FB_UI_SPACE_60;
	g_process_bar.fb_head_width = g_vinfo.xres;
	
	g_process_bar.fb_progress_x0 = (g_vinfo.xres + 16 * 4) / 2;
	g_process_bar.fb_progress_y0 = g_process_bar.fb_head_height +
	    FB_UI_SPACE_60;
	g_process_bar.fb_progress_height = FB_UI_SPACE_60 +
	    FB_FONTDATA_32X32_HEIGHT + FB_UI_SPACE_60 + FB_UI_SPACE_60;
	g_process_bar.fb_progress_width = 16 * 5;

	g_process_bar.fb_circle_x0 = g_vinfo.xres / 2 + 16;
	g_process_bar.fb_circle_y0 = g_process_bar.fb_progress_y0 + 16;
	g_process_bar.fb_circle_radius = 60;

	g_circle_index_x0 =
	    g_process_bar.fb_circle_x0 - g_process_bar.fb_circle_radius;
	g_circle_index_y0 = g_process_bar.fb_circle_y0;
	g_circle_index = 0;
	g_process_bar.fb_log_x0 = 0 + FB_UI_SPACE_15;
	g_process_bar.fb_log_y0 = g_process_bar.fb_head_height +
	    g_process_bar.fb_progress_height;

	g_process_bar.fb_log_height = g_vinfo.yres -
	    g_process_bar.fb_head_height -
	    g_process_bar.fb_progress_height - FB_UI_SPACE_60;

	g_process_bar.fb_log_width = g_vinfo.xres;

	g_log_row = g_process_bar.fb_log_height / 8 / 2;
	g_log_column =
	    (g_process_bar.fb_log_width - FB_UI_SPACE_15 - FB_UI_SPACE_15) / 8;

	g_log = (fb_log_t *) malloc(sizeof(fb_log_t) * g_log_row);
	if (NULL == g_log) {
		fb_debug_msg("Error: g_log malloc failed\n");
		exit(-1);
	}

	int iter = 0;
	for (iter = 0; iter < g_log_row; iter++) {
		g_log[iter].log = (char *)malloc(g_log_column);
		if (NULL == g_log[iter].log) {
			fb_debug_msg("Error: g_log[i] malloc failed\n");
			exit(-1);
		}
	}

	return 0;
}

/*******************************************************************************
\fn         int fb_open(const char* fb)
\brief      open /dev/fb*
*******************************************************************************/
//int fb_open(const char *fb)
int fb_open(const int fb)
{
	/*
	if (NULL == fb) {
		fb_debug_msg("Error: fb_open param is Null\n");
		exit(-1);
	}
	*/

	//g_screen_info.fb_fd = open(fb, O_RDWR);
	g_screen_info.fb_fd = fb;

    /** Get Fixed Info **/
	if (ioctl(g_screen_info.fb_fd, FBIOGET_FSCREENINFO, &g_finfo) != 0) {
		fb_debug_msg("Error: reading fixed information\n");
		exit(-1);
	}

    /** Get Variable Info **/
	if (ioctl(g_screen_info.fb_fd, FBIOGET_VSCREENINFO, &g_vinfo) != 0) {
		fb_debug_msg("Error: reading variable information\n");
		exit(-1);
	}

	g_screen_info.fb_width = g_vinfo.xres;
	g_screen_info.fb_height = g_vinfo.yres;
	g_screen_info.fb_pixel_bits = g_vinfo.bits_per_pixel;
	g_screen_info.fb_pixel_bytes = g_vinfo.bits_per_pixel / 8;
	g_screen_info.fb_line_len = g_finfo.line_length;
	g_screen_info.fb_size = g_finfo.smem_len;
	g_screen_info.fb_color = FB_COLOR_SCREEN;
	g_screen_info.fb_mem =
	    mmap(0, g_finfo.smem_len, PROT_READ | PROT_WRITE,
		 MAP_FILE | MAP_SHARED, g_screen_info.fb_fd, 0);

	fb_debug_msg("width:%d height:%d size:%d\n",g_screen_info.fb_width,
             g_screen_info.fb_height,g_screen_info.fb_size);
	if (g_vinfo.bits_per_pixel != 32) {
		fb_debug_msg("Error: not RGB24\n");
		close(g_screen_info.fb_fd);
		exit(-1);
	}
	if (g_screen_info.fb_mem == MAP_FAILED) {
		fb_debug_msg
		    ("Error: failed to map framebuffer device to memory\n");
		close(g_screen_info.fb_fd);
		exit(-1);
	} else {
		fb_debug_msg("Sucess: map framebuffer device to memory\n");
	}

	return 0;
}

/*******************************************************************************
\fn         int fb_clean()
\brief      draw screen black color(0)
*******************************************************************************/
int fb_clean()
{
//	memset(g_screen_info.fb_mem, FB_COLOR_SCREEN, g_screen_info.fb_size);
    fb_draw_rect(0,0,g_screen_info.fb_width,g_screen_info.fb_height,FB_COLOR_SCREEN);

    return 0;
}

/*******************************************************************************
\fn         int fb_drawPoint_rgb32 (int x0, int y0, int color , fb_info_t* scr_info )
\brief      draw a point
*******************************************************************************/
int fb_drawPoint_rgb32(int x0, int y0, int color, fb_info_t * scr_info)
{
	int *dest =
	    (int *)(scr_info->fb_mem) + (y0) * (scr_info->fb_width) + (x0);
	*dest = color;

	return 0;
}



int fb_drawPicture(int x0, int y0, int width, int height, unsigned char picture_data[])
{
	int i_looper = 0;
	int j_looper = 0;
	int i = 0;

	for (i_looper = 0; i_looper < width; i_looper++) {
		for (j_looper = 0; j_looper < height; j_looper++) {
			if ((x0 + i_looper < g_vinfo.xres) &&
			    (y0 + j_looper < g_vinfo.yres)) {
					fb_drawPoint_rgb32(x0 + i_looper, y0 + j_looper,
						   *(picture_data++),&g_screen_info);
			}
		}
	}
	return 0;
}

/*******************************************************************************
\fn         int fb_draw_rect(int x0, int y0, int width, int height, int color )
\brief      draw a rect
*******************************************************************************/
int fb_draw_rect(int x0, int y0, int width, int height, int color)
{
	int i_looper = 0;
	int j_looper = 0;

	for (i_looper = 0; i_looper < width; i_looper++) {
		for (j_looper = 0; j_looper < height; j_looper++) {
			if ((x0 + i_looper < g_vinfo.xres) &&
			    (y0 + j_looper < g_vinfo.yres)) {

				fb_drawPoint_rgb32(x0 + i_looper, y0 + j_looper,
						   color, &g_screen_info);
			}
		}
	}

	return 0;
}

/*******************************************************************************
\fn         int fb_get_char_index(const char* string)
\brief      get array index
*******************************************************************************/
int fb_get_char_index(const char *string)
{
	int index = 0xFF;
	if (string == NULL) {
		fb_debug_msg("Error: fb_get_char_index Draw Sting is Null.\n");
		exit(-1);
	}

	if ((*string) < 0x80) {
	/** ACSII **/
		index = *string - FB_FONTDATA_64X64_ASCII;

	}

	if ((index < 0) || (index > 95)) {
		index = 0;
	}

	return index;
}














