# swap partition
### create
    mkswap /dev/sdb2

### start
    swapon /dev/sdb2

### write to fstab
    /dev/sdb2 swap swap defaults 0 0


** swap file **
## create 512M swap file
    dd if=/dev/zero of=/swapfile bs=1024k count=512

## setting & start
    mkswap /swapfile1
    swapon /swapfile1
    /swapfile1 swap swap defaults 0 0

## close
    swapoff /dev/sdb2

** mem cmd **
    free -m 
    top
    vmstat
    cat /proc/meminfo
    dmesg | grep [mM][eE][mM]
    cat /proc/pid/status

  
