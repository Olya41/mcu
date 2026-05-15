#include <stdio.h>
#include "pico/stdlib.h"

#include "stdio-task/stdio-task.h"
#include "protocol-task/protocol-task.h"
#include "led-task/led-task.h"

#define DEVICE_NAME "Pico-Controller"
#define DEVICE_VRSN "v1.0.1"

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

void help_callback(const char* args);

const api_t device_api[] = {
	{"version",    version_callback,                 "print device name and firmware version"},
	{"on",         led_on_callback,                  "turn LED on"},
	{"off",        led_off_callback,                 "turn LED off"},
	{"blink",      led_blink_callback,               "blink LED"},
	{"set_period", led_blink_set_period_ms_callback, "set blink period in ms"},
	{"mem",        mem_callback,                     "read word from address: mem <addr_hex>"},
	{"wmem",       wmem_callback,                    "write word to address: wmem <addr_hex> <val_hex>"},
	{"help",       help_callback,                    "print commands description"},
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
