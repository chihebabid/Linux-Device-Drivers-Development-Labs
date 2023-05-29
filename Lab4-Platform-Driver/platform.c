#include <linux/module.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/miscdevice.h>

struct mydev_misc_t {
    struct miscdevice mymisc_device;
    const char *device_name;
};

/*
 * Read function
 */
static ssize_t mydev_read(struct file *file, char __user *buff, size_t count, loff_t *ppos)
{
    struct mydev_misc_t *mydev_data;
    pr_info("mydev_read() is called.\n");
    mydev_data = container_of(file->private_data, struct mydev_misc_t, mymisc_device);
    if (*ppos == 0){
        if(copy_to_user(buff, mydev_data->device_name, strlen(mydev_data->device_name))) {
            pr_info("Failed to return name to user space\n");
            return -EFAULT;
        }
        put_user('\n',buff+strlen(mydev_data->device_name));
        *ppos+=1;
        return strlen(mydev_data->device_name)+1;
    }
    return 0;
}

static const struct file_operations mydev_fops = {
    .owner = THIS_MODULE,
    .read = mydev_read,
};

static int mydev_probe(struct platform_device *pdev)
{
    int ret_val;
    struct mydev_misc_t *mydev;

    pr_info("mydev_probe() function is called.\n");
    mydev = devm_kzalloc(&pdev->dev, sizeof(struct mydev_misc_t), GFP_KERNEL);
    of_property_read_string(pdev->dev.of_node, "label", &mydev->device_name); /* Different label for each misc device*/

    mydev->mymisc_device.minor = MISC_DYNAMIC_MINOR;
    mydev->mymisc_device.name = mydev->device_name;
    mydev->mymisc_device.fops = &mydev_fops;

    ret_val = misc_register(&mydev->mymisc_device);
    if (ret_val) return ret_val; /* misc_register returns 0 if success */
    platform_set_drvdata(pdev, mydev);
    return 0;
}

static int mydev_remove(struct platform_device *pdev)
{
    struct mydev_misc_t  *misc_device = platform_get_drvdata(pdev);
    pr_info("mydev_remove() function is called.\n");
    misc_deregister(&misc_device->mymisc_device);
    return 0;
}

/* Declare a list of devices supported by the driver */
static const struct of_device_id my_of_ids[] = {
{ .compatible = "training,helloplatform"},
{},
};

MODULE_DEVICE_TABLE(of, my_of_ids);
static struct platform_driver my_platform_driver = {
    .probe = mydev_probe,
    .remove = mydev_remove,
    .driver = {
        .name = "helloplatform",
        .of_match_table = my_of_ids,
        .owner = THIS_MODULE,
    }
};

/*
 * Initialize
 */
static int mydev_init(void) {
    int ret_val;
    pr_info("driver init\n");
    ret_val = platform_driver_register(&my_platform_driver);
    if (ret_val !=0)
    {
        pr_err("platform value returned %d\n", ret_val);
        return ret_val;
    }
    return 0;
}

/*
 * Exit
 */
static void mydev_exit(void)
{
    pr_info("driver exit\n");
    platform_driver_unregister(&my_platform_driver);
}

module_init(mydev_init);
module_exit(mydev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("C.A. ABID");
MODULE_DESCRIPTION("This is a platform driver that manages two misc devices");
