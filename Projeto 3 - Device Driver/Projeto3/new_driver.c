#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/errno.h>

#ifndef QUERY_IOCTL_H
#define QUERY_IOCTL_H
#include <linux/ioctl.h>

typedef struct
{
	int count[26];
	char text[500];
} query_arg_t;
 
#define QUERY_GET_COUNT _IOR('q', 1, query_arg_t *)
#define QUERY_CLR_TEXT _IO('q', 2)
#define QUERY_SET_TEXT _IOW('q', 3, query_arg_t *)
 
#endif

static dev_t device_number;			
static struct cdev c_dev;
static struct class *my_class;

static int *count;
static char *text;

static int memory_open(struct inode *i, struct file *f) 
{
	/* Sucesso. */
	printk(KERN_INFO "Driver: open!!!");
	return 0;
}

static int memory_close(struct inode *i, struct file *f) 
{
	/* Sucesso. */
	printk(KERN_INFO "Driver: close!!!");
	return 0;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
static int my_ioctl(struct inode *i, struct file *f, unsigned int cmd, unsigned long arg)
#else
static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
#endif
{
	int i, j;
	query_arg_t q;
	switch(cmd) {
		case QUERY_GET_COUNT: {
			for(i = 0; i < 26; i++) {		
					q.count[i] = count[i];
			}
			if(copy_to_user((query_arg_t *)arg, &q, sizeof(query_arg_t))) {
             return -EACCES;
         }
         break;
		}
		case QUERY_CLR_TEXT: {
			memset(count, 0, sizeof(int)*26);
			memset(text, '\0', 500);
			break;
		}
		case QUERY_SET_TEXT: {
			if(copy_from_user(&q, (query_arg_t *)arg, sizeof(query_arg_t))) {
    			return -EACCES;
			}
			for(i = 0; i < 500; i++) {
				text[i] = q.text[i];
				if(text[i] == '\0') {
					break;
				}
				j = (int)text[i];
				j -= 65;
				if(j >= 0) {
					if(j > 25) {
						j -= 32;
					}
					if(j >= 0 && j <= 25) {
						count[j]++;
					}
				}			
			}
		}
		default: break;
	}
	return 0;
}

static struct file_operations memory_fops =
{
	.owner 	= THIS_MODULE,
   .open 	= memory_open,
   .release = memory_close,
   #if(LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
		.ioctl = my_ioctl
   #else
		.unlocked_ioctl = my_ioctl
   #endif
};

static int __init my_driver_init(void)
{
	printk(KERN_INFO "Driver registrado!!!");
	if(alloc_chrdev_region(&device_number, 0, 1, "new_driver") < 0) {
		return -1;
	}
	if((my_class = class_create(THIS_MODULE, "driver_mc504")) == NULL) {
		unregister_chrdev_region(device_number, 1);
		return -1;
	}
	if(device_create(my_class, NULL, device_number, NULL, "new_driver") == NULL) {
		class_destroy(my_class);
		unregister_chrdev_region(device_number, 1);
		return -1;
	}
	cdev_init(&c_dev, &memory_fops);
	if(cdev_add(&c_dev, device_number, 1) == -1) {
		device_destroy(my_class, device_number);
		class_destroy(my_class);
		unregister_chrdev_region(device_number, 1);
		return -1;
	}
	count = kmalloc(sizeof(int)*26, GFP_KERNEL);
	text = kmalloc(sizeof(char)*500, GFP_KERNEL);
	if(!count || !text) {
		device_destroy(my_class, device_number);
		class_destroy(my_class);
		unregister_chrdev_region(device_number, 1);
		return -1;
	}
	memset(count, 0, sizeof(int)*26);
	memset(text, '\0', 500);
	return 0;
}

static void __exit my_driver_exit(void) 
{
	printk(KERN_INFO "Fim do driver!!!");
	cdev_del(&c_dev);
	device_destroy(my_class, device_number);
	class_destroy(my_class);
	unregister_chrdev_region(device_number, 1);
	if(count) {
		kfree(count);
	}
	if(text) {
		kfree(text);
	}
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ulisses / Luiz / José");
MODULE_DESCRIPTION("Projeto 3 de MC504");
module_init(my_driver_init);
module_exit(my_driver_exit);

