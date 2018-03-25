MAINTAINERCLEANFILES = $(srcdir)/Makefile.in
ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = usbdev-fuzzer

usbdev_fuzzer_SOURCES = usbdev-fuzzer.c argp.c fuzz-setup.c
usbdev_fuzzer_CPPFLAGS = -I$(top_srcdir)/src/include
usbdev_fuzzer_CFLAGS = $(LIBUSB_CFLAGS)
usbdev_fuzzer_LDADD = $(LIBUSB_LIBS)
