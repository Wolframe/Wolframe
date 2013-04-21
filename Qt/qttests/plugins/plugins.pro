TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS = filechooser picturechooser

picturechooser.depends = filechooser
