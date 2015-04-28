#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/sched.h> /*for current global variable*/


MODULE_DESCRIPTION("Hello world");
MODULE_AUTHOR("HM Tsai");
MODULE_LICENSE("GPL");

static unsigned int hello_major = 0;
static int hello_devs = 2;
static struct cdev *hello_cdev = NULL;
static struct class *hello_class = NULL;

long hello_zero_ioctl(struct file *file, unsigned int req, unsigned long arg)
{
	return 0;
}

int hello_zero_open(struct inode *inode, struct file *file)
{
	return 0;
}

int hello_zero_release(struct inode *inode, struct file *file)
{
	printk("%s:major %d minor %d(pid %d)\n", __func__, imajor(inode), iminor(inode), current->pid);
	return 0;
}
ssize_t hello_zero_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	printk("%s\n", __func__);
	return 0;
}

ssize_t hello_zero_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	return 0;
}
long hello_one_ioctl(struct file *file, unsigned int req, unsigned long arg)
{
	return 0;
}

int hello_one_open(struct inode *inode, struct file *file)
{
	printk("%s:major %d minor %d(pid %d)\n", __func__, imajor(inode), iminor(inode), current->pid);
	return 0;
}

int hello_one_release(struct inode *inode, struct file *file)
{
	return 0;
}
ssize_t hello_one_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	printk("%s\n", __func__);
	return 0;
}
ssize_t hello_one_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	return 0;
}
struct file_operations hello_zero_fops =
{
	owner:	THIS_MODULE,
	unlocked_ioctl:	hello_zero_ioctl,
	open:		hello_zero_open,
	release:	hello_zero_release,
	read:		hello_zero_read,
	write:		hello_zero_write,
};

struct file_operations hello_one_fops =
{
	owner:	THIS_MODULE,
	unlocked_ioctl:	hello_one_ioctl,
	open:		hello_one_open,
	release:	hello_one_release,
	read:		hello_one_read,
	write:		hello_one_write,
};

long hello_ioctl(struct file *file, unsigned int req, unsigned long arg)
{
	return 0;
}

int hello_open(struct inode *inode, struct file *file)
{
	printk("%s:major %d minor %d(pid %d)\n", __func__, imajor(inode), iminor(inode), current->pid);
	switch(iminor(inode))
	{
		case 0:
			file->f_op = &hello_zero_fops;
			break;
		case 1:
			file->f_op = &hello_one_fops;
			break;
		default:
			return 0;
	}
	/* reopen for inode*/
	if(file->f_op && file->f_op->open)
		return file->f_op->open(inode, file);
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

	for(minor = 0; minor < hello_devs; minor++)
		device_create(hello_class, NULL, MKDEV(hello_major, minor), NULL, "hello""%d",  minor);

	return 0;
error:
	pr_info("get error install hello driver\n");
	if(alloc_ret == 0)
		unregister_chrdev_region(dev, hello_devs);
	return -1;
}

static void __exit hello_exit(void)
{
	int minor = 0;
	dev_t dev = MKDEV(hello_major, 0);

	for(minor = 0; minor < hello_devs; minor++)
		device_destroy(hello_class, MKDEV(hello_major, minor));
	class_destroy(hello_class);
	cdev_del(hello_cdev);
//	kfree(hello_cdev);
	unregister_chrdev_region(dev, hello_devs);
	printk(KERN_INFO"Goodbye\n");
}

module_init(hello_init);
module_exit(hello_exit);
