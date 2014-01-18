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
	-umount /mnt/nukofs 2>/dev/null
	-make rmmod 2>/dev/null
	make insmod && mount -t nukofs nukofs /mnt/nukofs

test:
	prove