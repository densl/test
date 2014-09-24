
## show cpu msg
	lscpu

## show memory msg
	dmidecode -t memory

## show pci msg
	lspci

## show usb msg
	lsusb

## show DMI(Desktop Management Interface) or SMBIOS msg
	dmidecode

## show hdd and part
	lsblk
     or	fdisk -l
     or df -h
 
## show net controller
	lspci | grep -i 'eth'
     or	ifconfig
     or ip link show
     or ethtool eth0

## show category
     du -h
 

