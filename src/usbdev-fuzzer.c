#include <usbdev-fuzzer.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libusb-1.0/libusb.h>

static libusb_device_handle *find_device(libusb_context *ctx,
					 struct usbdev_filter *filter)
{
	libusb_device **devs = NULL;
	libusb_device_handle *handle = NULL;
	ssize_t num_devs = 0, i;
	int rc = 0;

	num_devs = libusb_get_device_list(ctx, &devs);

	if (num_devs < 0) {

		fprintf(stderr, "libusb_get_device_list failed: %s\n",
			libusb_strerror(num_devs));

		return NULL;
	}

	for (i = 0; i < num_devs; i++) {

		if (filter->bus_devnum) {

			if (libusb_get_bus_number(devs[i]) != filter->bus)
				continue;

			if (libusb_get_device_address(devs[i]) != filter->devnum)
				continue;
		}

		if (filter->vendor_product) {
			struct libusb_device_descriptor desc;

			rc = libusb_get_device_descriptor(devs[i], &desc);

			if (rc < 0) {
				fprintf(stderr, "libusb_get_device_descriptor "
					"failed: %s\n", libusb_strerror(rc));
				break;
			}

			if (desc.idVendor != filter->vendor)
				continue;

			if (desc.idProduct != filter->product)
				continue;
		}

		rc = libusb_open(devs[i], &handle);

		if (rc < 0) {
			fprintf(stderr, "libusb_open failed: %s\n",
				libusb_strerror(rc));
			break;
		}

		rc = libusb_set_auto_detach_kernel_driver(handle, 1);

		if (rc < 0) {
			fprintf(stderr, "libusb_set_auto_detach_kernel_driver "
				"failed: %s\n", libusb_strerror(rc));

			libusb_close(handle);
			handle = NULL;

			break;
		}

		break;
	}

	libusb_free_device_list(devs, /* unref_devices */ true);

	return handle;
}

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
	libusb_context *ctx = NULL;
	libusb_device_handle *hnd = NULL;
	int rc = 0, exitcode = EXIT_FAILURE;

	rc = argp_parse(&usbdev_fuzzer_argp_parser, argc, argv, 0, NULL, &args);
	if (rc)
		goto done;

	rc = libusb_init(&ctx);
	if (rc < 0) {
		fprintf(stderr, "libusb_init failed. rc: %d\n", rc);
		goto done;
	}

	hnd = find_device(ctx, &args.dev_filter);
	if (!hnd) {
		fprintf(stderr, "No matching device found!\n");
		goto done;
	}

	rc = dump_device_descriptor(hnd);
	if (rc < 0) {
		fprintf(stderr, "Dumping USB Device Descriptor failed. "
			"rc: %d\n", rc);
		goto done;
	}

	rc = usbdev_fuzz_setup(hnd);
	if (rc < 0) {
		fprintf(stderr, "Setup Fuzzing failed. rc: %d\n", rc);
		goto done;
	}

	exitcode = EXIT_SUCCESS;
done:
	if (hnd)
		libusb_close(hnd);

	if (ctx)
		libusb_exit(ctx);

	return exitcode;
}
