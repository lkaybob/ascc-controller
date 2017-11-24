#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/platform_device.h>

MODULE_DESCRIPTION("System Programming practice 1, key matrix Driver");
MODULE_AUTHOR("Sysprog");
MODULE_LICENSE("GPL");

#define KEY_ADDRESS		(0x06000000 + 0x4000)
#define ADDRESS_MAP_SIZE	0x1000


volatile unsigned short *key_base;

int key_open(struct inode *inode, struct file *pfile){
	
	if(check_mem_region(KEY_ADDRESS, ADDRESS_MAP_SIZE)){
		printk("keydrv : memory already in use\n");
		return -EBUSY;
	}
	if(request_mem_region(KEY_ADDRESS,ADDRESS_MAP_SIZE, "KEY") == NULL){
		printk("keydrv : fail to allocate mem region\n");
		return -EBUSY;
	}

	key_base = ioremap(KEY_ADDRESS, ADDRESS_MAP_SIZE);
	if(key_base == NULL){
		printk("keydrv : fail to Io mapping\n");
		release_mem_region(KEY_ADDRESS, ADDRESS_MAP_SIZE);
		return -ENOMEM;
	}
	
	return 0;
}

ssize_t key_read(struct file *pfile, char *buf, size_t count, loff_t *filePos){
	
	unsigned short rdata;
	unsigned int ret;
	
	writew(0x10, key_base);
	rdata = readw(key_base);
	if(rdata > 0x10) {
		ret = copy_to_user(buf, &rdata, 2);
		return 2;	
	}
	
	writew(0x20, key_base);
	rdata = readw(key_base);
	if(rdata > 0x20) {
		ret = copy_to_user(buf, &rdata, 2);
		return 2;	
	}
	writew(0x40, key_base);
	rdata = readw(key_base);
	if(rdata > 0x40) {
		ret = copy_to_user(buf, &rdata, 2);
		return 2;	
	}
	writew(0x80, key_base);
	rdata = readw(key_base);
	if(rdata > 0x80) {
		ret = copy_to_user(buf, &rdata, 2);
		return 2;	
	}

	writew(0x00, key_base);
	rdata = readw(key_base);
	ret = copy_to_user(buf, &rdata, 2);

	return 2;

}

int key_release(struct inode *inode, struct file *pfile){

	if(key_base != NULL){
		iounmap(key_base);
		key_base = NULL;
		release_mem_region(KEY_ADDRESS, ADDRESS_MAP_SIZE);
	}
	return 0;
}

struct file_operations key_fops = {
	.owner = THIS_MODULE,
	.open = key_open,
	.read = key_read,
	.release = key_release
};

struct miscdevice key_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "kmsw",
	.fops = &key_fops
};

static int __init key_init(void)
{
	int res;

	res = misc_register(&key_device);
	if(res){
		printk("fail to register the device\n");
		return res;
	}
	
	return 0;
}

static void __exit key_exit(void)
{
	misc_deregister(&key_device);
}

module_init(key_init);
module_exit(key_exit);
