TESTS = 				\
	libgstc_client			\
	libgstc_ping			\
	libgstc_pipeline_create		\
	libgstc_pipeline_delete		\
	libgstc_pipeline_play   	\
	libgstc_pipeline_pause		\
	libgstc_pipeline_stop		\
	libgstc_json			\
	libgstc_socket			\
	libgstc_element_set		\
	libgstc_pipeline_inject_eos 	\
	libgstc_pipeline_bus_wait_async \
	libgstc_pipeline_bus_wait 	\
	libgstc_pipeline_flush_start 	\
	libgstc_pipeline_flush_stop

check_PROGRAMS = $(TESTS)

AM_CFLAGS = $(GST_CFLAGS) $(JANSSON_CFLAGS) $(GIO_CFLAGS) -I$(top_srcdir)/libgstc
AM_LDFLAGS = $(GST_LIBS) $(JANSSON_LIBS) $(GIO_LIBS)

COMMON_SOURCES = \
	@top_srcdir@/libgstc/libgstc_assert.c \
	@top_srcdir@/libgstc/libgstc_thread.c


libgstc_client_SOURCES =		\
	test_libgstc_client.c		\
	@top_srcdir@/libgstc/libgstc.c	\
	$(COMMON_SOURCES)
libgstc_client_CPPFLAGS = -Dmalloc=mock_malloc

libgstc_ping_SOURCES =			\
	test_libgstc_ping.c		\
	@top_srcdir@/libgstc/libgstc.c	\
	$(COMMON_SOURCES)

libgstc_pipeline_create_SOURCES = 		\
	test_libgstc_pipeline_create.c		\
	@top_srcdir@/libgstc/libgstc.c		\
	$(COMMON_SOURCES)

libgstc_pipeline_delete_SOURCES = 		\
	test_libgstc_pipeline_delete.c		\
	@top_srcdir@/libgstc/libgstc.c		\
	$(COMMON_SOURCES)

libgstc_pipeline_play_SOURCES = 		\
	test_libgstc_pipeline_play.c		\
	@top_srcdir@/libgstc/libgstc.c		\
	$(COMMON_SOURCES)

libgstc_pipeline_pause_SOURCES = 		\
	test_libgstc_pipeline_pause.c		\
	@top_srcdir@/libgstc/libgstc.c		\
	$(COMMON_SOURCES)

libgstc_pipeline_stop_SOURCES = 		\
	test_libgstc_pipeline_stop.c		\
	@top_srcdir@/libgstc/libgstc.c		\
	$(COMMON_SOURCES)

libgstc_json_SOURCES =		 		\
	test_libgstc_json.c			\
	@top_srcdir@/libgstc/libgstc_json.c	\
	$(COMMON_SOURCES)

libgstc_socket_SOURCES =	 		\
	test_libgstc_socket.c			\
	@top_srcdir@/libgstc/libgstc_socket.c	\
	$(COMMON_SOURCES)
libgstc_socket_CPPFLAGS = -Dmalloc=mock_malloc

libgstc_element_set_SOURCES =	 		\
	test_libgstc_element_set.c		\
	$(top_srcdir)/libgstc/libgstc.c		\
	$(COMMON_SOURCES)

libgstc_pipeline_inject_eos_SOURCES =	 	\
	test_libgstc_pipeline_inject_eos.c	\
	$(top_srcdir)/libgstc/libgstc.c		\
	$(COMMON_SOURCES)

libgstc_pipeline_bus_wait_async_SOURCES = 	\
	test_libgstc_pipeline_bus_wait_async.c	\
	$(top_srcdir)/libgstc/libgstc.c		\
	$(COMMON_SOURCES)

libgstc_pipeline_bus_wait_SOURCES = 	\
	test_libgstc_pipeline_bus_wait.c	\
	$(top_srcdir)/libgstc/libgstc_json.c	\
	$(top_srcdir)/libgstc/libgstc.c		\
	$(COMMON_SOURCES)

libgstc_pipeline_flush_start_SOURCES = 		\
	test_libgstc_pipeline_flush_start.c	\
	@top_srcdir@/libgstc/libgstc.c		\
	$(COMMON_SOURCES)

libgstc_pipeline_flush_stop_SOURCES = 		\
	test_libgstc_pipeline_flush_stop.c	\
	@top_srcdir@/libgstc/libgstc.c		\
	$(COMMON_SOURCES)
