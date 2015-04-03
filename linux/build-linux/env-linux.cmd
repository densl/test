

#manual logon in ubuntu12.04 

greeter-show-manual-login=true  ## manual login
allow-guest=false    ## not allow guest

#when move linux system, take care of:
  /et/fstab    ## define static disk infomation
/dev/sdb1	/media/test	defaults	0	0
#when tar linux system
  --exclude=/dev --exclude=/sys --exclude=/mnt --exclude=/proc
