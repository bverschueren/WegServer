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
#include <linux/string.h>

static dev_t first;
static struct cdev c_dev;
static struct class *cl;
static unsigned int seq = 0;

static int ns_open(struct inode *i, struct file *f){
	printk(KERN_INFO "ns_open()\n");
	return 0;
}

static int ns_close(struct inode *i, struct file *f){
	printk(KERN_INFO "ns_close()\n");
	return 0;
}

static ssize_t ns_read(struct file *f, char __user *buf, size_t len, loff_t *off){
	printk(KERN_INFO "ns_read()\n");
	char *out = (char *) kmalloc(10, GFP_KERNEL);
	if (*off < sizeof(*out)){
		char *out = (char *) kmalloc(10, GFP_KERNEL); */
		snprintf(out, sizeof(seq), "%d", seq);
		if(copy_to_user(buf, out, sizeof(out)) != 0)
		{
			return -EFAULT;
		}
		else{
			(*off)++;
			seq++;
			printk(KERN_INFO "strlen(out)=%d", strlen(out)); 
			return strlen(out);
		}
	}
	else{
		return 0;
	}
}

static ssize_t ns_write(struct file *f, const char __user *buf, size_t len, loff_t *off){
	printk(KERN_INFO "ns_write()\n");
	return -ENOTSUPP;
}

static struct file_operations ns_fops = {
	.owner = THIS_MODULE,
	.open = ns_open,
	.release = ns_close,
	.read = ns_read,
	.write = ns_write
};

static int __init nextseq_init(void){
	printk(KERN_INFO "nextseq registered");
	if (alloc_chrdev_region(&first, 0, 1, "next") < 0){
		return -1;
	}
	if ((cl = class_create(THIS_MODULE, "chardrv")) == NULL){
		unregister_chrdev_region(first, 1);
		return -1;
	}
	if (device_create(cl, NULL, first, NULL, "next") == NULL){
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		return 1;
	}
	cdev_init(&c_dev, &ns_fops);
	if (cdev_add(&c_dev, first, 1) == -1){
		device_destroy(cl, first);
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		return -1;
	}
	printk(KERN_INFO "nextseq registered <%d, %d>\n", MAJOR(first), MINOR(first));
	return 0;
}

static void __exit nextseq_exit(void){
	cdev_del(&c_dev);
	device_destroy(cl, first);
	class_destroy(cl);
	unregister_chrdev_region(first, 1);
	printk(KERN_INFO "nextseq unregistered");
}


module_init(nextseq_init);
module_exit(nextseq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("bverschueren");
MODULE_DESCRIPTION("module nextseq");

