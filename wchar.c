#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Joel Savitz <jsavitz@redhat.com>");

#define WCHAR_MAJOR 505
#define WCHAR_MINOR_COUNT 5

#define STR(x) #x

//int register_chrdev_region(dev_t first, unsigned int count, char *name);

int wchar_open(struct inode * inode, struct file * file);
int wchar_close(struct inode * inode, struct file * file);
ssize_t wchar_read(struct file * file, char * __user buf, size_t count, loff_t *offp);
ssize_t wchar_write(struct file * file, const char * __user buf, size_t count, loff_t *offp);

static struct cdev wchar_devs[WCHAR_MINOR_COUNT];

static struct file_operations wchar_fops = {
	.read = wchar_read,
	.write = wchar_write,
	.open = wchar_open,
	.release = wchar_close,
};

int __init wchar_init(void) {
	int i;

	printk(KERN_INFO "intitting");

	if (register_chrdev_region(MKDEV(WCHAR_MAJOR, 0), WCHAR_MINOR_COUNT, "wchar")) {
		pr_err("Unable to register major " STR(WCHAR_MAJOR));
		return -ENODEV;
	}

	for (i = 0; i < WCHAR_MINOR_COUNT; i++) {
		cdev_init(&wchar_devs[i], &wchar_fops);
		if (cdev_add(&wchar_devs[i], MKDEV(WCHAR_MAJOR,i), 1)) {
			pr_err("Unable to add cdev");
			return -EINVAL;
		}
	}

	return 0;
}

void wchar_exit(void) {
	int i;

	for (i = 0; i < WCHAR_MINOR_COUNT; i++) {
		cdev_del(&wchar_devs[i]);
	}

	unregister_chrdev_region(MKDEV(WCHAR_MAJOR,0), WCHAR_MINOR_COUNT);

	printk(KERN_INFO "exiting");
}

int wchar_open(struct inode * inode, struct file * file) {
	pr_info("wchar open");

	return 0;
}

int wchar_close(struct inode * inode, struct file * file) {
	pr_info("wchar close");

	return 0;
}

static char string[] = "wchar\n";

ssize_t wchar_read(struct file * file, char * __user buf, size_t count, loff_t *offp) {
	pr_info("wchar read");

	if (copy_to_user(buf, string, sizeof(string))) {
		return -ENOMEM;
	}

	return sizeof(string);
}

ssize_t wchar_write(struct file * file, const char * __user buf, size_t count, loff_t *offp) {
	pr_info("wchar write");

	return -EINVAL;
}

module_init(wchar_init);
module_exit(wchar_exit);
