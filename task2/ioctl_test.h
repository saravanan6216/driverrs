#ifndef IOCTL_TEST_H
#define IOCTL_TEST_H

struct mystruct
{
	int delay;
};

#define WR_VALUE _IOW('a', 'a',  int *)
#define RD_VALUE _IOR('a', 'b', int32_t *)
#define N_VALUE _IOW('a','c',struct mystruct *)

#endif
