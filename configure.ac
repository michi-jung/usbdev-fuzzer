AC_INIT(usbdev-fuzzer, 0.1)
AC_CANONICAL_TARGET
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE
AC_PROG_CC
AC_CONFIG_FILES([Makefile src/Makefile])
AC_ARG_ENABLE(strict,
		AS_HELP_STRING([--disable-strict],
			       [disable strict error checking]),
		[case "${enableval}" in
			yes) CFLAGS+=" -Wall -Werror" ;;
			no)  ;;
			*) AC_MSG_ERROR([bad value ${enableval} for --disable-strict]) ;;
		 esac],
		[CFLAGS+=" -Wall -Werror"])
PKG_CHECK_MODULES(LIBUSB, libusb-1.0)
AC_OUTPUT
