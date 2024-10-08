#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>


#define DEV_MEMORY_SIZE 512

/*Dummy character device memory*/
char device_buffer[DEV_MEMORY_SIZE];

/*The dev_t holds the device number(major and minor number)*/
dev_t device_number;

/*Create the cdev variable*/
struct cdev chardrv_cdev;

struct class *class_chardrv;

struct device *device_char;

ssize_t chardrv_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
	printk(KERN_INFO "chardrv_read : The read requested for %zu bytes \n", count);
	printk(KERN_INFO "chardrv_read : Current file position value in a memory : %lld \n", *f_pos);
	
	/*adjust the count*/
	if((*f_pos + count) > DEV_MEMORY_SIZE)
	{
		count = DEV_MEMORY_SIZE - *f_pos;
	}
	
	/*copy to user */
	if(copy_to_user(buff,&device_buffer[*f_pos],count))
	{
		return -EFAULT;
	}
	
	/*updatecurrent file position */
	*f_pos += count;
	
	printk(KERN_INFO "chardrv_read : The number bytes successfully read : %zu \n", count);
	printk(KERN_INFO "chardrv_read : Updated file position value in a memory : %lld \n", *f_pos);

	/*return nubers of bytes which have been successfully read */
	return count;
}

ssize_t chardrv_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{
	printk(KERN_INFO "chardrv_write : The Write requested for %zu bytes \n", count);
	printk(KERN_INFO "chardrv_write : Current file position value in a memory : %lld \n", *f_pos);
	
	/*adjust the count*/
	if((*f_pos + count) > DEV_MEMORY_SIZE)
	{
		count = DEV_MEMORY_SIZE - *f_pos;
	}
	
	if(count == 0)
	{
		return -ENOMEM;
	}
	
	/*copy from user */
	if(copy_from_user(&device_buffer[*f_pos],buff,count))
	{
		return -EFAULT;
	}
	
	/*updatecurrent file position */
	*f_pos += count;
	
	printk(KERN_INFO "chardrv_write : The number bytes successfully written : %zu \n", count);
	printk(KERN_INFO "chardrv_write : Updated file position value in a memory : %lld \n", *f_pos);
	
	/*return nubers of bytes which have been successfully read */
	return count;
}

loff_t chardrv_llseek(struct file *filp, loff_t offset, int whence)
{
	int temp;
	printk(KERN_INFO "chardrv_llseek : Current value of file position : : %lld \n", filp->f_pos);
	switch(whence)
	{
		case SEEK_SET:
			if((offset > DEV_MEMORY_SIZE) || (offset < 0))
			{
				return -EINVAL;
			}
			filp->f_pos = offset;
			break;
			
		case SEEK_CUR:
			temp = filp->f_pos + offset;
			if((temp > DEV_MEMORY_SIZE) || (temp < 0))
			{
				return -EINVAL;
			}
			filp->f_pos = temp;
			break;
			
		case SEEK_END:
			temp = DEV_MEMORY_SIZE + offset;
			if((temp > DEV_MEMORY_SIZE) || (temp < 0))
			{
				return -EINVAL;
			}
			filp->f_pos = temp;
			break;
			
		default:
			return -EINVAL;
	}
	
	printk(KERN_INFO "chardrv_llseek : Updated value of file position : : %lld \n", filp->f_pos);
	
	return filp->f_pos;
}

int chardrv_release(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "%s\n","chardrv_release : release was successful");
	return 0;
}

int chardrv_open(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "%s\n","chardrv_open : open was successful");
	return 0;
}

/*Create the cdev variable*/
struct file_operations chardrv_fops =
{
	.open = chardrv_open,
	.write = chardrv_write,
	.read = chardrv_read,
	.release = chardrv_release,
	.llseek = chardrv_llseek,
	.owner = THIS_MODULE 
}; 
static int __init chardrv_init(void)
{
	int ret;

	/* 1. Dynamically allocate a device number */
	ret = alloc_chrdev_region(&device_number,0,1,"chardevices");
	
	if(ret < 0)
	{
		goto out;
	}	
	/* 2. Initialize the cdev structure with fops*/
	cdev_init(&chardrv_cdev,&chardrv_fops);
	
	/* 3. Register a driver (cdev structure) with VFS */
	chardrv_cdev.owner = THIS_MODULE;
	ret = cdev_add(&chardrv_cdev,device_number,1);
	if(ret < 0)
	{
		goto unreg_chardev;
	}
	/* 4. Create device class under /sys/class */
	class_chardrv = class_create("chardrv_class");
	if(IS_ERR(class_chardrv))
	{
		printk(KERN_INFO "%s\n","Class creation failed\n");
		ret = PTR_ERR(class_chardrv);
		goto cdev_del;
	}	


	/* 5. populate the syfswith device information */
	device_char = device_create(class_chardrv,NULL,device_number,NULL,"chardev");
	if(IS_ERR(class_chardrv))
        {
                printk(KERN_INFO "%s\n","Device creation failed\n");
                ret = PTR_ERR(device_char);
                goto class_del;
        }    

	return 0;

class_del:
	class_destroy(class_chardrv);
cdev_del:
	cdev_del(&chardrv_cdev);
unreg_chardev:
	unregister_chrdev_region(device_number,1);
out:
	return ret;


}

static void __exit chardrv_exit(void)
{
	device_destroy(class_chardrv,device_number);
	class_destroy(class_chardrv);
	cdev_del(&chardrv_cdev);
	unregister_chrdev_region(device_number,1);
}


module_init(chardrv_init);
module_exit(chardrv_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hare Krishna");
MODULE_DESCRIPTION("Char Device demonstration");
