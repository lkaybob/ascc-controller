obj-m := buzzerDrv.o
KDIR := /CNDI_CD/source/BSP/kernel/kernel
PWD := $(shell pwd)

export CROSS_COMPILE=arm-none-linux-gnueabi-
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
clean:
	rm *.o *.ko *.moc.c .*.cmd modules.order Module.symvers
