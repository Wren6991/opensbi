/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 */

#include <sbi/riscv_asm.h>
#include <sbi/riscv_encoding.h>
#include <sbi/sbi_const.h>
#include <sbi/sbi_platform.h>

/*
 * Include these files as needed.
 * See objects.mk PLATFORM_xxx configuration parameters.
 */
#include <sbi_utils/ipi/aclint_mswi.h>
#include <sbi_utils/irqchip/plic.h>
#include <sbi_utils/serial/uart8250.h>
#include <sbi_utils/timer/aclint_mtimer.h>

#define PLATFORM_HART_COUNT         1
#define PLATFORM_MTIMER_FREQ 1000000
#define PLATFORM_MTIMER_ADDR 0xe0008000
#define PLATFORM_UART_ADDR          0xe0004000
#define PLATFORM_UART_INPUT_FREQ    10000000
#define PLATFORM_UART_BAUDRATE      115200

static struct aclint_mtimer_data mtimer = {
	.mtime_freq = PLATFORM_MTIMER_FREQ,
	.mtime_addr = PLATFORM_MTIMER_ADDR,
	.mtime_size = 8,
	.mtimecmp_addr = PLATFORM_MTIMER_ADDR + 8,
	.mtimecmp_size = 8 * PLATFORM_HART_COUNT,
	.first_hartid = 0,
	.hart_count = PLATFORM_HART_COUNT,
	.has_64bit_mmio = false,
};

static int platform_early_init(bool cold_boot)
{
	return 0;
}

static int platform_final_init(bool cold_boot)
{
	return 0;
}

static int platform_console_init(void)
{
	/* Example if the generic UART8250 driver is used */
	return uart8250_init(PLATFORM_UART_ADDR, PLATFORM_UART_INPUT_FREQ,
			     PLATFORM_UART_BAUDRATE, 0, 1, 0);
}

// Initialize the platform interrupt controller for current HART only
static int platform_irqchip_init(bool cold_boot)
{
	// TODO rvcpp
	return 0;
}

// Initialize IPI for current HART only
static int platform_ipi_init(bool cold_boot)
{
	// TODO rvcpp
	return 0;
}

// Initialize platform timer for current HART
static int platform_timer_init(bool cold_boot)
{
	int ret;

	if (cold_boot) {
		ret = aclint_mtimer_cold_init(&mtimer, NULL);
		if (ret)
			return ret;
	}

	return aclint_mtimer_warm_init();
}


// Platform descriptor.

const struct sbi_platform_operations platform_ops = {
	.early_init   = platform_early_init,
	.final_init   = platform_final_init,
	.console_init = platform_console_init,
	.irqchip_init = platform_irqchip_init,
	.ipi_init     = platform_ipi_init,
	.timer_init   = platform_timer_init
};

const struct sbi_platform platform = {
	.opensbi_version   = OPENSBI_VERSION,
	.platform_version  = SBI_PLATFORM_VERSION(0x0, 0x00),
	.name              = "rvcpp-sim-platform",
	.features          = SBI_PLATFORM_DEFAULT_FEATURES,
	.hart_count        = PLATFORM_HART_COUNT,
	.hart_stack_size   = SBI_PLATFORM_DEFAULT_HART_STACK_SIZE,
	.platform_ops_addr = (unsigned long)&platform_ops
};
