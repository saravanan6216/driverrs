#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <asm/current.h>
#include <linux/device.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/slab.h>

#define CHAR_DEV_NAME "my_cdrv"
#define MAX_LENGTH 4000
#define SUCCESS 0

static char *char_device_buf;
struct cdev *my_cdev;
dev_t mydev;
int count=1;
static struct class *my_class;

static DECLARE_WAIT_QUEUE_HEAD(mywq);
static atomic_t data_ready;

static int char_dev_open(struct inode *inode,
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

static int char_dev_release(struct inode *inode,
		            struct file *file)
{
	return SUCCESS;
}
static ssize_t char_dev_read(struct file *file, 
		            char *buf,
			    size_t lbuf,
			    loff_t *ppos)
/*			    
static int char_dev_read(struct file *file, 
		            char *buf,
			    size_t lbuf,
			    loff_t *ppos)*/
{
	int maxbytes; /* number of bytes from ppos to MAX_LENGTH */
	int bytes_to_do; /* number of bytes to read */
	int nbytes; /* number of bytes actually read */

	maxbytes = MAX_LENGTH - *ppos;
	
	if( maxbytes > lbuf ) bytes_to_do = lbuf;
	else bytes_to_do = maxbytes;
	
	if( bytes_to_do == 0 ) {
		printk("Reached end of device\n");
		return -ENOSPC; /* Causes read() to return EOF */
	}

	printk (KERN_INFO "process %i (%s) going to sleep\n", current->pid,current->comm);
	wait_event_interruptible (mywq, (atomic_read (&data_ready)));
    	printk (KERN_INFO "process %i (%s) resuming\n", current->pid,current->comm);
	atomic_set (&data_ready, 0);

	
	nbytes = bytes_to_do - 
		 copy_to_user( buf, /* to */
			       char_device_buf + *ppos, /* from */
			       bytes_to_do ); /* how many bytes */

	*ppos += nbytes;
	return nbytes;	
}
static long int char_dev_write(struct file *file,
		            const char *buf,
			    long unsigned int  lbuf,
			    long long  *ppos)
/*
static int char_dev_write(struct file *file,
		            const char *buf,
			    size_t lbuf,
			    loff_t *ppos)*/
{
	int nbytes; /* Number of bytes written */
	int bytes_to_do; /* Number of bytes to write */
	int maxbytes; /* Maximum number of bytes that can be written */

	maxbytes = MAX_LENGTH - *ppos;

	if( maxbytes > lbuf ) bytes_to_do = lbuf;
	else bytes_to_do = maxbytes;

	if( bytes_to_do == 0 ) {
		printk("Reached end of device\n");
		return -ENOSPC; /* Returns EOF at write() */
	}

	nbytes = bytes_to_do -
	         copy_from_user( char_device_buf + *ppos, /* to */
				 buf, /* from */
				 bytes_to_do ); /* how many bytes */
	*ppos += nbytes;

	printk (KERN_INFO "process %i (%s) wake up readers...\n",current->pid, current->comm);
	atomic_set (&data_ready, 1);
        wake_up_interruptible (&mywq);

	return nbytes;
}

static unsigned int char_dev_poll (struct file *file, poll_table * wait)
{
    poll_wait (file, &mywq, wait);
    printk (KERN_INFO "In poll at jiffies=%ld\n", jiffies);
    if (atomic_read (&data_ready))
        return POLLIN | POLLRDNORM;
    return 0;
}



static struct file_operations char_dev_fops = {
	.owner = THIS_MODULE,
	.read = char_dev_read,
	.write = char_dev_write,
	.open = char_dev_open,
	.release = char_dev_release,
	.poll = char_dev_poll
};

static __init int char_dev_init(void)
{
	int ret;

	if (alloc_chrdev_region (&mydev, 0, count, CHAR_DEV_NAME) < 0) {
            printk (KERN_ERR "failed to reserve major/minor range\n");
            return -1;
    }

        if (!(my_cdev = cdev_alloc ())) {
            printk (KERN_ERR "cdev_alloc() failed\n");
            unregister_chrdev_region (mydev, count);
            return -1;
 	}
	cdev_init(my_cdev,&char_dev_fops);

	ret=cdev_add(my_cdev,mydev,count);
	if( ret < 0 ) {
		printk(KERN_INFO "Error registering device driver\n");
	        cdev_del (my_cdev);
                unregister_chrdev_region (mydev, count); 	
		return -1;
	}

	my_class = class_create (THIS_MODULE, "VIRTUAL");
        device_create (my_class, NULL, mydev, NULL, "%s", "my_cdrv");

	printk(KERN_INFO"\nDevice Registered: %s\n",CHAR_DEV_NAME);
	printk (KERN_INFO "Major number = %d, Minor number = %d\n", MAJOR (mydev),MINOR (mydev));

	char_device_buf =(char *)kmalloc(MAX_LENGTH,GFP_KERNEL);
	return 0;
}

static __exit void  char_dev_exit(void)
{
	 device_destroy (my_class, mydev);
         class_destroy (my_class);
	 cdev_del(my_cdev);
	 unregister_chrdev_region(mydev,1);
	 kfree(char_device_buf);
	 printk(KERN_INFO "\n Driver unregistered \n");
}
module_init(char_dev_init);
module_exit(char_dev_exit);

MODULE_AUTHOR("Sathish Reddy C");
MODULE_DESCRIPTION("Character Device Driver - Test");
MODULE_LICENSE("GPL");
