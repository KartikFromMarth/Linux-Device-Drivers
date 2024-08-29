#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>              //copy_to/from_user()
#include <linux/ioctl.h>
#include <linux/err.h>


/*
**Funtion declaration 
*/
static int 	__init sample_driver_init(void);
static void	__exit sample_driver_exit(void);
static int	sample_driver_open(struct inode *inode, struct file *file);
static int	sample_driver_release(struct inode *inode, struct file *file);
static ssize_t  sample_driver_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t  sample_driver_write(struct file *filp, const char *buf, size_t len, loff_t * off);
static long	sample_driver_ioctl(struct file *file, unsigned int cmd, unsigned long arg);


#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)
 
int32_t value = 0;


/*The dev_t holds the device number(major and minor number)*/
dev_t device_number;

/*Create the cdev variable*/
struct cdev cdev_sample_drv;

struct class *class_sample_drv;

struct device *device_sample_drv;







/*
**File operation structure
*/
static struct file_operations fops =
{
	.owner 		= THIS_MODULE,
	.read  		= sample_driver_read,
	.write 		= sample_driver_write,
	.open  		= sample_driver_open,
	.release 	= sample_driver_release,
	.unlocked_ioctl = sample_driver_ioctl,
};

/*This function will be called when we open the device file */
static int sample_driver_open(struct inode *inode, struct file *file)
{
	pr_info("Device File Opened...!!!\n");
        return 0;

}

/*This function will be called when we close the device file */
static int sample_driver_release(struct inode *inode, struct file *file)
{
        pr_info("Device File Closed...!!!\n");
        return 0;
}

/*This function will be called when we read the device file */
static ssize_t sample_driver_read(struct file *filp, char __user *buf, size_t len,loff_t * off)
{
        pr_info("Read Function\n");
        return 0;
}

/*This function will be called when we write the device file */
static ssize_t sample_driver_write(struct file *filp, const char *buf, size_t len, loff_t * off)
{
	pr_info("Write function\n");
        return len;
}

/*This function will be called when we write IOCTL on the Device file */
static long sample_driver_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch(cmd)
	 {
                case WR_VALUE:
                        if( copy_from_user(&value ,(int32_t*) arg, sizeof(value)) )
                        {
                                pr_err("Data Write : Err!\n");
                        }
                        pr_info("Value = %d\n", value);
                        break;
                case RD_VALUE:
                        if( copy_to_user((int32_t*) arg, &value, sizeof(value)) )
                        {
                                pr_err("Data Read : Err!\n");
                        }
                        break;
                default:
                        pr_info("Default\n");
                        break;
        }
        return 0;
}

static int __init sample_driver_init(void)
{
	/* 1. Dynamically allocate a device number */
	if((alloc_chrdev_region(&device_number,0,1,"chardevices") < 0))
	{
		pr_err("Cannot allocate major number\n");
                return -1;
	}
	
	pr_info("Major = %d Minor = %d \n",MAJOR(device_number), MINOR(device_number));
	
	/* 2. Initialize the cdev structure with fops*/
	cdev_init(&cdev_sample_drv,&fops);
	
	/* 3. Register a driver (cdev structure) with VFS */
	cdev_sample_drv.owner = THIS_MODULE;
	if((cdev_add(&cdev_sample_drv,device_number,1) < 0))
	{
	    pr_err("Cannot add the device to the system\n");
            goto r_class;
	}
	
	
	/* 4. Create device class under /sys/class */
	if(IS_ERR(class_sample_drv = class_create("chardrv_class")))
	{
            pr_err("Cannot create the struct class\n");
            goto r_class;
	}
	
	
	if(IS_ERR(device_create(class_sample_drv,NULL,device_number,NULL,"chardev_device")))
	{
	    pr_err("Cannot create the Device 1\n");
            goto r_device;
	}
	
	pr_info("Device Driver Insert...Done!!!\n");
        return 0;
        
r_device:
        class_destroy(class_sample_drv);
r_class:
        unregister_chrdev_region(device_number,1);
        return -1;


}

static void __exit sample_driver_exit(void)
{
	device_destroy(class_sample_drv,device_number);
	class_destroy(class_sample_drv);
	cdev_del(&cdev_sample_drv);
	unregister_chrdev_region(device_number,1);
	pr_info("Device Driver Remove...Done!!!\n");

}

module_init(sample_driver_init);
module_exit(sample_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hare Krishna");
MODULE_DESCRIPTION("Simple Linux device driver (IOCTL)");
