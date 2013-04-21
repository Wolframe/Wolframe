TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS = libqtwolframeclient skeleton example plugins qtclient

skeleton.depends = libqtwolframeclient
qtclient.depends = libqtwolframeclient
example.depends = libqtwolframeclient skeleton
