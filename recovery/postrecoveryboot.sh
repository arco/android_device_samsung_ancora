#!/sbin/sh

# 0: charge
# 1: direct
# 2: recovery_enter
# 3: recovery_end
# 4: download_end

sleep 1
echo 3 > /dev/block/mmcblk0p14
