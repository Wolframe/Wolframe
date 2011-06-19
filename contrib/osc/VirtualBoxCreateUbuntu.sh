#!/bin/sh

# VBoxManage extpack install Oracle_VM_VirtualBox_Extension_Pack-4.0.8-71778.vbox-extpack 

VM="Ubuntu 11.04 Desktop i386"

VBoxManage unregistervm "$VM" --delete
rm -f "$VM.vdi"

VBoxManage createvm --name "$VM" --register
VBoxManage modifyvm "$VM" --memory 1024 --acpi on --boot1 dvd \
	--nic1 bridged --bridgeadapter1 eth0
VBoxManage createhd --filename "$VM.vdi" --format vdi \
	--variant Standard --size 8192
VBoxManage storagectl "$VM" --name "SATA Controller" --add sata \
	--bootable on
VBoxManage storageattach "$VM" --storagectl "SATA Controller" \
	--port 0 --device 0 --type hdd --medium "$VM.vdi"
VBoxManage storageattach "$VM" --storagectl "SATA Controller" \
	--port 1 --device 0 --type dvddrive --medium ubuntu-11.04-desktop-i386.iso

# install now

# reset boot device
#VBoxManage modifyvm "Ubuntu 11.04 Desktop i386" --boot1 disk

# VBoxHeadless -s "Ubuntu 11.04 Desktop i386" -von &
# VBoxManage controlvm "Ubuntu 11.04 Desktop i386" acpipowerbutton
# rdesktop -z -m localhost &
VBoxManage modifyvm "Ubuntu 11.04 Desktop i386" --boot1 disk
 