bin_PROGRAMS = gstd

# list of source files
# the prefix is the name of the binary
gstd_SOURCES = 	main.c \
		gstd.c \
		gstd_pipeline.c

# list of headers we're not going to install
noinst_HEADERS = return_codes.h \
		 gstd.h \
		 gstd_pipeline.h

# our CFLAGS and LDFLAGS used for compiling and linking
gstd_CFLAGS = $(GST_CFLAGS)
gstd_LDFLAGS = $(GST_LIBS)
