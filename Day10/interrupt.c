#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>                 //kmalloc()
#include <linux/uaccess.h>              //copy_to/from_user()
#include <linux/kthread.h>             //kernel threads
#include <linux/sched.h>               //task_struct 
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/interrupt.h>

#define SHARED_IRQ 18

static int irq=SHARED_IRQ,my_dev,irq_counter=0;
module_param(irq,int,S_IRUGO);

dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
 
static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);
 
static struct task_struct *etx_thread;
 

static irqreturn_t my_irq(int irq,void *dev_id)
{
	irq_counter++;
	pr_info("ISR counter = %d\n",irq_counter);
	return IRQ_NONE;
}	
 
/*
** This function will be called when we open the Device file
*/  
static int etx_open(struct inode *inode, struct file *file)
{
        pr_info("Device File Opened...!!!\n");
        return 0;
}

/*
** This function will be called when we close the Device file
*/   
static int etx_release(struct inode *inode, struct file *file)
{
        pr_info("Device File Closed...!!!\n");
        return 0;
}


static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .open           = etx_open,
        .release        = etx_release,
};

/*
** Module Init function
*/  
static int __init etx_driver_init(void)
{

	if(request_irq(irq,my_irq,IRQF_SHARED,"Bitsilica_IRQ",&my_dev))
		return -1;

        pr_info("IRQ Driver loaded \n");
       return 0;	
}

/*
** Module exit function
*/  
static void __exit etx_driver_exit(void)
{
        synchronize_irq(irq);
	free_irq(irq,&my_dev);
	pr_info("Device Driver Remove...Done!!\n");
}
 
module_init(etx_driver_init);
module_exit(etx_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sathish Reddy");
MODULE_DESCRIPTION("A simple device driver - Kernel Thread");
