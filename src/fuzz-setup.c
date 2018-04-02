#include <stdio.h>
#include <usbdev-fuzzer.h>

int usbdev_fuzz_setup(libusb_device_handle *dev)
{
	char hello[] = "Hello NuttX!";
	char buffer[256];
	int rc = 0, transferred = 0;
	uint8_t request_type = 0;

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

	request_type = LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR |
		       LIBUSB_RECIPIENT_DEVICE;

	rc = libusb_control_transfer(dev, request_type,
				     USBDEV_FUZZER_REQ_ECHOONBULKIN,
                                     0, 0,
				     (unsigned char *)hello, sizeof(hello),
				     100 /*ms*/);
	if (rc < 0) {
		fprintf(stderr, "libusb_control_transfer() failed. rc: %d\n",
			rc);
		goto done;
	}

	rc = libusb_bulk_transfer(dev, LIBUSB_ENDPOINT_IN | 0x02,
				  (unsigned char *)buffer, sizeof(buffer),
				  &transferred, 100);
	if (rc) {
		fprintf(stderr, "libusb_bulk_transfer() failed. rc: %d\n",
			rc);
		goto done;
	}

        printf("Received (%d): '%.*s'\n", transferred, transferred, buffer);

done:
	libusb_release_interface(dev, 0);
	return rc;
}
