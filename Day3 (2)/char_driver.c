#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#define MYMAJOR 110
#define MYMINOR 0

#define DRVNAME "bitsilica"

static struct cdev *mycdev;
static dev_t  devid;

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


static int __init  drv_init(void)
{
	int ret;
	/*step 1: Reserve major and minor no's */
	devid = MKDEV(MYMAJOR, MYMINOR);
        ret = register_chrdev_region(devid, 1, DRVNAME);

	/* step 2: register driver with vfs */
	mycdev = cdev_alloc();
	cdev_init(mycdev, &mydrv_ops);
	cdev_add( mycdev, devid, 1);	


	return 0;
}

static  void __exit  drv_exit(void)
{
	unregister_chrdev_region(devid, 1);
	cdev_del(mycdev);
	
}

module_init(drv_init);
module_exit(drv_exit);

MODULE_AUTHOR("Sathish Reddy ");
MODULE_DESCRIPTION("Basic Char driver");
MODULE_LICENSE("GPL");

