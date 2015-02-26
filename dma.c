#include<linux/init.h>
#include<linux/module.h>
#include<linux/slab.h>

MODULE_LICENSE("GPL");

static int *logical_address;
static unsigned long physical_address;

static __init int debugfs_init(void)
{

	logical_address = kmalloc(4096, GFP_KERNEL);

	pr_warn("Logical address allocated = %p\n", logical_address);
	pr_warn("PAGE_OFFSET = %p\n", PAGE_OFFSET);
	pr_warn("Physical address could be = %p\n", 
			(unsigned long)logical_address - PAGE_OFFSET);
	pr_warn("Physical address using macros = %p\n", 
			physical_address = __pa(logical_address));
	pr_warn("Logical address using macros again = %p\n",
			__va(physical_address));
	return 0;
}

static __exit void debugfs_exit(void)
{
	kfree(logical_address);
}

module_init(debugfs_init);
module_exit(debugfs_exit);
