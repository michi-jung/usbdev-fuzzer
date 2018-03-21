#ifndef SRC_INCLUDE_USBDEV_FUZZER
#define SRC_INCLUDE_USBDEV_FUZZER

#include <stdbool.h>
#include <stdint.h>
#include <argp.h>

struct usbdev_filter {
	bool bus_devnum : 1;
	bool vendor_product : 1;
	uint8_t bus;
	uint8_t devnum;
	uint16_t vendor;
	uint16_t product;
};

struct usbdev_fuzzer_args {
	struct usbdev_filter dev_filter;
};

extern const struct argp usbdev_fuzzer_argp_parser;

#endif /* ndef SRC_INCLUDE_USBDEV_FUZZER */
