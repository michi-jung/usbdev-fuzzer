#include <stdio.h>
#include <stdlib.h>
#include <argp.h>

#include <libusb-1.0/libusb.h>

/* Argument Parsing */

const char *argp_program_version = PACKAGE_VERSION;
const char *argp_program_bug_address = "mijung@gmx.net";

static const struct argp_option argp_options[] = {
	{ NULL, 's', "bus:devnum", 0, "Fuzz the USB device at bus:devnum", 0 },
	{ 0 }
};

static error_t arg_parser(int key, char *arg, struct argp_state *state)
{
	switch (key) {
		case 's':
			break;

		default:
			return ARGP_ERR_UNKNOWN;
	}

	return 0;
}

static const struct argp argp_parser = {
	.options = argp_options,
	.parser = arg_parser
};

#define VENDOR_ID 0x1D6Bu
#define PRODUCT_ID 0x0129u

static int dump_device_descriptor(libusb_device_handle *dev)
{
	struct libusb_device_descriptor desc;
	int rc = 0;

	rc = libusb_get_device_descriptor(libusb_get_device(dev), &desc);
	if (rc)
		return rc;

	printf("bLength:            %hhu\n",     desc.bLength);
	printf("bDescriptorType:    %hhu\n",     desc.bDescriptorType);
	printf("bcdUSB:             0x%04hx\n",  desc.bcdUSB);
	printf("bDeviceClass:       0x%02hhx\n", desc.bDeviceClass);
	printf("bDeviceSubClass:    0x%02hhx\n", desc.bDeviceSubClass);
	printf("bDeviceProtocol:    0x%02hhx\n", desc.bDeviceProtocol);
	printf("bMaxPacketSize0:    %hhu\n",     desc.bMaxPacketSize0);
	printf("idVendor:           0x%04hx\n",  desc.idVendor);
	printf("idProduct:          0x%04hx\n",  desc.idProduct);
	printf("bcdDevice:          0x%04hx\n",  desc.bcdDevice);
	printf("iManufacturer:      %hhu\n",     desc.iManufacturer);
	printf("iProduct:           %hhu\n",     desc.iProduct);
	printf("iSerialNumber:      %hhu\n",     desc.iSerialNumber);
	printf("bNumConfigurations: %hhu\n",     desc.bNumConfigurations);

	return rc;
}

int main(int argc, char *argv[])
{
	libusb_device_handle *dev = NULL;
	int rc = 0;

	argp_parse(&argp_parser, argc, argv, 0, NULL, NULL);

	rc = libusb_init(NULL);
	if (rc < 0)
		return EXIT_FAILURE;

	dev = libusb_open_device_with_vid_pid(NULL, VENDOR_ID, PRODUCT_ID);
	if (!dev) {
		fprintf(stderr,
			"Failed to open device (vid: 0x%04x, pid: 0x%04x)\n",
			VENDOR_ID, PRODUCT_ID);

		return EXIT_FAILURE;
	}

	rc = dump_device_descriptor(dev);
	if (rc < 0)
		return EXIT_FAILURE;

	libusb_close(dev);
	libusb_exit(NULL);

	return EXIT_SUCCESS;
}
