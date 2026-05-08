#pragma once

typedef enum
{
    ADC_TASK_STATE_IDLE = 0,
    ADC_TASK_STATE_RUN  = 1,
} adc_task_state_t;

void adc_task_init(void);
void get_adc_voltage(void);
void get_adc_temp(void);

void adc_task_set_state(adc_task_state_t state);
void adc_task_handle(void);
