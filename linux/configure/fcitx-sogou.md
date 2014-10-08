
## install sogou pinyin for ubuntu 12

step 1, add ppa and refresh source:
	sudo add-apt-repository ppa:fcitx-team/nightly
	sudo apt-get update

step 2, remove ibus:
	sudo apt-get remove ibus

step 3, install fcitx:
	sudo apt-get install fcitx

step 4, install dependency:
	##check dependency 
	sudo dpkg -i sogou_pinyinxxx.deb
	##install dependency 
	sudo apt-get -f install

step 5, install .deb file:
	sudo dpkg -i sogou_pinyinxxx.deb

setp 6, reboot

notice:
1. more than one time for login x-windows CAUSE an error.

2. or else dependencies:
	sudo apt-get install fcitx fcitx-bin fcitx-config-common \
	  fcitx-config-gtk fcitx-config-gtk2 fcitx-config-gtk3 \
	  fcitx-config-qt4 fcitx-libs fcitx-module-dbus fcitx-module-x11 \
	  fcitx-modules fcitx-table fcitx-ui-classic

3. generally, fcitx include:
	:-bin
	:-config
	:-frontend
	:-modules
	:-table
	:-ui
