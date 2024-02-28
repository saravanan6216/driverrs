#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>

void func(void);

static int val=100;

module_param(val,int,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );

MODULE_PARM_DESC(val,"this is my int variable");


void func(void)
{	
	printk("Function invoked \n");
	printk("Val = %d\n",val);
}

static int __init hellowolrd_init(void) {
   printk("Hello world loaded!\n");
   func();
    return 0;
}

static void __exit hellowolrd_exit(void) {
    printk("End of the world\n");
}

module_init(hellowolrd_init);
module_exit(hellowolrd_exit);
MODULE_AUTHOR("Sathish <csathish.micro@gmail.com>");
MODULE_LICENSE("GPL");
