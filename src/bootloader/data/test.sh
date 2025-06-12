#!/bin/bash
set -e

IMG="$1"
IMG_SIZE_MB=256
VOLUME_LABEL=KERNELVOL
LOOP_DEV=

cleanup() {
    if mountpoint -q tmp; then
        sudo umount tmp
    fi
    if [ -n "$LOOP_DEV" ]; then
        sudo losetup -d "$LOOP_DEV"
    fi
	rmdir tmp
}
trap cleanup EXIT

dd if=/dev/zero of=$IMG bs=1M count=$IMG_SIZE_MB

parted -s $IMG mklabel gpt
parted -s $IMG mkpart ext2-part ext2 1MiB 100%
parted -s $IMG name 1 $VOLUME_LABEL

LOOP_DEV=$(sudo losetup --find --partscan --show $IMG)
PART_DEV=${LOOP_DEV}p1

sleep 1

sudo mkfs.ext2 -L "$VOLUME_LABEL" $PART_DEV

mkdir tmp/
sudo mount $PART_DEV tmp/

sudo mkdir tmp/kernel_src/
sudo cp kboot tmp/kernel_src/kernel.elf

sudo mkdir -p tmp/boot/conf
sudo ln -s /kernel_src/kernel.elf tmp/boot/conf/kernel

sudo blkid $PART_DEV

sync
