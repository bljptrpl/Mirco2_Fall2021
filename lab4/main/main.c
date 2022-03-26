//lab4
//ESPIP 192.168.4.1

#include <stdio.h>
#include <math.h>
#include "sdkconfig.h"
#include <driver/dac.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void triangleWave(void *pvParameter)
{
 dac_output_enable(DAC_CHANNEL_1);
 static int i = 0;
 while(1) {

	 for(i = 0; i < 200; i++){
		 dac_output_voltage(DAC_CHANNEL_1, i); //Output on GPIO 25
	 }
 	 for(i = 198; i > 0 ; i--){
 		 dac_output_voltage(DAC_CHANNEL_1, i);
 	 }
        vTaskDelay(1/portTICK_PERIOD_MS);
    }//while
}//triangle

void sineWave(void *pvParameter)
{
	dac_output_enable(DAC_CHANNEL_2); //output GPIO 26
	double val = 0, rads, pi = 3.14;
	while(1) {

		for(val = 0; val < 256; val++){
			rads = val * ((2*pi) / 256);
			dac_output_voltage(DAC_CHANNEL_2, (sin(9*pi*rads) * 127) + 128); //remove 9*pi, only there for 10Hz freq
			vTaskDelay(10/portTICK_PERIOD_MS);
			}
    }//while
}//sin

void app_main()
{
    xTaskCreate(&triangleWave,"createTriangleWave",4096,NULL,5,NULL);
    xTaskCreate(&sineWave,"createSineWave",4096,NULL,5,NULL);
}
