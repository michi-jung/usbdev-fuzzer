#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <argp.h>

#include <libusb-1.0/libusb.h>

/* Argument Parsing */

struct usbdev_fuzzer_args {
	struct {
		bool bus_devnum : 1;
		bool vendor_product : 1;
		uint8_t bus;
		uint8_t devnum;
		uint16_t vendor;
		uint16_t product;
	} dev_filter;
};

const char *argp_program_version = PACKAGE_VERSION;
const char *argp_program_bug_address = "mijung@gmx.net";

static const struct argp_option argp_options[] = {
	{ NULL, 's', "bus:devnum", 0, "Fuzz the USB device at bus:devnum", 0 },
	{ NULL, 'd', "vendor:product", 0, "Fuss the USB device with given vendor and product ID", 0 },
	{ 0 }
};

static error_t parse_s_arg(char *arg, struct usbdev_fuzzer_args * args)
{
	char *tok = NULL, *value = strdup(arg);
	long long bus = 0, devnum = 0;
	error_t err = EINVAL;

	tok = strtok(value, ":");
	if (!tok)
		goto done;
	bus = strtoll(tok, NULL, 10);

	tok = strtok(NULL, ":");
	if (!tok)
		goto done;
	devnum = strtoll(tok, NULL, 10);

	if (strtok(NULL, ":"))
		goto done;

	if ((bus > UINT8_MAX) || (devnum > UINT8_MAX))
		goto done;

	args->dev_filter.bus_devnum = true;
	args->dev_filter.bus = (uint8_t)bus;
	args->dev_filter.devnum = (uint8_t)devnum;
	err = 0;

done:
	free(value);
	return err;
}

static error_t parse_d_arg(char *arg, struct usbdev_fuzzer_args * args)
{
	char *tok = NULL, *value = strdup(arg);
	long long vendor = 0, product = 0;
	error_t err = EINVAL;

	tok = strtok(value, ":");
	if (!tok)
		goto done;
	vendor = strtoll(tok, NULL, 16);

	tok = strtok(NULL, ":");
	if (!tok)
		goto done;
	product = strtoll(tok, NULL, 16);

	if (strtok(NULL, ":"))
		goto done;

	if ((vendor > UINT16_MAX) || (product > UINT16_MAX))
		goto done;

	args->dev_filter.vendor_product = true;
	args->dev_filter.vendor = (uint16_t)vendor;
	args->dev_filter.product = (uint16_t)product;
	err = 0;

done:
	free(value);
	return err;
}

static error_t arg_parser(int key, char *arg, struct argp_state *state)
{
	struct usbdev_fuzzer_args *args = (struct usbdev_fuzzer_args *)state->input;
	error_t err = 0;

	switch (key) {
		case 's':
			err = parse_s_arg(arg, args);
			if (err)
				argp_error(state, "Failed to parse '-s' value '%s': %s\n", arg, strerror(err));
			break;

		case 'd':
			err = parse_d_arg(arg, args);
			if (err)
				argp_error(state, "Failed to parse '-d' value '%s': %s\n", arg, strerror(err));
			break;

		default:
			err = ARGP_ERR_UNKNOWN;
	}

	return err;
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
	struct usbdev_fuzzer_args args = { 0 };
	libusb_device_handle *dev = NULL;
	int rc = 0;

	argp_parse(&argp_parser, argc, argv, 0, NULL, &args);

	if (args.dev_filter.bus_devnum)
		printf("bus: %hhu, devnum: %hhu\n", args.dev_filter.bus, args.dev_filter.devnum);

	if (args.dev_filter.vendor_product)
		printf("vendor: 0x%04hx, product: 0x%04hx\n", args.dev_filter.vendor, args.dev_filter.product);

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
