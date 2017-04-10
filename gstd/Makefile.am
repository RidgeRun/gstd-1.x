# Core library if user wants to use as code base for its own application
lib_LTLIBRARIES = libgstd-core.la

libgstd_core_la_SOURCES = gstd_session.c		\
			  gstd_object.c			\
	                  gstd_pipeline.c 		\
			  gstd_element.c		\
			  gstd_list.c			\
			  gstd_ipc.c			\
			  gstd_tcp.c			\
			  gstd_icreator.c		\
			  gstd_iformatter.c		\
			  gstd_pipeline_creator.c	\
			  gstd_no_creator.c		\
			  gstd_json_builder.c		\
			  gstd_ideleter.c		\
			  gstd_pipeline_deleter.c	\
			  gstd_no_deleter.c		\
			  gstd_debug.c			\
			  gstd_event_creator.c		\
			  gstd_event_factory.c		\
			  gstd_pipeline_bus.c		\
			  gstd_ireader.c		\
			  gstd_property_reader.c	\
			  gstd_no_reader.c		\
			  gstd_list_reader.c		\
			  gstd_property.c		\
			  gstd_property_int.c		\
			  gstd_property_string.c	\
			  gstd_property_boolean.c	\
			  gstd_iupdater.c		\
			  gstd_no_updater.c		\
			  gstd_property_enum.c		\
			  gstd_event_handler.c

libgstd_core_la_CFLAGS = $(GST_CFLAGS) $(GIO_CFLAGS) $(GJSON_CFLAGS)
libgstd_core_la_LDFLAGS = $(GST_LIBS) $(GIO_LIBS) $(GJSON_LIBS)

# Standalone application that uses libgstd-core
bin_PROGRAMS = gstd

gstd_SOURCES = gstd.c
gstd_CFLAGS = $(GST_CFLAGS) $(GIO_CFLAGS) $(GJSON_CFLAGS)
gstd_LDFLAGS = $(GST_LIBS) $(GIO_LIBS) $(GJSON_LIBS) -Wl,-rpath -Wl,$(libdir)
gstd_LDADD = libgstd-core.la

gstdincludedir = $(includedir)/gstd
gstdinclude_HEADERS = \
		  gstd_session.h		\
		  gstd_object.h			\
		  gstd_return_codes.h 		\
		  gstd_pipeline.h		\
		  gstd_element.h		\
		  gstd_list.h			\
		  gstd_ipc.h			\
		  gstd_tcp.h			\
		  gstd_icreator.h		\
		  gstd_iformatter.h		\
		  gstd_pipeline_creator.h	\
		  gstd_json_builder.h		\
		  gstd_no_creator.h		\
		  gstd_ideleter.h		\
		  gstd_pipeline_deleter.h	\
		  gstd_no_deleter.h		\
		  gstd_ireader.h		\
		  gstd_property_reader.h	\
		  gstd_no_reader.h		\
		  gstd_list_reader.h		\
		  gstd_property.h		\
		  gstd_property_int.h		\
		  gstd_property_string.h	\
		  gstd_property_boolean.h	\
		  gstd_iupdater.h		\
		  gstd_no_updater.h		\
		  gstd_property_enum.h		\
		  gstd_event_handler.h		\
		  gstd_event_creator.h

noinst_HEADERS = 
