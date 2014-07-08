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
#include <linux/ioctl.h>
 
#define QUERY_SET_TEXT _IOR('q', 1, char *)
#define QUERY_CLR_TEXT _IO('q', 2)
#define QUERY_GET_COUNT _IOW('q', 3, char *)
 
#define FIRST_MINOR 0
#define MINOR_CNT 1



static dev_t first; // Global variable for the first device number
static struct cdev c_dev; // Global variable for the character device structure
static struct class *cl; // Global variable for the device class

static char c;
static char *q;
static int *v;

static int my_open(struct inode *i, struct file *f)
{
  printk(KERN_INFO "Driver: open()\n");
  return 0;
}
static int my_close(struct inode *i, struct file *f)
{
  printk(KERN_INFO "Driver: close()\n");
  return 0;
}
 
static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_INFO "Driver: read()\n");
    if (copy_to_user(buf, &c, 1) != 0)
        return -EFAULT;
    else
        return 1;
}
static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_INFO "Driver: write()\n");
    if (copy_from_user(&c, buf + len - 1, 1) != 0)
        return -EFAULT;
    else
        return len;
};

static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
    int i, j;
    switch (cmd)
    {
        case QUERY_GET_COUNT:
            if(v == NULL)
              	v = kmalloc(sizeof(int)*26, GFP_KERNEL);
			      for(i = 0; i < 500; i++) {
				      j = (int)q[i];
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
            if (copy_to_user((int *) arg, v, sizeof(int)*26))
            {
                return -EACCES;
            }
            break;
        case QUERY_CLR_TEXT:
            if(q != NULL)
              kfree(q);
            break;
        case QUERY_SET_TEXT:
            if(q == NULL)
              q = kmalloc(sizeof(char) * 500 ,GFP_KERNEL);
            if (copy_from_user(&q, (char *)arg, sizeof(char) *500))
            {
                return -EACCES;
            }
            break;
        default:
            return -EINVAL;
    }
 
    return 0;
}

static struct file_operations pugs_fops =
  {
      .owner = THIS_MODULE,
      .open = my_open,
      .release = my_close,
      .read = my_read,
      .write = my_write,
      .unlocked_ioctl = my_ioctl
  };

static int __init ofcd_init(void) /* Constructor */
{
  printk(KERN_INFO "Namaskar: ofcd-lastchar registered");
  if (alloc_chrdev_region(&first, 0, 1, "ofcd-lastchar") < 0)
    {
      return -1;
    }
  if ((cl = class_create(THIS_MODULE, "chardrv")) == NULL)
    {
      unregister_chrdev_region(first, 1);
      return -1;
    }
  if (device_create(cl, NULL, first, NULL, "ofcd-lastchar") == NULL)
    {
      class_destroy(cl);
      unregister_chrdev_region(first, 1);
      return -1;
    }
  cdev_init(&c_dev, &pugs_fops);
  if (cdev_add(&c_dev, first, 1) == -1)
    {
      device_destroy(cl, first);
      class_destroy(cl);
      unregister_chrdev_region(first, 1);
      return -1;
    }
  return 0;
}

static void __exit ofcd_exit(void) /* Destructor */
{
  cdev_del(&c_dev);
  device_destroy(cl, first);
  class_destroy(cl);
  unregister_chrdev_region(first, 1);
  printk(KERN_INFO "Alvida: ofcd-lastchar unregistered");
}

module_init(ofcd_init);
module_exit(ofcd_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anil Kumar Pugalia <email_at_sarika-pugs_dot_com>");
MODULE_DESCRIPTION("Our First Character Driver");
