#include <stdio.h>
#include "pico/stdlib.h"
#include "stdio-task/stdio-task.h"

#define DEVICE_NAME "Pico-Controller"
#define DEVICE_VRSN "v1.0.1"

const uint LED_PIN = 25;

int main() {
    stdio_init_all();

    stdio_task_init();
    while (1) {
        stdio_task_handle();
    }
}
