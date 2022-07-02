#include <linux/module.h>

// Entry function : this function will be executed when module is loaded
static int __init hello_init(void)
{
	pr_info("Hello world init\n");
	return 0;
}
// Exit function : this function will be executed when module is unloaded
static void __exit hello_exit(void)
{
	pr_info("Hello world exit\n");
}

module_init(hello_init);  // Macro to specify entry function
module_exit(hello_exit);  // Macro to specify exit function

MODULE_LICENSE("GPL");    //  Licence
MODULE_AUTHOR("C.A. ABID");  // Author name
MODULE_DESCRIPTION("Hello world driver...");
