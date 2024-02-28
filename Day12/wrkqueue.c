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
#include <linux/workqueue.h>
#include <linux/sched.h>

#define SHARED_IRQ 1

#define DELAY 100

static int irq=SHARED_IRQ,my_dev,irq_counter=0;
module_param(irq,int,S_IRUGO);

dev_t dev = 0;

 
static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);
 

void wq_func(void *arg);
 
DECLARE_WORK(wq,(work_func_t)wq_func);


void wq_func(void *arg)
{
	irq_counter++;
	pr_info("WorkQueue:ISR counter = %d\n",irq_counter);
}

static irqreturn_t my_irq(int irq,void *dev_id)
{
	schedule_work(&wq);
	return IRQ_NONE;
}	
 
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


