#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "stdio-task/stdio-task.h"
#include "protocol-task.h"
#include "led-task/led-task.h"
#include "bme280-driver.h"

#define DEVICE_NAME "Pico-Controller"
#define DEVICE_VRSN "v1.0.1"

void rp2040_i2c_read(uint8_t* buffer, uint16_t length)
{
	i2c_read_timeout_us(i2c1, 0x76, buffer, length, false, 100000);
}

void rp2040_i2c_write(uint8_t* data, uint16_t size)
{
	i2c_write_timeout_us(i2c1, 0x76, data, size, false, 100000);
}

void version_callback(const char* args)
{
	printf("%s %s\n", DEVICE_NAME, DEVICE_VRSN);
}

void led_on_callback(const char* args)
{
	led_task_state_set(LED_STATE_ON);
	printf("LED on\n");
}

void led_off_callback(const char* args)
{
	led_task_state_set(LED_STATE_OFF);
	printf("LED off\n");
}

void led_blink_callback(const char* args)
{
	led_task_state_set(LED_STATE_BLINK);
	printf("LED blink\n");
}

void led_blink_set_period_ms_callback(const char* args)
{
	uint period_ms = 0;
	sscanf(args, "%u", &period_ms);

	if (period_ms == 0)
	{
		printf("error: invalid period\n");
		return;
	}

	led_task_set_blink_period_ms(period_ms);
	printf("blink period set to %u ms\n", period_ms);
}

void mem_callback(const char* args)
{
	uint32_t addr = 0;
	sscanf(args, "%lx", &addr);

	if (addr == 0)
	{
		printf("error: invalid address\n");
		return;
	}

	uint32_t value = *(volatile uint32_t*)addr;
	printf("mem[0x%08lx] = 0x%08lx (%lu)\n", addr, value, value);
}

void wmem_callback(const char* args)
{
	uint32_t addr  = 0;
	uint32_t value = 0;
	int parsed = sscanf(args, "%lx %lx", &addr, &value);

	if (parsed != 2 || addr == 0)
	{
		printf("error: usage: wmem <addr_hex> <val_hex>\n");
		return;
	}

	*(volatile uint32_t*)addr = value;
	printf("wmem[0x%08lx] = 0x%08lx\n", addr, value);
}

void read_regs_callback(const char* args)
{
	uint32_t addr = 0;
	uint32_t N    = 0;
	int parsed = sscanf(args, "%lx %lx", &addr, &N);

	if (parsed != 2)
	{
		printf("error: usage: read_reg <addr_hex> <count_hex>\n");
		return;
	}
	if (addr > 0xFF)
	{
		printf("error: addr must be <= 0xFF\n");
		return;
	}
	if (N > 0xFF)
	{
		printf("error: count must be <= 0xFF\n");
		return;
	}
	if (addr + N > 0x100)
	{
		printf("error: addr + count must be <= 0x100\n");
		return;
	}

	uint8_t buffer[256] = {0};
	bme280_read_regs((uint8_t)addr, buffer, (uint16_t)N);

	for (int i = 0; i < (int)N; i++)
	{
		printf("bme280 register [0x%X] = 0x%X\n", (unsigned)(addr + i), buffer[i]);
	}
}

void help_callback(const char* args);

const api_t device_api[] = {
	{"version",              version_callback,                 "get device name and firmware version"},
	{"on",                   led_on_callback,                  "switch on led"},
	{"off",                  led_off_callback,                 "switch off led"},
	{"blink",                led_blink_callback,               "provide unblocking"},
	{"set_period",           led_blink_set_period_ms_callback, "blinking with arguments"},
	{"mem",                  mem_callback,                     "read from memory"},
	{"wmem",                 wmem_callback,                    "write in memory"},
	{"read_reg",             read_regs_callback,               "read bme280 registers: read_reg <addr_hex> <count_hex>"},
	{"help",                 help_callback,                    "print commands description"},
	{NULL, NULL, NULL},
};

void help_callback(const char* args)
{
	for (int i = 0; device_api[i].command_name != NULL; i++)
	{
		printf("Команда '%s': '%s'\n", device_api[i].command_name, device_api[i].command_help);
	}
}

int main()
{
	stdio_init_all();

	i2c_init(i2c1, 100000);
	gpio_set_function(14, GPIO_FUNC_I2C);
	gpio_set_function(15, GPIO_FUNC_I2C);

	bme280_init(rp2040_i2c_read, rp2040_i2c_write);

	stdio_task_init();
	led_task_init();
	protocol_task_init((api_t*)device_api);

	while (1)
	{
		char* command = stdio_task_handle();
		protocol_task_handle(command);

		led_task_handle();
	}
}
