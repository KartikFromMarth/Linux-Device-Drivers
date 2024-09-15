#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>

static int my_variable = 0;


static ssize_t my_variable_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", my_variable);
}

static ssize_t my_variable_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) 
{
    sscanf(buf, "%du", &my_variable);
    return count;
}

static struct kobject *my_kobject;

static struct kobj_attribute my_variable_attribute = __ATTR(my_variable, 0660, my_variable_show, my_variable_store);


static int __init my_driver_init(void)
{
	int error;
	
	my_kobject = kobject_create_and_add("my_kobject", kernel_kobj);
	
	if(!my_kobject)
	{
		return -ENOMEM;
	}
	
	error = sysfs_create_file(my_kobject, &my_variable_attribute.attr);
	if (error) 
	{
		pr_debug("failed to create the my_variable file in /sys/kernel/my_kobject\n");
		kobject_put(my_kobject);
        	return error;
       }
	pr_debug("sysfs attribute created successfully\n");
	return 0;
	
	
	
}

static void __exit my_driver_exit(void)
{
	kobject_put(my_kobject);
	pr_debug("sysfs attribute removed successfully\n");
}



module_init(my_driver_init);
module_exit(my_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HARE KRISHNA");
MODULE_DESCRIPTION("A simple Linux driver with sysfs support");
