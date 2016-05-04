#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/miscdevice.h>
#include<linux/uaccess.h>
#include<linux/string.h>
#include<linux/list.h>
#include<linux/slab.h>

MODULE_LICENSE("GPL");

static void free_page_list(void);

LIST_HEAD(page_list_head);
struct used_page {
        void *page;
	int page_size;
        struct list_head page_list;
};

void add_to_used_list(void *page, int size)
{
        struct used_page *used_page;

        used_page = kzalloc(sizeof(*used_page), GFP_KERNEL);
        if (!used_page)
                return;

        used_page->page = page;
	used_page->page_size = size;

        list_add_tail(&used_page->page_list, &page_list_head);
}

static void free_page_list(void)
{
        struct list_head *pos, *tmp;
        struct used_page *used_page;

        list_for_each_safe(pos, tmp, &page_list_head) {
                used_page = list_entry(pos, struct used_page, page_list);
                list_del(&used_page->page_list);
                pr_alert("Page ptr = %p\n", used_page->page);
                kfree(used_page->page);
		kfree(used_page);
	}
}

static void display_allocation(void)
{
	struct used_page *page;

	list_for_each_entry(page, &page_list_head, page_list)
	{
		pr_alert("Page addr = %p [%d]\n", page->page, page->page_size);
	}
}

static ssize_t misc_read(struct file *file,
char __user *buff, size_t length, loff_t *offset)
{
	return 0;
}

static ssize_t misc_write(struct file *file,
const char __user *buff, size_t length, loff_t *offset)
{

	return length;
}

gfp_t get_flag(int flag)
{
	switch(flag)
	{
		case 1: 
			return GFP_DMA;
		case 3:
			return GFP_DMA32;
		default:
			return GFP_KERNEL;
	}
}

static long mpdev_ioctl(struct file *file, unsigned int cmd,
				unsigned long args)
{
	int user_data[3];
	void *ptr;
	int size, flag;

	pr_alert("CMD = %d\n", cmd);
	if (cmd == 0) {
		if (copy_from_user(user_data, (__user int *) args, sizeof(user_data)))
			return -EINVAL;

		size = (1 << user_data[1]) * PAGE_SIZE;
		flag = get_flag(user_data[0]);

		while (user_data[2]--) {
			pr_alert("Allocating page of size %d\n", size);
			ptr = kmalloc(size, GFP_DMA);
			if (!ptr)
				return -ENOMEM;
			add_to_used_list(ptr, size);
		}

	} else if (cmd == 1) {
		free_page_list();
	} else if (cmd == 2) {
		display_allocation();
	}

	return 0;
}
static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = misc_read,
	.write = misc_write,
	.unlocked_ioctl = mpdev_ioctl
};

static struct miscdevice misc_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "mp",
	.fops = &fops,
	.mode = 0777
};

static __init int miscdrv_init(void)
{
	misc_register(&misc_device);
	return 0;
}

static __exit void miscdrv_exit(void)
{
	misc_deregister(&misc_device);
}

module_init(miscdrv_init);
module_exit(miscdrv_exit);
