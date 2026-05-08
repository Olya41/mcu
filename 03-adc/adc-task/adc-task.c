#include "hardware/adc.h"
#include "stdio.h"
#include "pico/stdlib.h"
#include "adc-task.h"

#define ADC_VOLTAGE 3.3f
#define ADC_BITS 12

const uint ADC_PIN = 26;
const uint ADC_CHANNEL_TEMP = 4;
const uint ADC_CHANNEL = 0;

const uint32_t ADC_TASK_MEAS_PERIOD_US = 100000;

static adc_task_state_t adc_state = ADC_TASK_STATE_IDLE;
static uint64_t last_meas_ts_us = 0;


void adc_task_init(void)
{
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(ADC_CHANNEL);
    adc_set_temp_sensor_enabled(true);
}


void get_adc_voltage(void)
{
    adc_select_input(ADC_CHANNEL);
    uint16_t voltage_counts = adc_read();
    float voltage_V = voltage_counts * ADC_VOLTAGE / (1 << ADC_BITS);

    printf("%f\n", voltage_V);
}

void get_adc_temp(void)
{
    adc_select_input(ADC_CHANNEL_TEMP);
    uint16_t temp_counts = adc_read();
    float temp_V = temp_counts * ADC_VOLTAGE / (1 << ADC_BITS);
    float temp_C = 27.0f - (temp_V - 0.706f) / 0.001721f;

    printf("%f\n", temp_C);
}

void adc_task_set_state(adc_task_state_t state)
{
    adc_state = state;
}

void adc_task_handle(void)
{
    if (adc_state != ADC_TASK_STATE_RUN) {
        return;
    }

    uint64_t now = time_us_64();
    if (now - last_meas_ts_us < ADC_TASK_MEAS_PERIOD_US) {
        return;
    }
    last_meas_ts_us = now;

    adc_select_input(ADC_CHANNEL);
    uint16_t voltage_counts = adc_read();
    float voltage_V = voltage_counts * ADC_VOLTAGE / (1 << ADC_BITS);

    adc_select_input(ADC_CHANNEL_TEMP);
    uint16_t temp_counts = adc_read();
    float temp_V = temp_counts * ADC_VOLTAGE / (1 << ADC_BITS);
    float temp_C = 27.0f - (temp_V - 0.706f) / 0.001721f;

    printf("%f %f\n", voltage_V, temp_C);
}

