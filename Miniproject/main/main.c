/*
 * Daniel Villalva
 * 10-31-21
 *
 * Implement the following deadlock problem in FreeRTOS:

 * Task 1 will wait for Task 3 and then initiate Task 2.
 * Task 2 will wait for Task 1 and then initiate Task 3.
 * Task 3 will wait for Task 2 and then initiate Task 1.
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_system.h"

SemaphoreHandle_t xSemaphore1 = NULL; //sets semaphore to NULL
SemaphoreHandle_t xSemaphore2 = NULL;
SemaphoreHandle_t xSemaphore3 = NULL;

void task1(void *pvParameter)
{
	while(1)
	{
		if( xSemaphore1 != NULL)
		{
			//vTaskDelay(1000 / portTICK_RATE_MS);
		     if( xSemaphoreTake( xSemaphore1, ( TickType_t ) 0 ) == pdTRUE )
		     {
		   		vTaskDelay(1 / portTICK_RATE_MS);
		    	 printf("Waiting for Task 3 in Task 1...\n");
		    	 xSemaphoreGive( xSemaphore2 ); //Initiate Task 2
		     }//if
		}//if
		else
		{
			printf("Error in Task 1\n");
		}


	}//while
	vTaskDelete(NULL);
}
void task2(void *pvParameter)
{
	while(1)
	{
		if( xSemaphore2 != NULL)
		{
			//vTaskDelay(1000 / portTICK_RATE_MS);
		     if( xSemaphoreTake( xSemaphore2, ( TickType_t ) 0 ) == pdTRUE )
		     {
		   		vTaskDelay(1 / portTICK_RATE_MS);
		    	 printf("Waiting for Task 1 in Task 2...\n");
		    	 xSemaphoreGive( xSemaphore3 ); //Initiate Task 2
		     }//if
		}//if
		else
		{
			printf("Error in Task 2\n");
		}
	}//while
	vTaskDelete(NULL);
}
void task3(void *pvParameter)
{
	while(1)
	{
		if( xSemaphore3 != NULL)
		{
			//vTaskDelay(1000 / portTICK_RATE_MS);
		     if( xSemaphoreTake( xSemaphore3, ( TickType_t ) 0 ) == pdTRUE )
		     {
		   		vTaskDelay(1 / portTICK_RATE_MS);
		    	 printf("Waiting for Task 2 in Task 3...\n");
		    	 xSemaphoreGive( xSemaphore1 ); //Initiate Task 2
		     }//if
		}//if
		else
		{
			printf("Error in Task 1\n");
		}
	}//while
	vTaskDelete(NULL);
}
void app_main(void)
{
	vSemaphoreCreateBinary( xSemaphore1 ); //creates binary semaphore
	vSemaphoreCreateBinary( xSemaphore2 );
	vSemaphoreCreateBinary( xSemaphore3 );
	xSemaphoreGive( xSemaphore1 );

	printf("Created Tasks 1-3\n");
	printf("Launched scheduler...\n\n");
	printf("Timer is 100 ticks per second...\n");

	xTaskCreate(&task1,"task1",4096,NULL,2,NULL);
	xTaskCreate(&task2,"task2",4096,NULL,3,NULL);
	xTaskCreate(&task3,"task3",4096,NULL,4,NULL);


}

