#include<stdio.h>
#include<stdlib.h>
#include<getopt.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/ioctl.h>
#include<fcntl.h>

#define GFP_DMA 1
#define GFP_DMA32 4
#define GFP_NORMAL 0
#define GFP_HIGHMEM 2

#define MAX_ORDER 10

#define MEM_ALLOC 0
#define MEM_FREE 1
#define MEM_INFO 2

struct mem_desc {
	int zone;
	int order;
	int pages;
};

static struct option options[] = {
	{"help", no_argument, 0, 0},
	{"info", no_argument, 0, 1},
	{"zone", required_argument, 0, 2},
	{"order", required_argument, 0, 3},
	{"pages", required_argument, 0, 4},
	{"free", no_argument, 0, 5},
	{0, 0, 0, 0}
};

void display_help()
{
	printf("This is help\n");
}

int call_dev_ioctl(int cmd, struct mem_desc *arg)
{
	int fd = open("/dev/mp", O_RDWR);
	int ret;

	if (fd < 0) {
		perror("Could not open /dev/mp");
		return;
	}

	ret = ioctl(fd, cmd, arg);

	close(fd);
	
	return ret;
}

void display_info()
{
	printf("Here = %d\n", MEM_INFO);
	call_dev_ioctl(MEM_INFO, NULL);
}

void free_memory()
{
	printf("freeing pages...\n");
	call_dev_ioctl(MEM_FREE, NULL);
}

void allocate_memory(struct mem_desc *mem)
{
	int ret = call_dev_ioctl(MEM_ALLOC, mem);

	if (ret)
		printf("Failed to allocate memory: %s\n", strerror(ret));
	else
		printf("Allocated requested chunk of memory...\n");
}

int main(int argc, char *argv[])
{
	int ret;
	int index;

	static struct mem_desc mem;

	mem.zone = GFP_DMA;
	mem.pages = 1;
	mem.order = 0;

	while (1) {
		ret = getopt_long(argc, argv, "", options, &index);

		if (ret == -1)
			break;

		switch(ret) {
			case 0:
				display_help();
				return 0;
			case 1:
				display_info();
				return 0;
			case 2:
				if (strcasecmp(optarg, "dma") == 0)
					mem.zone = GFP_DMA;
				else if (strcasecmp(optarg, "normal") == 0)
					mem.zone = GFP_NORMAL;
				else if (strcasecmp(optarg, "dma32") == 0)
					mem.zone = GFP_DMA32;
				else if (strcasecmp(optarg, "highmem") == 0)
					mem.zone = GFP_HIGHMEM;
				continue;
			case 3:
				mem.order = atoi(optarg);
				if (mem.order < 0 || mem.order >= MAX_ORDER) {
					printf("Invalid argument - '%s'\n", optarg);
					return 0;
				}
				continue;
			case 4:
				mem.pages = atoi(optarg);
				if (mem.pages <= 0) {
					printf("Invalid argument - '%s'\n", optarg);
					return 0;
				}
				continue;
			case 5:
				free_memory();
				return 0;
			default:
				break;
		}

	}

	allocate_memory(&mem);

	return 0;
}
