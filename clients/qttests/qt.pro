TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS = libqtwolframeclient skeleton example wolframewidgets qtclient

skeleton.depends = libqtwolframeclient
qtclient.depends = libqtwolframeclient
example.depends = libqtwolframeclient skeleton
