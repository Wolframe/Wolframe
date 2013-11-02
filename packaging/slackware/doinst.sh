#!/bin/sh -e

# Author: Andreas Baumann <abaumann@yahoo.com>

# create wolframe user and group if they don't exist
getent group wolframe >/dev/null || /usr/sbin/groupadd wolframe
getent passwd wolframe >/dev/null || /usr/sbin/useradd -g wolframe -c "Wolframe user" -d /dev/null -M -s /bin/false wolframe

# create lock directory
if test ! -d /var/run/wolframe; then
	mkdir /var/run/wolframe
	chown wolframe:wolframe /var/run/wolframe
	chmod 0755 /var/run/wolframe
fi

# create a log directory
if test ! -d /var/log/wolframe; then
	mkdir /var/log/wolframe
	chown wolframe:wolframe /var/log/wolframe
	chmod 0755 /var/log/wolframe
fi

# echo a message to the installer
echo "Add '/etc/rc.d/rc.wolframed' to ' /etc/rc.d/rc.local' to enable the"
echo "Wolframe server at startup"
echo
