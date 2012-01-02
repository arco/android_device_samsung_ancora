#!/sbin/sh
dd if=/dev/zero of=/dev/block/mmcblk0p14 bs=1 count=1 > /dev/null 2>&1 ; sync
