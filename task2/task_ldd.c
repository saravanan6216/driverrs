
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>              //copy_to/from_user()
#include <linux/ioctl.h>
#include <linux/err.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/seq_file.h>

#include "ioctl_test.h" 
#include <linux/kernel.h>
#include <linux/unistd.h>
#include <linux/syscalls.h>
         
#define LOG_PATH "/var/log/my_driver_log"


static void log_dmesg_to_file(void)

{

        /* char *argv[] - array of strings, each element of this array is pointer to a character (string)

         * -c - common option in many shells, it tells the shell to take the next argument as a command to be executed

         *  "dmesg > " - shell redirection operator,

         *  LOG_FILE_PATH is a macro representing the path to the log file where the output of dmesg will be redirected.

         */

        char *argv[] = { "/bin/sh", "-c", "dmesg -l info > " LOG_PATH, NULL };
 
        /* char *envp[] - array of strings, each element points to a character (string)

         * HOME=/: The first element sets the HOME environment variable to the root directory (/).

         * TERM=linux: The second element sets the TERM environment variable to linux. This variable is often used to specify the type of terminal being used.

         * PATH=/sbin:/bin:/usr/sbin:/usr/bin: The third element sets the PATH environment variable. It specifies a colon-separated list of directories where 

         * the shell looks for executable files. In this case, it includes standard system directories like /sbin, /bin, /usr/sbin, and /usr/bin.

         */

        static char *envp[] = { "HOME=/", "TERM=linux", "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };

        call_usermodehelper(argv[0], argv, envp, UMH_WAIT_EXEC);

}

        
        /*
        ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
	ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);  
	int (*open) (struct inode *, struct file *);
	int (*release) (struct inode *, struct file *);
	long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
	
	
	copy_from_user(void *to, const void __user *from, unsigned long n)
	copy_to_user(void __user *to, const void *from, unsigned long n)

	int cdev_add(struct cdev *, dev_t, unsigned);
	int cdev_device_add(struct cdev *cdev, struct device *dev);
	void cdev_device_del(struct cdev *cdev, struct device *dev);
	int device_create_file(struct device *device,
		       const struct device_attribute *entry);
		       
	sprintf(buffer, "%u:%u", MAJOR(dev), MINOR(dev));	
	*/
          
int del;
char value[100]="driver1";
char  data[100];
dev_t dev = 0;
static struct class *my_class;
static struct cdev my_cdev;
DEFINE_MUTEX(mylock);


          
static int mydrv_open(struct inode *inode, struct file *file)
{
        pr_info("Device File Open\n");
        return 0;
}

static int mydrv_close(struct inode *inode, struct file *file)
{
	
	log_dmesg_to_file();
        pr_info("Device File Closed...!!!\n");
        return 0;
}

static ssize_t mydrv_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
        pr_info("Read Function\n");
        return 0;
}

static ssize_t mydrv_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
        pr_info("Write function\n");
        return len;
}

   static long mydrv_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
   {
       struct mystruct test;
       
         switch(cmd) {			 
                case WR_VALUE:
                
			 mutex_lock_interruptible(&mylock);
          		 printk("lock acquired\n");
                        copy_from_user(&data ,(char *) arg, sizeof(data));
                         pr_info("test delay = %d\n", del);
                              msleep(del);
          		// pr_info(" delay = %d\n", del);
                        pr_info("data in driver = %s\n", data);
                        
                        mutex_unlock(&mylock); 
                        printk("unlock acquired\n");
                        break;
                        
                        
                case RD_VALUE:
                	
			 printk(KERN_DEBUG "READER : acquired lock: executing critical code\n");
                        copy_to_user((char*) arg, &value, sizeof(value));
                        printk("data read\n");                     
                        break;
                        
               case N_VALUE:
                        copy_from_user(&test,(struct mystruct*)arg,sizeof(test));
                      //  pr_info("N delay=%d\n",test.delay);
                        del=test.delay;
                        break;
        }
        return 0;

   }
       



static struct file_operations fops={
        .owner = THIS_MODULE,
        .read  = mydrv_read,
        .write = mydrv_write,
        .open  = mydrv_open,
        .release =mydrv_close,
        .unlocked_ioctl=mydrv_ioctl,
};


static int __init drv_init(void)
{


        if((alloc_chrdev_region(&dev, 0, 3, "task4")) <0){
                pr_err("Cannot allocate major number\n");
                return -1;
        }
        pr_info("module started \n");
        pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
        
        cdev_init(&my_cdev,&fops);
        cdev_add(&my_cdev,dev,3);
       
        my_class = class_create(THIS_MODULE,"Virtuaaall");
 
        device_create(my_class,NULL,dev,NULL,"task4");
        
        pr_info("Device Driver Insert...Done!!!\n");
        return 0;
 

}

static void __exit drv_exit(void)
{
        device_destroy(my_class,dev);
        class_destroy(my_class);
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev, 3);
        pr_info("Device Driver Remove...Done!!!\n");
}
 
module_init(drv_init);
module_exit(drv_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("saravanan");
MODULE_DESCRIPTION("Simple driver");


