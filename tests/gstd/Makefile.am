TESTS = test_gstd_session test_gstd_state

check_PROGRAMS = $(TESTS)

test_gstd_session_SOURCES = test_gstd_session.c 

test_gstd_session_CFLAGS = $(GST_CFLAGS) -I$(top_srcdir)/gstd/
test_gstd_session_LDFLAGS = $(GST_LIBS)
test_gstd_session_LDADD = $(top_srcdir)/gstd/libgstd-core.la

test_gstd_state_SOURCES = test_gstd_state.c

test_gstd_state_CFLAGS = $(GST_CFLAGS) -I$(top_srcdir)/gstd/
test_gstd_state_LDFLAGS = $(GST_LIBS)
test_gstd_state_LDADD = $(top_srcdir)/gstd/libgstd-core.la
