#include <stdio.h>
#include "pico/stdlib.h"
#include "stdio-task/stdio-task.h"
#include "protocol-task/protocol-task.h"

#define DEVICE_NAME "Pico-Controller"
#define DEVICE_VRSN "v1.0.1"

const uint LED_PIN = 25;
void version_callback(const char* args);

api_t device_api[] = {
    {"version", version_callback, "show firmware version"},
    {NULL, NULL, NULL}
};

void version_callback(const char* args)
{
	printf("device name: '%s', firmware version: %s\n", DEVICE_NAME, DEVICE_VRSN);
}


int main() {
    stdio_init_all();

    stdio_task_init();
    protocol_task_init(device_api);

    while (1) {
        char* command = stdio_task_handle();
        protocol_task_handle(command);
    }
}
