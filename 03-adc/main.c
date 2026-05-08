#include <stdio.h>
#include "pico/stdlib.h"
#include "stdio-task/stdio-task.h"
#include "protocol-task/protocol-task.h"
#include "adc-task/adc-task.h"

#define DEVICE_NAME "Pico-Controller"
#define DEVICE_VRSN "v1.0.1"

const uint LED_PIN = 25;
void version_callback(const char* args);
void get_adc_callback(const char* args);
void get_adc_temp_callback(const char* args);
void tm_start_callback(const char* args);
void tm_stop_callback(const char* args);

api_t device_api[] = {
    {"version", version_callback, "show firmware version"},
    {"get_adc", get_adc_callback, "read ADC voltage"},
    {"get_temp", get_adc_temp_callback, "read die temperature"},
    {"tm_start", tm_start_callback,   "start telemetry"},
    {"tm_stop",  tm_stop_callback,    "stop telemetry"},
    {NULL, NULL, NULL}
};

void version_callback(const char* args)
{
	printf("device name: '%s', firmware version: %s\n", DEVICE_NAME, DEVICE_VRSN);
}

void get_adc_callback(const char* args)
{
    get_adc_voltage();
}

void get_adc_temp_callback(const char* args)
{
    get_adc_temp();
}

void tm_start_callback(const char* args)
{
    adc_task_set_state(ADC_TASK_STATE_RUN);
}

void tm_stop_callback(const char* args)
{
    adc_task_set_state(ADC_TASK_STATE_IDLE);
}


int main() {
    stdio_init_all();
    adc_task_init();

    stdio_task_init();
    protocol_task_init(device_api);
    
    while (1) {
        char* command = stdio_task_handle();
        protocol_task_handle(command);
        adc_task_handle();
    }
}
