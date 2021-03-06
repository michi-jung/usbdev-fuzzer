#ifndef SRC_INCLUDE_USBDEV_FUZZER
#define SRC_INCLUDE_USBDEV_FUZZER

#include <stdbool.h>
#include <stdint.h>
#include <argp.h>

#include <libusb-1.0/libusb.h>

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

int usbdev_fuzz_setup(libusb_device_handle *dev_handle);

/* Vendor Specific USB Requests */

#define USBDEV_FUZZER_REQ_ECHOONBULKIN (0)

#endif /* ndef SRC_INCLUDE_USBDEV_FUZZER */
