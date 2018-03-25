#include <stdio.h>
#include <usbdev-fuzzer.h>

int usbdev_fuzz_setup(libusb_device_handle *dev)
{
	char hello[] = "Hello NuttX!";
	int rc = 0;

	rc = libusb_set_configuration(dev, 1);
	if (rc) {
		fprintf(stderr, "libusb_set_configuration() failed. rc %d\n",
			rc);
		goto done;
	}

	rc = libusb_claim_interface(dev, 0);
	if (rc) {
		fprintf(stderr, "libusb_claim_interface() failed. rc: %d\n",
			rc);
		goto done;
	}

	rc = libusb_control_transfer(dev, LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR |
				     LIBUSB_RECIPIENT_DEVICE, 0x01, 0x100, 0,
				     (unsigned char *)hello, sizeof(hello), 100);
	if (rc) {
		fprintf(stderr, "libusb_control_transfer() failed. rc: %d\n",
			rc);
		goto done;
	}

done:
	libusb_release_interface(dev, 0);
	return rc;
}
