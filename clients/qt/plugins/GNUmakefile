TOPDIR = ../../..

SUBDIRS =

.NOTPARALLEL:

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS = \
	-DQT_PLUGIN -DQT_SHARED -DQT_NO_DEBUG \
	-DQDESIGNER_EXPORT_WIDGETS

INCLUDE_DIRS = \
	-I. \
	-I.. \
	-Ifilechooser \
	-Ipicturechooser \
	-Iformwidget \
	-I$(QT_INCLUDE_DIR) \
	-I$(QT_INCLUDE_DIR)/QtCore \
	-I$(QT_INCLUDE_DIR)/QtGui \
	-I$(QT_INCLUDE_DIR)/QtDesigner \
	-I$(QT_INCLUDE_DIR)/QtUiTools

INCLUDE_LDFLAGS = \
	-L$(QT_DIR)/lib \
	-L$(QT_LIB_DIR) \
	$(QT_LDFLAGS)

INCLUDE_LIBS = \
	-lQtCore -lQtGui -lQtDesigner

MOC_FLAGS = \
	$(INCLUDE_DIRS)

CPP_OBJS = \
	WolframeWidgets.o \
	moc_WolframeWidgets.o \
	../FileChooser.o \
	../moc_FileChooser.o \
	filechooser/FileChooserPlugin.o \
	filechooser/moc_FileChooserPlugin.o \
	../PictureChooser.o \
	../moc_PictureChooser.o \
	picturechooser/PictureChooserPlugin.o \
	picturechooser/moc_PictureChooserPlugin.o

DYNAMIC_MODULE = libwolframewidgets.so

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

moc_WolframeWidgets.cpp: WolframeWidgets.hpp
	$(QT_MOC) $(MOC_FLAGS) WolframeWidgets.hpp -o moc_WolframeWidgets.cpp

../moc_FileChooser.cpp: ../FileChooser.hpp
	$(QT_MOC) $(MOC_FLAGS) ../FileChooser.hpp -o ../moc_FileChooser.cpp

filechooser/moc_FileChooserPlugin.cpp: filechooser/FileChooserPlugin.hpp
	$(QT_MOC) $(MOC_FLAGS) filechooser/FileChooserPlugin.hpp -o filechooser/moc_FileChooserPlugin.cpp

../moc_PictureChooser.cpp: ../PictureChooser.hpp
	$(QT_MOC) $(MOC_FLAGS) ../PictureChooser.hpp -o ../moc_PictureChooser.cpp

picturechooser/moc_PictureChooserPlugin.cpp: picturechooser/PictureChooserPlugin.hpp
	$(QT_MOC) $(MOC_FLAGS) picturechooser/PictureChooserPlugin.hpp -o picturechooser/moc_PictureChooserPlugin.cpp

local_clean:
	@-rm -f moc_*.cpp filechooser/moc_*.cpp picturechooser/moc_*.cpp formwidget/moc_*.cpp
	@-rm -f Makefile

local_distclean:

local_install:
	$(INSTALL) -d -m 0755 $(DESTDIR)$(libdir)/wolframe
	$(INSTALL) -m 0775 $(DYNAMIC_MODULE) $(DESTDIR)$(libdir)/wolframe

local_uninstall:
	-rm $(DESTDIR)$(libdir)/wolframe/modules/$(DYNAMIC_MODULE)
	-rmdir $(DESTDIR)$(libdir)/wolframe/modules

local_test:
