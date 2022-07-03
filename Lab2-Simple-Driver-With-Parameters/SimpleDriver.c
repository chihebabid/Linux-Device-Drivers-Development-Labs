#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>


static char *name="XXXXXXXXXXXXXXX";
module_param(name, charp, 0644);
MODULE_PARM_DESC(param, "Your name...");

static int number=5;
module_param(number, int, 0644);
MODULE_PARM_DESC(param, "Your chosen number...");

static int __init hello_init(void)
{
    pr_info("Module is loaded!\n");
    pr_info("Your name is %s\n",name);
    pr_info("The number you've chosen :%d\n",number);
    return 0;
}

static void __exit hello_exit(void)
{	    
    pr_info("Module is being unloaded!\n");
    pr_info("Good bye %s!\n",name);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("C.A. ABID");
MODULE_DESCRIPTION("Simple drivers with parameters...");
MODULE_LICENSE("GPL");
