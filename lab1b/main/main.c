//Daniel Villalva
//Lab1b_demofinish
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
QueueHandle_t xQueue;

typedef enum
{
 eSender1,
 eSender2,
} DataSource_t;

typedef struct
{
 uint8_t ucValue;
 char *p;
 DataSource_t eDataSource;

} Data_t;

static const Data_t xStructsToSend[3] =
{
 { 10, "EE4178", eSender1 }, /* Used by Sender1. */
 { 20, "Fall2021", eSender2 },/* Used by Sender2. */
};


static void vSenderTask( void *pvParameters )
{
BaseType_t xStatus;
const TickType_t xTicksToWait = pdMS_TO_TICKS( 100 );

 for( ;; )
 {
 xStatus = xQueueSendToBack( xQueue, pvParameters, xTicksToWait ); //Sends parameter to the back of the Queue.
 if( xStatus != pdPASS )
 {

 printf( "Could not send to the queue.\r\n" );
 }
 }//for
}//sender

static void vReceiverTask( void *pvParameters )
{

Data_t xReceivedStructure;
BaseType_t xStatus;

 for( ;; )
 {

 if( uxQueueMessagesWaiting( xQueue ) != 3 )
 {
 printf( "Queue should have been full!\r\n" );
 }

 xStatus = xQueueReceive( xQueue, &xReceivedStructure, 0 ); //Receives Queue and Address of Struct
 if( xStatus == pdPASS )
 {

    if( xReceivedStructure.eDataSource == eSender1 )
    {
    	printf( "From Sender 1 = %d, %s \n", xReceivedStructure.ucValue, xReceivedStructure.p);
    }
    else
    {
        printf( "From Sender 2 = %d, %s \n", xReceivedStructure.ucValue,xReceivedStructure.p);
    }
 }
 else
 {

 printf( "Could not receive from the queue.\r\n" );
 }
 }//for
}//receiver
int app_main( void )
{
 xQueue = xQueueCreate( 3, sizeof( Data_t ) );
 if( xQueue != NULL )
 {

 xTaskCreate( vSenderTask, "Sender1", 3000, &(xStructsToSend[0]), 2, NULL ); //Crea
 xTaskCreate( vSenderTask, "Sender2", 3000, &(xStructsToSend[1]), 2, NULL );
 xTaskCreate( vReceiverTask, "Receiver", 3000, NULL, 1, NULL );
 }
 else
 { }
for(;;)
{

}
}
