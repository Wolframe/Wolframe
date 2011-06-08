#!/bin/sh

#PATH=$PATH:/usr/sbin:/sbin
#brctl addbr br0
#ifconfig eth0 0.0.0.0
#brctl addif br0 eth0
#ifconfig br0 192.168.0.198 netmask 255.255.255.0 up
#route add -net 192.168.1.0 netmask 255.255.255.0 br0
#route add default gw 192.168.0.1 br0
#tunctl -b -u root
#ifconfig tap0 up
#brctl addif br0 tap0
export SDL_VIDEO_X11_DGAMOUSE=0
export DISPLAY=andreas-pc:0.0
#iptables -I RH-Firewall-1-INPUT -i br0 -j ACCEPT
#/usr/libexec/qemu-kvm -m 1024 -cpu qemu64 -hda Fedora15.img \
#	-net nic -net tap,ifname=tap0,script=0

/usr/libexec/qemu-kvm -m 1024 -cpu qemu64 -hda /data/sas/machines/Fedora15/Fedora15.img -vnc 0.0.0.0:0 &
/usr/libexec/qemu-kvm -m 1024 -cpu qemu64 -hda /data/sas/machines/Ubuntu1010/Ubuntu1010.img -vnc 0.0.0.0:1 \
	-hdb /data/sas/machines/Ubuntu1010/Ubuntu1010-data.img \
	-cdrom /data/sas/isos/ubuntu-10.10-server-amd64.iso &
/usr/libexec/qemu-kvm -m 1024 -cpu qemu64 -hda /data/sas/machines/OpenSuse114/openSUSE114.img -vnc 0.0.0.0:2 &
/usr/libexec/qemu-kvm -m 1024 -cpu qemu64 -hda /data/sas/machines/ScientificLinux6/ScientificLinux6.img \
	-hdb /data/sas/machines/ScientificLinux6/ScientificLinux6-data.img -vnc 0.0.0.0:3 &
