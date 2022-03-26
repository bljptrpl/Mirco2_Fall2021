#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

QueueHandle_t xQueue1;

struct Lmao
{
	int kek;
	int data;
}Lmaoxd;
static void vSenderTask( void *pvParameters )
{
		int32_t lValueToSend;
		BaseType_t xStatus;
		lValueToSend = ( int32_t ) pvParameters;
		for( ;; )
		{
			xStatus = xQueueSendToBack( xQueue1, &lValueToSend, 0 );
		if( xStatus != pdPASS )
		{
			printf( "Could not send to the queue. \r \n" );
		}
		}//for
		vTaskDelete(NULL);
}
static void vReceiverTask( void *pvParameters )
{
	int32_t lReceivedValue;
	BaseType_t xStatus;
	const TickType_t xTicksToWait = pdMS_TO_TICKS( 100 );
	for( ;; )
	{
		if( uxQueueMessagesWaiting( xQueue1 ) != 0 )
		{
			printf( "Queue should have been empty! \r \n" );
		}
		xStatus = xQueueReceive( xQueue1, &lReceivedValue, xTicksToWait );
		if( xStatus == pdPASS )
		{
			printf("Received = %d\n", lReceivedValue );
		}
		else
		{
			printf( "Could not receive from the queue. \r \n" );
		}
	}//for
	vTaskDelete(NULL);
}//receiver

int app_main(void)
	{
		struct Lmao Lmaoxd;
//		strcpy(Lmaoxd.kek, "Omg is this working?\n");
//		strcpy(Lmaoxd.data, "Possibly...\n");
		Lmaoxd.kek = 1234;
		Lmaoxd.data = 555;
		xQueue1 = xQueueCreate(5, sizeof(int32_t));
//		xQueue2 = xQueueCreate(5, sizeof(int32_t));
		if( xQueue1 != NULL )
		{
			xTaskCreate( vSenderTask, "Sender1", 1000, ( void * )Lmaoxd.kek , 1, NULL);
			xTaskCreate( vSenderTask, "Sender2", 1000, ( void * )Lmaoxd.data , 1, NULL);
			xTaskCreate(vReceiverTask, "Receiver", 2000, NULL, 2, NULL);

		}
		else
		{}
		return 0;
	}//main


