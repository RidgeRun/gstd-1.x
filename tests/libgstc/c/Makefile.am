TESTS = 				\
	libgstc_client			\
	libgstc_ping			\
	libgstc_pipeline_create		\
	libgstc_pipeline_delete		\
	libgstc_pipeline_play   	\
	libgstc_pipeline_pause		\
	libgstc_pipeline_stop		\
	libgstc_pipeline_get_graph	\
	libgstc_json			\
	libgstc_socket			\
	libgstc_element_set		\
	libgstc_pipeline_inject_eos 	\
	libgstc_pipeline_bus_wait_async \
	libgstc_pipeline_bus_wait 	\
	libgstc_pipeline_flush_start 	\
	libgstc_pipeline_flush_stop 	\
	libgstc_pipeline_seek		\
	libgstc_debug			\
	libgstc_pipeline_list_elements	\
	libgstc_pipeline_list 		\
	libgstc_pipeline_list_properties\
	libgstc_pipeline_element_get	\
	libgstc_pipeline_verbose        \
	libgstc_pipeline_get_state      \
	libgstc_pipeline_list_signals   \
	libgstc_pipeline_signal_connect \
	libgstc_pipeline_signal_disconnect

check_PROGRAMS = $(TESTS)

AM_CFLAGS = $(GST_CFLAGS) $(JANSSON_CFLAGS) $(GIO_CFLAGS) -I$(top_srcdir)/libgstc/c
AM_LDFLAGS = $(GST_LIBS) $(JANSSON_LIBS) $(GIO_LIBS)

COMMON_SOURCES = \
	@top_srcdir@/libgstc/c/libgstc_assert.c \
	@top_srcdir@/libgstc/c/libgstc_thread.c


libgstc_client_SOURCES =		\
	test_libgstc_client.c		\
	@top_srcdir@/libgstc/c/libgstc.c	\
	$(COMMON_SOURCES)
libgstc_client_CPPFLAGS = -Dmalloc=mock_malloc

libgstc_ping_SOURCES =			\
	test_libgstc_ping.c		\
	@top_srcdir@/libgstc/c/libgstc.c	\
	$(COMMON_SOURCES)

libgstc_pipeline_create_SOURCES = 		\
	test_libgstc_pipeline_create.c		\
	@top_srcdir@/libgstc/c/libgstc.c	\
	$(COMMON_SOURCES)

libgstc_pipeline_delete_SOURCES = 		\
	test_libgstc_pipeline_delete.c		\
	@top_srcdir@/libgstc/c/libgstc.c	\
	$(COMMON_SOURCES)

libgstc_pipeline_play_SOURCES = 		\
	test_libgstc_pipeline_play.c		\
	@top_srcdir@/libgstc/c/libgstc.c	\
	$(COMMON_SOURCES)

libgstc_pipeline_pause_SOURCES = 		\
	test_libgstc_pipeline_pause.c		\
	@top_srcdir@/libgstc/c/libgstc.c	\
	$(COMMON_SOURCES)

libgstc_pipeline_stop_SOURCES = 		\
	test_libgstc_pipeline_stop.c		\
	@top_srcdir@/libgstc/c/libgstc.c	\
	$(COMMON_SOURCES)

libgstc_pipeline_get_graph_SOURCES =	\
	test_libgstc_pipeline_get_graph.c	\
	@top_srcdir@/libgstc/c/libgstc.c	\
		$(COMMON_SOURCES)

libgstc_json_SOURCES =		 		\
	test_libgstc_json.c			\
	@top_srcdir@/libgstc/c/libgstc_json.c	\
	$(COMMON_SOURCES)

libgstc_socket_SOURCES =	 		\
	test_libgstc_socket.c			\
	@top_srcdir@/libgstc/c/libgstc_socket.c	\
	$(COMMON_SOURCES)
libgstc_socket_CPPFLAGS = -Dmalloc=mock_malloc

libgstc_element_set_SOURCES =	 		\
	test_libgstc_element_set.c		\
	@top_srcdir@/libgstc/c/libgstc.c	\
	$(COMMON_SOURCES)

libgstc_pipeline_inject_eos_SOURCES =	 	\
	test_libgstc_pipeline_inject_eos.c	\
	@top_srcdir@/libgstc/c/libgstc.c	\
	$(COMMON_SOURCES)

libgstc_pipeline_bus_wait_async_SOURCES = 	\
	test_libgstc_pipeline_bus_wait_async.c	\
	@top_srcdir@/libgstc/c/libgstc.c	\
	$(COMMON_SOURCES)

libgstc_pipeline_bus_wait_SOURCES = 	\
	test_libgstc_pipeline_bus_wait.c	\
	@top_srcdir@/libgstc/c/libgstc_json.c	\
	@top_srcdir@/libgstc/c/libgstc.c	\
	$(COMMON_SOURCES)

libgstc_pipeline_flush_start_SOURCES = 		\
	test_libgstc_pipeline_flush_start.c	\
	@top_srcdir@/libgstc/c/libgstc.c	\
	$(COMMON_SOURCES)

libgstc_pipeline_flush_stop_SOURCES = 		\
	test_libgstc_pipeline_flush_stop.c	\
	@top_srcdir@/libgstc/c/libgstc.c	\
	$(COMMON_SOURCES)

libgstc_pipeline_list_SOURCES = 		\
	test_libgstc_pipeline_list.c		\
	@top_srcdir@/libgstc/c/libgstc.c	\
	$(COMMON_SOURCES)

libgstc_pipeline_seek_SOURCES =	 	\
  	test_libgstc_pipeline_seek.c		\
	@top_srcdir@/libgstc/c/libgstc.c	\
  	$(COMMON_SOURCES)

libgstc_debug_SOURCES = 			\
	test_libgstc_debug.c			\
	@top_srcdir@/libgstc/c/libgstc.c	\
	$(COMMON_SOURCES)

libgstc_pipeline_list_elements_SOURCES = 	\
	test_libgstc_pipeline_list_elements.c	\
	@top_srcdir@/libgstc/c/libgstc.c	\
	$(COMMON_SOURCES)

libgstc_pipeline_list_properties_SOURCES = 	\
	test_libgstc_pipeline_list_properties.c	\
	@top_srcdir@/libgstc/c/libgstc.c	\
	$(COMMON_SOURCES)

libgstc_pipeline_element_get_SOURCES =		\
	test_libgstc_element_get.c		\
	@top_srcdir@/libgstc/c/libgstc.c	\
		$(COMMON_SOURCES)

libgstc_pipeline_verbose_SOURCES =	\
	test_libgstc_pipeline_verbose.c	\
	@top_srcdir@/libgstc/c/libgstc.c	\
		$(COMMON_SOURCES)

libgstc_pipeline_get_state_SOURCES =	\
	test_libgstc_pipeline_get_state.c	\
	@top_srcdir@/libgstc/c/libgstc.c	\
		$(COMMON_SOURCES)

libgstc_pipeline_signal_connect_SOURCES =       \
        test_libgstc_pipeline_signal_connect.c  \
        @top_srcdir@/libgstc/c/libgstc.c        \
        $(COMMON_SOURCES)

libgstc_pipeline_signal_disconnect_SOURCES =      \
        test_libgstc_pipeline_signal_disconnect.c \
        @top_srcdir@/libgstc/c/libgstc.c          \
        $(COMMON_SOURCES)

libgstc_pipeline_list_signals_SOURCES =         \
        test_libgstc_pipeline_list_signals.c    \
        @top_srcdir@/libgstc/c/libgstc.c        \
        $(COMMON_SOURCES)

