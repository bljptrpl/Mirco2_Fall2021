#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <driver/ledc.h>
#include <driver/adc.h>


#define LED 4
#define BUTTON 18                                //GPIO18
#define GPIO_INPUT_SEL 1ULL << BUTTON           //set GPIO select for INPUT
#define ESP_INTR_FLAG_DEFAULT 0                //INTERRUPT DEFAULT FLAG

static xQueueHandle duty_queue = NULL;        //global queue handle
void GPIO_Init()
{
    //  GPIO config: INPUT PULL_UP NEG_EDGE GPIO_2
    gpio_config_t io;
    io.pin_bit_mask = GPIO_INPUT_SEL;
    io.mode = GPIO_MODE_INPUT;
    io.pull_down_en = 0;
    io.pull_up_en = 1;
    io.intr_type = GPIO_INTR_NEGEDGE;
    gpio_config(&io);
    // setup isr
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

}


void ADCtask(void *pvParameter){
    while(1){
        vTaskDelay(100/portTICK_PERIOD_MS);
        /* Read ADC voltage @ channel */
        int adc_value = adc1_get_raw(ADC1_CHANNEL_0);
        xQueueSendToBack(duty_queue, &adc_value,(TickType_t)10);

    }
}
void PWMtask(void *pvParameter)
{   int value;         /* value received from the queue */
    while(1)
    {
        if(xQueueReceive(duty_queue,&value,(TickType_t)10) == pdPASS){
        printf("VALUE FROM ADC: %d\n",value);
        ledc_set_duty(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_0,(uint32_t)value);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        }
    }
}
void setADC()
{
    // config 12 bit resolution ADC/
    adc1_config_width(ADC_WIDTH_12Bit);
    /*!<The input voltage of ADC will be attenuated extending the range of measurement
    by about 11 dB (3.55 x) */
    adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_DB_11 );
}

void setPWM(){

    ledc_timer_config_t timerConfig;
    timerConfig.duty_resolution = LEDC_TIMER_12_BIT;
    timerConfig.timer_num = LEDC_TIMER_0;
    timerConfig.freq_hz = 5000;
    timerConfig.speed_mode = LEDC_LOW_SPEED_MODE;
    timerConfig.clk_cfg = LEDC_AUTO_CLK;
    ledc_timer_config(&timerConfig);
    ledc_channel_config_t tChaConfig;
    tChaConfig.gpio_num = LED;
    tChaConfig.speed_mode = LEDC_LOW_SPEED_MODE;
    tChaConfig.channel = LEDC_CHANNEL_0;
    tChaConfig.intr_type = LEDC_INTR_DISABLE;
    tChaConfig.timer_sel =LEDC_TIMER_0;
    /* 5000 duty = 3.3v  e.g. 2500 = 3.3v/2 */
    tChaConfig.duty = 0;
    ledc_channel_config(&tChaConfig);
}
void app_main(){
    GPIO_Init();
    setADC();
    setPWM();
    duty_queue = xQueueCreate(10, sizeof(int));
    xTaskCreate(&ADCtask,"ADCtask",2048,NULL,5,NULL);
    xTaskCreate(&PWMtask,"PWMtask",2048,NULL,5,NULL);
    }
