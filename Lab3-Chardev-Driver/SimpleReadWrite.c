#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>


#define MY_MAJOR_NUM 190 // Major number
static struct cdev my_dev;
static char ker_buf[100];
static bool ker_read=false;


static int my_dev_open(struct inode *inode, struct file *file)
{
  pr_info("my_dev_open() is called.\n");
  ker_read=false;
  return 0;
}

static int my_dev_close(struct inode *inode, struct file *file)
{
  pr_info("my_dev_close() is called.\n");
  return 0;
}

static ssize_t my_dev_read(struct file *filp, char *buf,size_t count, loff_t *pos) {
  int len=ker_read ? 0: strlen(ker_buf);
  pr_info("Longueur %d , count %d, long2 %d\n",len,count,strlen(ker_buf));
  copy_to_user(buf,ker_buf,len);
  ker_read=true;
  return len;
}

static ssize_t my_dev_write(struct file *flip,const char *buf,size_t len,loff_t *off) {
    copy_from_user(ker_buf,buf,len);
    ker_buf[len]='\0';
    ker_read=false;
    return len;
}

/* Declare a file_operations structure */
static const struct file_operations my_dev_fops = {
  .owner = THIS_MODULE,
  .open = my_dev_open,
  .release = my_dev_close,
  .write = my_dev_write,
  .read = my_dev_read,
};


static int __init hello_init(void)
{
  int ret;
  /* Get first device identifier */
  dev_t dev = MKDEV(MY_MAJOR_NUM, 0);
  pr_info("chardev driver init\n");
  /* Allocate a number of devices */
  ret = register_chrdev_region(dev, 1, "my_char_device");
  if (ret < 0) {		
    pr_err("Unable to allocate mayor number %d\n", MY_MAJOR_NUM);
    return ret;
  }
  /* Initialize the cdev structure and add it to kernel space */
  cdev_init(&my_dev, &my_dev_fops);
  ret= cdev_add(&my_dev, dev, 1);
  if (ret < 0) {		
   unregister_chrdev_region(dev, 1);		
   pr_err("Unable to add cdev\n");
   return ret;
  }
  ker_buf[0]='\0';
  return 0;
}

static void __exit hello_exit(void)
{
  pr_info("chardev driver exit\n");
  cdev_del(&my_dev);
  unregister_chrdev_region(MKDEV(MY_MAJOR_NUM, 0), 1);
}
module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("C.A. ABID");
MODULE_DESCRIPTION("This is a chardev module!");
