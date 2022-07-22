/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-4-Clause
 */

#include <zephyr.h>
#include <stdint.h>
#include <string.h>

#include <unity.h>
#include <stdlib.h>

#include <spi_bus.h>

// Real pin number for sx1262 NSS for nrf52840
#define NSS_PIN_NUMBER 40

static halo_serial_bus_client_t client = (halo_serial_bus_client_t){ .client_selector = NSS_PIN_NUMBER };

void test_init_spi()
{
	const halo_serial_bus_iface_t *interface;

	TEST_ASSERT_EQUAL(0, bus_serial_ncs_spi_create(&interface, NULL));

}

void test_invalid_init_spi()
{
	TEST_ASSERT_EQUAL(HALO_ERROR_INCOMPATIBLE_PARAMS, bus_serial_ncs_spi_create(NULL, NULL));
}

void test_xfer_no_buffer()
{
	const halo_serial_bus_iface_t *interface = NULL;

	TEST_ASSERT_EQUAL(0, bus_serial_ncs_spi_create(&interface, NULL));
	TEST_ASSERT(interface);
	TEST_ASSERT(interface->xfer);
	halo_error_t e = interface->xfer(interface, NULL, NULL, NULL, 1);
	TEST_ASSERT_EQUAL(HALO_ERROR_INVALID_ARGS, e);
}

void test_xfer_no_interface()
{
	const halo_serial_bus_iface_t *interface = NULL;

	TEST_ASSERT_EQUAL(0, bus_serial_ncs_spi_create(&interface, NULL));
	TEST_ASSERT(interface);
	TEST_ASSERT(interface->xfer);
	uint8_t tx[] = { 0x1d, 0x08, 0xac, 0, 0 };
	uint8_t rx[5] = { 0 };
	halo_error_t e = interface->xfer(NULL, NULL, tx, rx, sizeof(rx));
	TEST_ASSERT_EQUAL(HALO_ERROR_INVALID_ARGS, e);
	uint8_t rx_expect[5] = { 0 };
	TEST_ASSERT_EQUAL_MEMORY(rx_expect, rx, sizeof(rx));
}

void test_xfer_wrong_interface()
{
	const halo_serial_bus_iface_t *interface = NULL;
	const halo_serial_bus_iface_t interface2;

	TEST_ASSERT_EQUAL(0, bus_serial_ncs_spi_create(&interface, NULL));
	TEST_ASSERT(interface);
	TEST_ASSERT(interface->xfer);
	uint8_t tx[] = { 0x1d, 0x08, 0xac, 0, 0 };
	uint8_t rx[5] = { 0 };
	halo_error_t e = interface->xfer(&interface2, NULL, tx, rx, sizeof(rx));
	TEST_ASSERT_EQUAL(HALO_ERROR_INVALID_ARGS, e);
	uint8_t rx_expect[5] = { 0 };
	TEST_ASSERT_EQUAL_MEMORY(rx_expect, rx, sizeof(rx));
}

void test_send_spi()
{
	const halo_serial_bus_iface_t *interface = NULL;

	TEST_ASSERT_EQUAL(0, bus_serial_ncs_spi_create(&interface, NULL));
	TEST_ASSERT(interface);
	TEST_ASSERT(interface->xfer);
	uint8_t tx[] = { 0x1d, 0x08, 0xac, 0, 0 };
	uint8_t rx[5] = { 0 };
	halo_error_t e = interface->xfer(interface, &client, tx, rx, sizeof(rx));
	TEST_ASSERT_EQUAL(HALO_ERROR_NONE, e);
	for (int i = 0; i < ARRAY_SIZE(rx); i++) {
		// if shield is connected, rx will have valid response, and there should be no zeros
		// and in case where shield is not connected rx will be full of 0xff
		TEST_ASSERT(rx[i]);
		printf("%x", rx[i]);
	}
}

void test_only_tx_spi()
{
	const halo_serial_bus_iface_t *interface = NULL;

	TEST_ASSERT_EQUAL(0, bus_serial_ncs_spi_create(&interface, NULL));
	TEST_ASSERT(interface);
	TEST_ASSERT(interface->xfer);
	uint8_t tx[] = { 0x1d, 0x08, 0xac };
	halo_error_t e = interface->xfer(interface, &client, tx, NULL, sizeof(tx));
	TEST_ASSERT_EQUAL(HALO_ERROR_NONE, e);
}

extern int unity_main(void);

void main(void)
{
	(void)unity_main();
}