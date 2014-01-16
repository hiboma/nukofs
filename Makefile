obj-m := nukofs.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

insmod:
	insmod nukofs.ko

rmmod:
	rmmod nukofs.ko

reload:
	-sudo umount /mnt/nukofs 2>/dev/null
	-sudo make rmmod 2>/dev/null || 1
	sudo make insmod && sudo mount -t nukofs nukofs /mnt/nukofs
