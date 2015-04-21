#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>

MODULE_DESCRIPTION("Hello world");
MODULE_AUTHOR("HM Tsai");
MODULE_LICENSE("GPL");

static unsigned int hello_major = 0;
static int hello_devs = 2;
static struct cdev *hello_cdev = NULL;
static struct class *hello_class = NULL;

long hello_ioctl(struct file *file, unsigned int req, unsigned long arg)
{
	return 0;
}

int hello_open(struct inode *inode, struct file *file)
{
	return 0;
}

int hello_release(struct inode *inode, struct file *file)
{
	return 0;
}

struct file_operations hello_fops =
{
	owner:	THIS_MODULE,
	unlocked_ioctl:	hello_ioctl,
	open:		hello_open,
	release:	hello_release,
};
static int __init hello_init(void)
{
	dev_t dev = MKDEV(hello_major, 0);
	int alloc_ret = 0;
	int major, minor;
	int cdev_err = 0;
	pr_info("Hello, world\n");
	minor = 0;
	alloc_ret = alloc_chrdev_region(&dev, 0, hello_devs, "hello");
	if(alloc_ret)
		goto error;
	
	hello_cdev = cdev_alloc();
	if(hello_cdev == NULL)
		goto error;

	hello_major = major = MAJOR(dev);
	pr_info("hello major = %d\n", major);

	hello_cdev->owner = THIS_MODULE;
	hello_cdev->ops = &hello_fops;

	cdev_err = cdev_add(hello_cdev, dev, hello_devs);
	if(cdev_err)
	{
		goto error;
	}

	hello_class = class_create(THIS_MODULE, "hello");
	if(IS_ERR(hello_class))
	{
		goto error;
	}

	device_create(hello_class, NULL, MKDEV(hello_major, 0), NULL, "hello""%d",  minor);

	return 0;
error:
	pr_info("get error install hello driver\n");
	if(alloc_ret == 0)
		unregister_chrdev_region(dev, hello_devs);
	return -1;
}

static void __exit hello_exit(void)
{
	dev_t dev = MKDEV(hello_major, 0);

	device_destroy(hello_class, MKDEV(hello_major, 0 ));
	class_destroy(hello_class);
	cdev_del(hello_cdev);
	kfree(hello_cdev);
	unregister_chrdev_region(dev, hello_devs);
	printk(KERN_INFO"Goodbye\n");
}

module_init(hello_init);
module_exit(hello_exit);
