
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<sys/ioctl.h>

#include "ioctl_test.h"

 
//#define WR_VALUE _IOW('a','a',char*)
//#define RD_VALUE _IOR('a','b',char*)
// #definr N_VALUE _IOW('a','c',int*)
int main()
{
        int fd;
        
        char value[100];
        char data[100];
        int delay;
        struct mystruct test = {10000};
       
        printf("\nOpening Driver App1\n");
        fd = open("/dev/task4", O_RDWR);
        if(fd < 0) {
                printf("Cannot open device file...\n");
                return 0;
        }
 
        printf("Enter the Value to send App1\n");
       
        scanf("%s",&data);
        printf("Writing Value to Driver App1\n");
        ioctl(fd, N_VALUE, (int*) &test);
      
  	ioctl(fd, WR_VALUE, (char*) &data); 
        printf("Reading Value from Driver App1\n");
        ioctl(fd, RD_VALUE, (char*) &value);
      
  	printf("read data App1 in driver is %s\n", value);
        printf("Closing Driver App1\n");
        close(fd);
}
