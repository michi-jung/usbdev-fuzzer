#include <usbdev-fuzzer.h>
#include <stdlib.h>
#include <string.h>

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

const struct argp usbdev_fuzzer_argp_parser = {
	.options = argp_options,
	.parser = arg_parser
};
