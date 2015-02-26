obj-m := dma.o
KDIR ?= /usr/src/linux-headers-`uname -r`/
all:
	make -C $(KDIR) M=`pwd` modules

clean:
	rm *.ko *.o *.symvers *.order *.mod.c

