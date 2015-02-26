#include<linux/init.h>
#include<linux/module.h>
#include<linux/slab.h>

MODULE_LICENSE("GPL");

static __init int debugfs_init(void)
{
	return 0;
}

static __exit void debugfs_exit(void)
{
}

module_init(debugfs_init);
module_exit(debugfs_exit);


