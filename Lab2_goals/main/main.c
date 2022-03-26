//Daniel Villalva
//Lab2_goals
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
/*Initializations
 * GPIO:
 * GPIO18: output
 * GPIO4:  input, pulled up, interrupt from rising edge and falling edge
 * GPIO5:  input, pulled up, interrupt from rising edge.
 *
 * Test:
 * Connect GPIO18 with GPIO4
 * Connect GPIO19 with GPIO5
 * Generate pulses on GPIO19, that triggers interrupt on GPIO4/5
 */
#define GPIO_INPUT_IO_0     4
#define GPIO_INPUT_IO_1     5
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_0) | (1ULL<<GPIO_INPUT_IO_1))
#define GPIO_OUTPUT_IO_0    19
#define GPIO_OUTPUT_PIN_SEL  (1ULL<<GPIO_OUTPUT_IO_0)

#define ESP_INTR_FLAG_DEFAULT 0


static xQueueHandle gpio_queue = NULL;
static void IRAM_ATTR gpio_isr_handler(void* arg)
{
 GPIO.out ^= GPIO_OUTPUT_PIN_SEL;//output/LED
 uint32_t gpio_num = (uint32_t) arg;
 xQueueSendFromISR(gpio_queue, &gpio_num, NULL);//Send gpio data into queue
}
static void print_task(void* arg)
{
uint32_t gpio_num = (uint32_t) arg;
 	 while(1)
 	 {

 		 if(xQueueReceive(gpio_queue, &gpio_num, 100))	//receive gpio data
 		 {
 			 printf("GPIO[%d] caused an interrupt\n", gpio_num);
 		 }
 	 }//while
}//task
void setUpGPIO()
{
 gpio_config_t io_conf; //initialize config struture
 //INPUT
 io_conf.intr_type = GPIO_INTR_POSEDGE; //interrupt of rising edge
 io_conf.mode = GPIO_MODE_INPUT;//set as input
 io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL; //(1ULL << 4 | 1ULL <<5)
 io_conf.pull_down_en = 0;//disable pull-down mode
 io_conf.pull_up_en = 1;//enable pull-up mode
 gpio_config(&io_conf);//config GPIO with given settings
 //OUTPUT
 io_conf.intr_type = GPIO_INTR_DISABLE;// disable interrupt
 io_conf.mode = GPIO_MODE_OUTPUT;//set as output mode
 io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL; //bit mask of the pins
 io_conf.pull_down_en = 0;//disable pull-down mode
 io_conf.pull_up_en = 0;//disable pull-up mode
 gpio_config(&io_conf);
 //Set ISR
 gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
 gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0); //gpio_isr_handler_add(5, gpio_isr_handler, (void*) 5);
 gpio_isr_handler_add(GPIO_INPUT_IO_1, gpio_isr_handler, (void*) GPIO_INPUT_IO_1);
 gpio_isr_handler_remove(GPIO_OUTPUT_IO_0); //remove isr from output
}
void app_main()
{
setUpGPIO();
gpio_queue = xQueueCreate(10, sizeof(uint32_t));

xTaskCreate(&print_task, "print_task", 2048, NULL, 10, NULL);
}
