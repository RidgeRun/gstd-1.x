TESTS = 			\
	libgstc_client		\
	libgstc_ping		\
	libgstc_pipeline_create	\
	libgstc_pipeline_delete	\
	libgstc_pipeline_play   \
	libgstc_pipeline_pause	\
	libgstc_pipeline_stop

check_PROGRAMS = $(TESTS)

AM_CFLAGS = $(GST_CFLAGS) -I$(top_srcdir)/libgstc
AM_LDFLAGS = $(GST_LIBS)

COMMON_SOURCES = $(top_srcdir)/libgstc/libgstc_assert.c

libgstc_client_SOURCES =		\
	test_libgstc_client.c		\
	$(top_srcdir)/libgstc/libgstc.c	\
	$(COMMON_SOURCES)

libgstc_ping_SOURCES =			\
	test_libgstc_ping.c		\
	$(top_srcdir)/libgstc/libgstc.c	\
	$(COMMON_SOURCES)

libgstc_pipeline_create_SOURCES = 		\
	test_libgstc_pipeline_create.c		\
	$(top_srcdir)/libgstc/libgstc.c		\
	$(COMMON_SOURCES)

libgstc_pipeline_delete_SOURCES = 		\
	test_libgstc_pipeline_delete.c		\
	$(top_srcdir)/libgstc/libgstc.c		\
	$(COMMON_SOURCES)

libgstc_pipeline_play_SOURCES = 		\
	test_libgstc_pipeline_play.c		\
	$(top_srcdir)/libgstc/libgstc.c		\
	$(COMMON_SOURCES)

libgstc_pipeline_pause_SOURCES = 		\
	test_libgstc_pipeline_pause.c		\
	$(top_srcdir)/libgstc/libgstc.c		\
	$(COMMON_SOURCES)

libgstc_pipeline_stop_SOURCES = 		\
	test_libgstc_pipeline_stop.c		\
	$(top_srcdir)/libgstc/libgstc.c		\
	$(COMMON_SOURCES)
