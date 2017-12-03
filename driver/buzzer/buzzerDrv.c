#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/delay.h> 
#include <linux/platform_device.h>
//#include <linux/type.h> 

#include "peripheral.h"

MODULE_DESCRIPTION("Buzzer Driver");
MODULE_AUTHOR("Sysprog");
MODULE_LICENSE("GPL");

#define	BUZZER_BASE_ADDRESS	(0x06000000 + 0xB000)
#define ADDRESS_MAP_SIZE 0x1000

volatile unsigned short *buzzer_base;

int buzzer_open(struct inode *inode, struct file *pfile) {
	printk("[buzzer_open] - success:\n");

	if(request_mem_region(BUZZER_BASE_ADDRESS, ADDRESS_MAP_SIZE, "CNBUZZER") == NULL)
		return -EBUSY;

	buzzer_base = ioremap(BUZZER_BASE_ADDRESS, ADDRESS_MAP_SIZE);

	if(buzzer_base == NULL) {
		release_mem_region(BUZZER_BASE_ADDRESS, ADDRESS_MAP_SIZE);
		return -ENOMEM;
	}

	return 0;
}

ssize_t buzzer_read(struct file *pfile, char *buf, size_t count, loff_t *filepos) {
	unsigned short rdata;
	unsigned int ret;

	rdata = readw(buzzer_base);
	rdata &= 0xff;
	ret = copy_to_user(buf, &rdata, 2);

	return 2;
}

ssize_t buzzer_write(struct file *pfile, const char *buf, size_t count, loff_t *filePos) {
	unsigned short wdata;
	unsigned int ret;

	ret = copy_from_user(&wdata, buf, 2);
	writew(wdata, buzzer_base);
	return count;
}

int buzzer_release(struct inode *inode, struct file *pfile) {
	if(buzzer_base != NULL) {
		iounmap(buzzer_base);
		buzzer_base = NULL;
		release_mem_region(BUZZER_BASE_ADDRESS, ADDRESS_MAP_SIZE);
	}

	return 0;
}

struct file_operations buzzer_fops = {
	.owner = THIS_MODULE,
	.open = buzzer_open,
	.write = buzzer_write,
	.read = buzzer_read,
	.release = buzzer_release
};

struct miscdevice buzzer_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEV_NAME_BUZZER,
	.fops = &buzzer_fops
};

static int __init buzzer_init(void) {
	int error;

	error = misc_register(&buzzer_device);
	if(error) {
		pr_err("Can't misc_register :(\n");
		return error;
	}

	return 0;
}

static void __exit buzzer_exit(void) {
	misc_deregister(&buzzer_device);
}

module_init(buzzer_init);
module_exit(buzzer_exit);
