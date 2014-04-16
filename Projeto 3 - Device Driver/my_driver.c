#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/config.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/version.h>
#include <asm/system.h>
#include <asm/uaccess.h>

//http://www.freesoftwaremagazine.com/articles/drivers_linux
//http://www.opensourceforu.com/2011/08/io-control-in-linux/

static dev_t d_number;				/* Número do dispositivo. */
static struct cdev c_dev;			/* Estrutura do dispositivo. */
static struct class *my_class;	/* Classe do dispositivo. */

static int *v;

static int memory_open(struct inode *inode, struct file *filp) 
{
	/* Sucesso. */
	printk(KERN_INFO "Driver: open()\n");
	return 0;
}

static int memory_release(struct inode *inode, struct file *filp) 
{
	/* Sucesso. */
	printk(KERN_INFO "Driver: close()\n");
	return 0;
}

static ssize memory_read(struct file *filp, char *buf, size_t count, loff_t *f_pos) 
{
	printk(KERN_INFO "Driver: read()\n");
	
}

static ssize memory_write(struct file *filp, char *buf, size_t count, loff_t *f_pos) 
{
	printk(KERN_INFO "Driver: write()\n");
	char *kernelBuf;
	kernelBuf = kmalloc(sizeof(char)*count, GFP_KERNEL);
	if(kernelBuf && v) {
		if(copy_from_user(kernelBuf, buf, count) == 0) {
			int i, j;
			for(i = 0; i < count; i++) {
				j = (int)kernelBuf[i];
				j -= 65;  /* Letras maiúsculas. */
				if(j >= 0) {
					if(j > 25) {
						j -= 32;  /* Letras minúsculas. */
					}
					if(j >= 0 && j <= 25) {
						v[j]++;
					}
				}			
			}
			kfree(kernelBuf);
		}
		else {
			kfree(kernelBuf);
			return -EFAULT;
		}
	}
	return 1;		
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
static int my_ioctl(struct inode *i, struct file *f, unsigned int cmd, unsigned long arg)
#else
static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
#endif
{
	
}

/* Estrutura que declara as funções usuais de acesso a arquivos. */
struct file_operations memory_fops = {
	owner:		THIS_MODULE,
	read:			memory_read,
	write:		memory_write,
	open:			memory_open,
	release:		memory_release,
	ioctl:		my_ioctl
	
};

static int __init driver_init(void)
{
	printk(KERN_INFO "Driver iniciado!!!\n");
	if(alloc_chrdev_region(&d_number, 0, 1, "my_driver") < 0) {
		return -1;
	}
	if((my_class = class_create(THIS_MODULE, "driver_504")) == NULL) {
		unregister_chrdev_region(d_number, 1);
		return -1;
	}
	if(device_create(my_class, NULL, d_number, NULL, "my_driver") == NULL) {
		class_destroy(my_class);
		unregister_chrdev_region(d_number, 1);
		return -1;
	}
	cdev_init(&c_dev, &memory_fops);
	if(cdev_add(&c_dev, d_number, 1) == -1) {
		device_destroy(my_class, d_number);
		class_destroy(my_class);
		unregister_chrdev_region(d_number, 1);
		return -1;
	}
	v = kmalloc(sizeof(int)*26, GFP_KERNEL);
	if(!v) {
		device_destroy(my_class, d_number);
		class_destroy(my_class);
		unregister_chrdev_region(d_number, 1);
		return -1;
	}
	memset(v, 0, 26);
	return 0;
}

static void __exit driver_exit(void) {
	printk(KERN_INFO "Fim do driver!!!\n");
	cdev_del(&c_dev);
	device_destroy(my_class, d_number);
	class_destroy(my_class);
	unregister_chrdev_region(d_number, 1);
	if(v) {
		kfree(v);
	}
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ulisses / Luiz / José");
MODULE_DESCRIPTION("Projeto 3 de MC504");
module_init(driver_init);
module_exit(driver_exit);

