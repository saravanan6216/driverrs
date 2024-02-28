#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include<linux/slab.h>

#define CHAR_DEV_NAME "bitsilica2"
#define MAX_LENGTH 4000
#define SUCCESS 0

static char *char_device_buf;
struct cdev *my_cdev;
dev_t mydev;


static int mydrv_open(struct inode *inode, struct file *file)
{
	pr_info("Drivers open call invoked\n");
	return 0;
}

static int mydrv_close(struct inode *inode, struct file *file)
{
	pr_info("Drivers release call invoked\n");
	return 0;
}

static ssize_t mydrv_read(struct file *file, char __user *ptr, size_t count, loff_t *off)
{
	pr_info("Drivers read invoked");
	return 0;
}

static ssize_t mydrv_write(struct file *file,const char __user *ptr,size_t count,loff_t *off)
{
	pr_info("Drivers write invoked");
	return 0;
}

static struct file_operations mydrv_ops = {
	.owner = THIS_MODULE,
	.read = mydrv_read,
	.write = mydrv_write,
	.open = mydrv_open,
	.release = mydrv_close,
};

static __init int char_dev_init(void)
{
	int ret,count=1;

	if (alloc_chrdev_region (&mydev, 0, count, CHAR_DEV_NAME) < 0) {
            printk (KERN_ERR "failed to reserve major/minor range\n");
            return -1;
    }

        if (!(my_cdev = cdev_alloc ())) {
            printk (KERN_ERR "cdev_alloc() failed\n");
            unregister_chrdev_region (mydev, count);
            return -1;
 	}
	cdev_init(my_cdev,&mydrv_ops);

	ret=cdev_add(my_cdev,mydev,count);
	if( ret < 0 ) {
		printk(KERN_INFO "Error registering device driver\n");
	        cdev_del (my_cdev);
                unregister_chrdev_region (mydev, count); 	
		return -1;
	}
	printk(KERN_INFO"\nDevice Registered: %s\n",CHAR_DEV_NAME);
	printk (KERN_INFO "Major number = %d, Minor number = %d\n", MAJOR (mydev),MINOR (mydev));

	char_device_buf =(char *)kmalloc(MAX_LENGTH,GFP_KERNEL);
	return 0;
}

static __exit void  char_dev_exit(void)
{
	
	 cdev_del(my_cdev);
	 unregister_chrdev_region(mydev,1);
	 kfree(char_device_buf);
	 printk(KERN_INFO "\n Driver unregistered \n");
}

module_init(char_dev_init);
module_exit(char_dev_exit);

MODULE_AUTHOR("Sathish Reddy ");
MODULE_DESCRIPTION("Basic Char driver");
MODULE_LICENSE("GPL");

