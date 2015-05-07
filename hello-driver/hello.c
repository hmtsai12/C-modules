#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/sched.h> /*for current global variable*/
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include "hello.h"

MODULE_DESCRIPTION("Hello world");
MODULE_AUTHOR("HM Tsai");
MODULE_LICENSE("GPL");

static unsigned int hello_major = 0;
static int hello_devs = 2;
//static struct cdev *pHelloDev->Hello_cdev = NULL;
//static struct class *pHelloDev->pHello_class = NULL;

struct hello_pipe
{
	struct cdev Hello_cdev;
	struct class *pHello_class;
	char data[1024];
	int len;
	struct semaphore sem;
	wait_queue_head_t inq, outq;
	struct kfifo *pFifo;
	struct task_struct *pThread;
	struct mutex lock;
	//struct device pDevice[hello_devs];
	//struct device *Device;
};

struct hello_pipe *pHelloDev = NULL;

long hello_zero_ioctl(struct file *file, unsigned int req, unsigned long arg)
{
	hello_transfer data;
	struct hello_pipe *pDevice;
	long retLen;
	struct inode *inode = file->f_path.dentry->d_inode;
	int minor = MINOR(inode->i_rdev);
	pDevice = (struct hello_pipe *)file->private_data;
	printk("minor = %d[0x%x]\n", minor, &pDevice->lock);
	mutex_lock(&pDevice->lock);
	if(_IOC_TYPE(req) != HELLO_IOC_MAGIC)
		return -ENOTTY;
	if(_IOC_NR(req) > HELLO_IOC_MAXNR)
		return -ENOTTY;
	copy_from_user(&data, (hello_transfer *)arg, sizeof(hello_transfer));
	switch (req)
	{
		case HELLO_READ:
			printk("read command\n");
			copy_to_user((char *)data.rx_buf, pHelloDev->data, pHelloDev->len);
			data.len = pHelloDev->len;
			retLen = pHelloDev->len;
			break;
		case HELLO_WRITE:
			printk("write command\n");
			pHelloDev->len = data.len;
			copy_from_user(pHelloDev->data, (char *)data.tx_buf, data.len);
			retLen = pHelloDev->len;
			break;
		default:
			printk("none define command\n");
			retLen = 0;
			break;
	};
	mutex_unlock(&pDevice->lock);
	return retLen;
}

int hello_zero_open(struct inode *inode, struct file *file)
{
	struct hello_pipe *pDevice;
	pDevice = container_of(inode->i_cdev, struct hello_pipe, Hello_cdev);
	file->private_data = pDevice;
	printk("%s:major %d minor %d(pid %d)\n", __func__, imajor(inode), iminor(inode), current->pid);
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
	printk("%s\n", __func__);
	return 0;
}
long hello_one_ioctl(struct file *file, unsigned int req, unsigned long arg)
{
	hello_transfer data;
	struct hello_pipe *pDevice;
	long retLen;
	struct inode *inode = file->f_path.dentry->d_inode;
	int minor = MINOR(inode->i_rdev);

	pDevice = (struct hello_pipe *)file->private_data;
	printk("minor = %d[0x%x]\n", minor, &pDevice->lock);
	mutex_lock(&pDevice->lock);
	if(_IOC_TYPE(req) != HELLO_IOC_MAGIC)
		return -ENOTTY;
	if(_IOC_NR(req) > HELLO_IOC_MAXNR)
		return -ENOTTY;
	copy_from_user(&data, (hello_transfer *)arg, sizeof(hello_transfer));
	switch (req)
	{
		case HELLO_READ:
			printk("read command\n");
			copy_to_user((char *)data.rx_buf, pHelloDev->data, pHelloDev->len);
			data.len = pHelloDev->len;
			retLen = pHelloDev->len;
			break;
		case HELLO_WRITE:
			printk("write command\n");
			pHelloDev->len = data.len;
			copy_from_user(pHelloDev->data, (char *)data.tx_buf, data.len);
			retLen = pHelloDev->len;
			break;
		default:
			printk("none define command\n");
			retLen = 0;
			break;
	};
	mutex_unlock(&pDevice->lock);
	return retLen;
}

int hello_one_open(struct inode *inode, struct file *file)
{
	struct hello_pipe *pDevice;
	pDevice = container_of(inode->i_cdev, struct hello_pipe, Hello_cdev);
	file->private_data = pDevice;
	printk("%s:major %d minor %d(pid %d)\n", __func__, imajor(inode), iminor(inode), current->pid);
	return 0;
}

int hello_one_release(struct inode *inode, struct file *file)
{
	printk("%s:major %d minor %d(pid %d)\n", __func__, imajor(inode), iminor(inode), current->pid);
	return 0;
}
ssize_t hello_one_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	printk("%s\n", __func__);
	return 0;
}
ssize_t hello_one_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	printk("%s\n", __func__);
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
	struct inode *inode = file->f_path.dentry->d_inode;
	int minor = MINOR(inode->i_rdev);
	printk("minor = %d\n", minor);
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
	
	pHelloDev = kmalloc(sizeof(struct hello_pipe), GFP_KERNEL);
	if(pHelloDev == NULL)
		goto error;
	memset(pHelloDev, 0x00, sizeof(struct hello_pipe));

	alloc_ret = alloc_chrdev_region(&dev, 0, hello_devs, "hello");
	if(alloc_ret)
		goto error;
	
	cdev_init(&pHelloDev->Hello_cdev, &hello_fops);
	if(&pHelloDev->Hello_cdev == NULL)
		goto error;

	hello_major = major = MAJOR(dev);
	pr_info("hello major = %d\n", major);

	pHelloDev->Hello_cdev.owner = THIS_MODULE;

	cdev_err = cdev_add(&pHelloDev->Hello_cdev, dev, hello_devs);
	if(cdev_err)
	{
		goto error;
	}

	pHelloDev->pHello_class = class_create(THIS_MODULE, "hello");
	if(IS_ERR(pHelloDev->pHello_class))
	{
		goto error;
	}

	for(minor = 0; minor < hello_devs; minor++)
		device_create(pHelloDev->pHello_class, NULL, MKDEV(hello_major, minor), NULL, "hello""%d",  minor);

	mutex_init(&pHelloDev->lock);
	return 0;
error:
	pr_info("get error install hello driver\n");

	if(pHelloDev->pHello_class != NULL)
		class_destroy(pHelloDev->pHello_class);
	if(cdev_err == 0)
		cdev_del(&pHelloDev->Hello_cdev);

	if(alloc_ret == 0)
		unregister_chrdev_region(dev, hello_devs);

	if(pHelloDev != NULL)
		kfree(pHelloDev);
	return -1;
}

static void __exit hello_exit(void)
{
	int minor = 0;
	dev_t dev = MKDEV(hello_major, 0);

	for(minor = 0; minor < hello_devs; minor++)
		device_destroy(pHelloDev->pHello_class, MKDEV(hello_major, minor));
	class_destroy(pHelloDev->pHello_class);
	cdev_del(&pHelloDev->Hello_cdev);
	unregister_chrdev_region(dev, hello_devs);
	kfree(pHelloDev);
	printk(KERN_INFO"Goodbye\n");
}

module_init(hello_init);
module_exit(hello_exit);
