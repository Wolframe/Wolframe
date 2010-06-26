TOPDIR = ../../..

SUBDIRS =

INCLUDE_CPPFLAGS = \
	-DBOOST_LOG_USE_NATIVE_SYSLOG

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)

LOG_OBJS = \
	attribute_set.o \
	attribute_values_view.o \
	code_conversion.o \
	core.o \
	exceptions.o \
	global_logger_storage.o \
	named_scope.o \
	process_id.o \
	severity_level.o \
	sink_frontends.o \
	syslog_backend.o \
	text_file_backend.o \
	text_ostream_backend.o \
	thread_specific.o \
	timer.o \
	trivial.o \
	slim_string.o

LOG_SETUP_OBJS = \
	parser_utils.o \
	init_from_stream.o \
	init_from_settings.o \
	filter_parser.o \
	formatter_parser.o

OBJS = \
	$(LOG_OBJS) \
	$(LOG_SETUP_OBJS)

local_all: libboost_log.a libboost_log_setup.a

libboost_log.a: $(LOG_OBJS)
	$(AR) cr $@ $?

libboost_log_setup.a: $(LOG_SETUP_OBJS)
	$(AR) cr $@ $?

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:
	-rm -rf libboost_log.a libboost_log_setup.a

local_distclean:

local_test:
