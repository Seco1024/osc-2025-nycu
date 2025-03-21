sudo mount /dev/sdb1 /mnt/usb
sudo cp bootloader.img /mnt/usb/bootloader.img
cd ./rootfs
find . | cpio -o -H newc > initramfs.cpio
cd ..
sudo cp initramfs.cpio /mnt/usb/initramfs.cpio
sudo cp config.txt /mnt/usb/config.txt
sync
sudo umount /mnt/usb