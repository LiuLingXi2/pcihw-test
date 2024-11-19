obj-m += self.o

CFLAGS := -Werror -Wall -Wno-unused-variable

all: module initramfs boot

module:
	make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -C /home/ubuntu/liuhao/linux-5.14.5 M=$(PWD) modules

boot:
	qemu-system-aarch64 \
		-machine virt,gic-version=3 \
		-cpu cortex-a72 -smp 4 -m 1G \
		-device qemu-xhci,id=xhci \
		-device pci-bridge,chassis_nr=1,id=pci.1 \
		-device nvme,drive=nvme0n1,bus=pci.1,addr=0x4,serial="nvme-serial0" \
		-drive file=nvme0n1.img,if=none,id=nvme0n1,format=raw \
		-device nvme,drive=nvme1n1,bus=pci.1,addr=0x5,serial="nvme-serial1" \
		-drive file=nvme1n1.img,if=none,id=nvme1n1,format=raw \
		-kernel Image \
		-append "root=/dev/ram0 rdinit=/linuxrc console=ttyAMA0" \
		-initrd rootfs.img \
		-nographic

.PHONY: initramfs
initramfs:
	cd ./initramfs && find . -print0 | cpio -ov --null --format=newc | gzip -9 > ../rootfs.img
	@echo "create initramfs success!"

nvme:
	# qemu-img create -f raw nvme0n1.img 10G
	sudo losetup --find --show nvme0n1.img
	@echo "Yes" | sudo parted /dev/loop0 mklabel gpt
	sudo parted /dev/loop0 mkpart primary ext4 0% 100%
	sudo parted /dev/loop0 print
	@echo "y" | sudo mkfs.ext4 /dev/loop0p1
	sudo losetup -d /dev/loop0

	sudo losetup --find --show nvme1n1.img
	@echo "Yes" | sudo parted /dev/loop0 mklabel gpt
	sudo parted /dev/loop0 mkpart primary ext4 0% 100%
	sudo parted /dev/loop0 print
	@echo "y" | sudo mkfs.ext4 /dev/loop0p1
	sudo losetup -d /dev/loop0
	@echo "create nvme success!"

.PHONY: clean
clean:
	make -C /home/ubuntu/liuhao/linux-5.14.5 M=$(PWD) clean