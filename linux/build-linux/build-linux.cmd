
##

First, build bootloader (uboot or any others).

Second:
	make config for kernel
	make kernel

Last:
	make root file system.


##
Android same as linux. 


#set manual login
 add at last: /etc/lightdm/lightdm.conf
	greeter-show-manual-login=true
	allow-guest=false
	
##build lib and&or envs
ubuntu12 x86:
	sudo apt-get install git-core gnupg flex bison gperf \
	build-essential zip curl libc6-dev libncurses5-dev:i386 \
	x11proto-core-dev libx11-dev:i386 libreadline6-dev:i386 \
	libgl1-mesa-dev:i386 g++-multilib mingw32 openjdk-6-jkd \
	tofrodos python-markdown libxml2-utils xsltproc zlib1g-dev:i386

ubuntu12 x64:
	apt-get install build-essential bison flex gnupg gperf curl \
	python-markdown git-core tofrodos libxml2-utils xsltproc \
	zlib1g-dev x11proto-core-dev libx11-dev libreadline6-dev \
	libncurses5-dev libgl1-mesa-dev


		
