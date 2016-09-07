
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

#define TFAIL -1
#define TPASS 0


struct testbuffer
{
	unsigned char *start;
	size_t offset;
	unsigned int length;
};

extern char v4l_capture_dev[100];
extern char v4l_output_dev[100];

extern int fd_capture_v4l;
extern int fd_output_v4l ;
extern int g_output_num_buffers;
extern int g_capture_num_buffers;

struct testbuffer output_buffers[4];
struct testbuffer capture_buffers[3];


extern int make_device(char* path,	mode_t mode,char type, int maj, int min);
extern int start_capturing(void);
extern int prepare_output(void);
extern int v4l_capture_setup(void);
extern int v4l_output_setup(void);
extern int mxc_v4l_tvin_test(void);
extern int process_cmdline(int argc, char **argv);





