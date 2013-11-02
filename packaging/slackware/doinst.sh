#!/bin/sh -e

# Author: Andreas Baumann <abaumann@yahoo.com>

ARCH=`uname -m`
if test "x$ARCH" = "xx86_64"; then
	LIBDIR="usr/lib64"
else
if test "x$ARCH" = "xi686"; then
	LIBDIR="usr/lib"
else
	echo "ERROR: Unknown slackware architecture '$ARCH'"
	exit 1
fi
fi

( cd $LIBDIR/wolframe ; rm -rf liblua.so.5 )
( cd $LIBDIR/wolframe ; ln -sf liblua.so.5.2.0 liblua.so.5 )
( cd $LIBDIR/wolframe ; rm -rf libwolframe_prnt.so.0 )
( cd $LIBDIR/wolframe ; ln -sf libwolframe_prnt.so.0.0.0 libwolframe_prnt.so.0 )
( cd $LIBDIR/wolframe ; rm -rf libwolframe_serialize.so )
( cd $LIBDIR/wolframe ; ln -sf libwolframe_serialize.so.0.0.0 libwolframe_serialize.so )
( cd $LIBDIR/wolframe ; rm -rf libwolframe_prnt.so )
( cd $LIBDIR/wolframe ; ln -sf libwolframe_prnt.so.0.0.0 libwolframe_prnt.so )
( cd $LIBDIR/wolframe ; rm -rf libwolframe_langbind.so.0 )
( cd $LIBDIR/wolframe ; ln -sf libwolframe_langbind.so.0.0.0 libwolframe_langbind.so.0 )
( cd $LIBDIR/wolframe ; rm -rf libwolframe_client.so )
( cd $LIBDIR/wolframe ; ln -sf libwolframe_client.so.0.0.0 libwolframe_client.so )
( cd $LIBDIR/wolframe ; rm -rf libwolframe_langbind.so )
( cd $LIBDIR/wolframe ; ln -sf libwolframe_langbind.so.0.0.0 libwolframe_langbind.so )
( cd $LIBDIR/wolframe ; rm -rf libwolframe.so.0 )
( cd $LIBDIR/wolframe ; ln -sf libwolframe.so.0.0.0 libwolframe.so.0 )
( cd $LIBDIR/wolframe ; rm -rf libwolframe_serialize.so.0 )
( cd $LIBDIR/wolframe ; ln -sf libwolframe_serialize.so.0.0.0 libwolframe_serialize.so.0 )
( cd $LIBDIR/wolframe ; rm -rf libfreeimageplus.so )
( cd $LIBDIR/wolframe ; ln -sf libfreeimageplus.so.3.15.4 libfreeimageplus.so )
( cd $LIBDIR/wolframe ; rm -rf libwolframe_database.so )
( cd $LIBDIR/wolframe ; ln -sf libwolframe_database.so.0.0.0 libwolframe_database.so )
( cd $LIBDIR/wolframe ; rm -rf libfreeimage.so.3 )
( cd $LIBDIR/wolframe ; ln -sf libfreeimage.so.3.15.4 libfreeimage.so.3 )
( cd $LIBDIR/wolframe ; rm -rf libwolframe_database.so.0 )
( cd $LIBDIR/wolframe ; ln -sf libwolframe_database.so.0.0.0 libwolframe_database.so.0 )
( cd $LIBDIR/wolframe ; rm -rf libfreeimageplus.so.3 )
( cd $LIBDIR/wolframe ; ln -sf libfreeimageplus.so.3.15.4 libfreeimageplus.so.3 )
( cd $LIBDIR/wolframe ; rm -rf liblua.so )
( cd $LIBDIR/wolframe ; ln -sf liblua.so.5.2.0 liblua.so )
( cd $LIBDIR/wolframe ; rm -rf libfreeimage.so )
( cd $LIBDIR/wolframe ; ln -sf libfreeimage.so.3.15.4 libfreeimage.so )
( cd $LIBDIR/wolframe ; rm -rf libhpdf.so )
( cd $LIBDIR/wolframe ; ln -sf libhpdf.so.2.2.1 libhpdf.so )
( cd $LIBDIR/wolframe ; rm -rf libhpdf.so.2 )
( cd $LIBDIR/wolframe ; ln -sf libhpdf.so.2.2.1 libhpdf.so.2 )
( cd $LIBDIR/wolframe ; rm -rf libwolframe.so )
( cd $LIBDIR/wolframe ; ln -sf libwolframe.so.0.0.0 libwolframe.so )
( cd $LIBDIR/wolframe ; rm -rf libwolframe_client.so.0 )
( cd $LIBDIR/wolframe ; ln -sf libwolframe_client.so.0.0.0 libwolframe_client.so.0 )

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
