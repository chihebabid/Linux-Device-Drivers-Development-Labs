#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>

#define MY_MAJOR_NUM 190
static struct cdev my_dev;
static char ker_buf[100];   // Array to store elements of the stack
static int top;

static int my_open(struct inode *inod, struct file *fil);
static ssize_t my_read(struct file *filep, char *buf, size_t len, loff_t *off);
static ssize_t my_write(struct file *flip, const char *buff, size_t len,loff_t *off);
static int my_release(struct inode *inod, struct file *fil);
static long my_ioctl(struct file *filep, unsigned int cmd, unsigned long arg);

static struct file_operations my_dev_fops = {
	.read = my_read,
	.write = my_write,
	.open = my_open,
	.release = my_release,
	.unlocked_ioctl = my_ioctl,
};

static int my_init(void)   {
	int ret;
	dev_t dev = MKDEV(MY_MAJOR_NUM, 0);
	pr_info("chardev driver init\n");

	ret = register_chrdev_region(dev, 1, "my_char_device");
	if (ret < 0) {
		pr_err("Unable to allocate mayor number %d\n", MY_MAJOR_NUM);
		return ret;
	}

	cdev_init(&my_dev, &my_dev_fops);
	ret= cdev_add(&my_dev, dev, 1);
	if (ret < 0) {
		unregister_chrdev_region(dev, 1);
		pr_err("Unable to add cdev\n");
		return ret;
	}
	top=0;
	pr_info("Device file is associated...\n");
	return 0;
}

static void __exit my_exit(void) {//fonction de clôture
	pr_info("chardev driver exit\n");
	cdev_del(&my_dev);
	unregister_chrdev_region(MKDEV(MY_MAJOR_NUM, 0), 1);
}

static int my_open(struct inode *inod, struct file *fil) {
	pr_info("chardev device opened");
	return 0;
}

static ssize_t my_read(struct file *filep, char *buf, size_t len, loff_t *off) {
	int i = 0;
	for (i = 0; i < len && top > 0; ++i) {
		put_user(*(ker_buf + top - 1), buf + i);
		top--;
	}

	return i;
}

static ssize_t my_write(struct file *flip, const char *buf, size_t len, loff_t *off) {
	int i = 0;
	for(i = 0; i < len && top < 100; i++) {
		get_user(ker_buf[top], buf + i);
		top++;
	}
	return len;
}

static int my_release(struct inode *inod, struct file *fil) {
	pr_info("chardev device closed\n");
	return 0;
}

static long my_ioctl(struct file *filep, unsigned int cmd,unsigned long arg) {
	if (cmd == 1) {
		if (arg == 1) top = 0;
		else if (arg == 2)	return top;
	}
	return top;
}

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("C.A. ABID");
MODULE_DESCRIPTION("Chardev module simulating a character stack...");
