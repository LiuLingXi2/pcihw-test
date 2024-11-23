obj-m += pci_config_rd.o \
			pci_test.o \
			gic_its_test.o

PWD = $(shell pwd)

CFLAGS := -Werror -Wall -Wno-unused-variable -Wno-pointer-to-int-cast

mdl:
	make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -C /home/ubuntu/liuhao/linux-5.14.5 M=$(PWD) CFLAGS="-Wno-unused-variable" modules

.PHONY: clean
clean:
	make -C /home/ubuntu/liuhao/linux-5.14.5 M=$(PWD) clean
