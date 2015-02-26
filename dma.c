#include<linux/init.h>
#include<linux/module.h>
#include<linux/debugfs.h>
#include<linux/slab.h>

MODULE_LICENSE("GPL");

#define SOURCE_FILE "source"
#define SOURCE_FILE_MODE (0666)
#define DIR_NAME "dma-copy"
#define DEST_FILE "destination"
#define DEST_FILE_MODE (0444)
#define ACTION_FILE "action"
#define ACTION_MODE (0222)

static struct dentry *root;
static char *source, *dest;

static void copy_using_dma(char *dst, char *src)
{
	pr_warn("Copy using dma complete!\n");
}

static ssize_t source_read(struct file *file,
char __user *buff, size_t length, loff_t *offset)
{
	return simple_read_from_buffer(buff, length, offset, source, PAGE_SIZE);
}

static ssize_t source_write(struct file *file,
const char __user *buff, size_t length, loff_t *offset)
{
	return simple_write_to_buffer(source, PAGE_SIZE, offset, buff, length);
}

static ssize_t destination_read(struct file *file,
char __user *buff, size_t length, loff_t *offset)
{
	/* wait for dma completion here
	the buffer might not be ready */
	return simple_read_from_buffer(buff, length, offset, dest, PAGE_SIZE);
}

static ssize_t action_write(struct file *file,
const char __user *buff, size_t length, loff_t *offset)
{
	copy_using_dma(dest, source);
	dma_alloc_coherent
	return length;
}

static const struct file_operations source_ops = {
	.owner = THIS_MODULE,
	.read = source_read,
	.write = source_write
};

static const struct file_operations destination_ops = {
	.owner = THIS_MODULE,
	.read = destination_read,
};

static const struct file_operations action_ops = {
	.owner = THIS_MODULE,
	.write = action_write
};

static __init int debugfs_init(void)
{
	root = debugfs_create_dir(DIR_NAME, NULL);
	if (!root)
		return -ENODEV;
	if (!debugfs_create_file(SOURCE_FILE, SOURCE_FILE_MODE, root, NULL, &source_ops))
		goto cleanup;
	if (!debugfs_create_file(DEST_FILE, DEST_FILE_MODE,
					root, NULL, &destination_ops))
		goto cleanup;
	if (!debugfs_create_file(ACTION_FILE, ACTION_MODE, root, NULL, &action_ops))
		goto cleanup;
	source = kzalloc(PAGE_SIZE, GFP_DMA);
	if (!source)
		goto cleanup;
	dest = kzalloc(PAGE_SIZE, GFP_DMA);
	if (!dest) {
		kfree(source);
		goto cleanup;
	}

	return 0;

cleanup:
	debugfs_remove_recursive(root);
	return -ENODEV;
}

static __exit void debugfs_exit(void)
{
	debugfs_remove_recursive(root);
	kfree(source);
	kfree(dest);
}

module_init(debugfs_init);
module_exit(debugfs_exit);


