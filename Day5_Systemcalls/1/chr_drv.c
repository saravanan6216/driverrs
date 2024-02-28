#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/slab.h>

#define CHAR_DEV_NAME "bitsilica_cdrvv"
#define MAX_LENGTH 4000
#define SUCCESS 0

static struct class *my_class;
static char *char_device_buf;
struct cdev *my_cdev;
dev_t mydev;
int count=1,inuse=0;
//int a
/*
static int mydrv_open(struct inode *inode,
			    struct file  *file)
{
	static int counter = 0;
	counter++;
	printk(KERN_INFO "Number of times open() was called: %d\n", counter);
	printk (KERN_INFO " MAJOR number = %d, MINOR number = %d\n",imajor (inode), iminor (inode));
	printk(KERN_INFO "Process id of the current process: %d\n",current->pid );
	printk (KERN_INFO "ref=%d\n", module_refcount(THIS_MODULE));
	return SUCCESS;
}
*/
static int mydrv_open(struct inode *inode, struct file *file)
{
	if(inuse)
	{
		printk(KERN_INFO "\nDevice busy %s\n",CHAR_DEV_NAME);
		return -EBUSY;
	}	
	inuse=1;
	printk(KERN_INFO "Open operation invoked \n");
	return SUCCESS;
}
static int mydrv_close(struct inode *inode, struct file *file)
{
	pr_info("Drivers release call invoked\n");
	inuse=0;
	return 0;
}

/*	
static int mydrv_close(struct inode *inode, struct file *file)
{
	pr_info("Drivers release call invoked\n");
	return 0;
}
*/

static ssize_t mydrv_read(struct file *file, 
		            char *buf,
			    size_t lbuf,
			    loff_t *ppos)
{
	int maxbytes; /* number of bytes from ppos to MAX_LENGTH   a=*buf*/
	int bytes_to_do; /* number of bytes to read */
	int nbytes; /* number of bytes actually read */

	maxbytes = MAX_LENGTH - *ppos;
	
	if( maxbytes > lbuf ) bytes_to_do = lbuf;
	else bytes_to_do = maxbytes;
	
	if( bytes_to_do == 0 ) {
		printk("Reached end of device\n");
		return -ENOSPC; /* Causes read() to return EOF */
	}
	
	nbytes = bytes_to_do - 
		 copy_to_user( buf, /* to */
			       char_device_buf + *ppos, /* from */
			       bytes_to_do ); /* how many bytes */

	*ppos += nbytes;
	return nbytes;	
}

static ssize_t mydrv_write(struct file *file, const char *buf, size_t lbuf, loff_t *ppos)
{
       int nbytes = lbuf - copy_from_user (char_device_buf + *ppos, buf, lbuf);
       *ppos += nbytes;
       printk (KERN_INFO "\n Rec'vd data from app %s , nbytes=%d\n",char_device_buf,nbytes);
	// a++;
       return nbytes;
}   
   
static loff_t mydrv_lseek (struct file *file, loff_t offset, int orig)
{
    loff_t testpos;     //64 bit which driver needs to know the file current position in the file
    switch (orig) {

    case 0:                 /* SEEK_SET */
            testpos = offset;
            break;
    case 1:                 /* SEEK_CUR */
            testpos = file->f_pos + offset; // filp->f_pos is current pointer position in file, whereas offp is where user does access to file.
            break;
    case 2:                 /* SEEK_END */
           testpos = MAX_LENGTH + offset;
           break;
    default:
          return -EINVAL;
    }
   
    testpos = testpos < MAX_LENGTH ? testpos : MAX_LENGTH;
    testpos = testpos >= 0 ? testpos : 0;
    file->f_pos = testpos;
    printk (KERN_INFO "Seeking to pos=%ld\n", (long)testpos);
    return testpos;
}


static struct file_operations mydrv_ops = {
	.owner = THIS_MODULE,
	.read = mydrv_read,
	.write = mydrv_write,
	.open = mydrv_open,
	.release = mydrv_close,
	.llseek = mydrv_lseek,
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
	
	my_class = class_create (THIS_MODULE, "Virtuaaall");
        device_create (my_class, NULL, mydev, NULL, "%s", "bitsilica_cdrw");
        
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

