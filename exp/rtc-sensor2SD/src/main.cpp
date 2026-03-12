/***
 * Write temperature readings to an SD card every 10 seconds
 * -- Jon Durrant
 * March-2026
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <pico/stdlib.h>
#include "filesystem/vfs.h"
#include "hardware/adc.h"
#include "pico/aon_timer.h"
#include "pico/util/datetime.h"
#include <time.h>

#include "FreeRTOS.h"
#include "task.h"
#include "WifiHelper.h"

#define TASK_PRIORITY			( tskIDLE_PRIORITY + 1UL )


const char*   TARGET_FOLDER = "/";
const char* DATA_FILE="/sensor.csv";

void tempSensorInit(){
	adc_init();
	adc_set_temp_sensor_enabled(true);
	adc_select_input(4);
}

float readOnboardTemperature() {

    /* 12-bit conversion, assume max value == ADC_VREF == 3.3 V */
    const float conversionFactor = 3.3f / (1 << 12);

    float adc = (float)adc_read() * conversionFactor;
    float tempC = 27.0f - (adc - 0.706f) / 0.001721f;

    return tempC;
}

void newReading(){
	struct timespec ts;
	if (!aon_timer_is_running()){
		return;
	}
	FILE *fp = fopen(DATA_FILE, "a");
	if (fp != NULL){
		aon_timer_get_time (&ts);
	   char buff[100];
		strftime(buff, sizeof buff, "%D %T", gmtime(&ts.tv_sec));
		float c = readOnboardTemperature();
		fprintf(fp, "%s, %.3f\n",
				buff,
				c
				);
		printf("%s, %.3f\n",
						buff,
						c
						);
		fclose(fp);
	} else {
		printf("Open for A failed\n");
	}
}


void main_task(void *params){

	printf("Main task started\n");

	if (WifiHelper::init()){
		printf("Wifi Controller Initialised\n");
	} else {
		printf("Failed to initialise controller\n");
		return;
	}


	printf("Connecting to WiFi... %s \n", WIFI_SSID);

	if (WifiHelper::join(WIFI_SSID, WIFI_PASSWORD)){
		printf("Connected to Wifi\n");
	} else {
		printf("Failed to connect to Wifi \n");
	}


	//Print MAC Address
	char macStr[20];
	WifiHelper::getMACAddressStr(macStr);
	printf("MAC ADDRESS: %s\n", macStr);

	//Print IP Address
	char ipStr[20];
	WifiHelper::getIPAddressStr(ipStr);
	printf("IP ADDRESS: %s\n", ipStr);

	//Setup RTC
	WifiHelper::sntpAddServer("uk.pool.ntp.org");
	WifiHelper::sntpStartSync();
	vTaskDelay(5000);

	 for(;;){
		printf("Read Sensor\n");
		newReading();
		vTaskDelay(10000);
	}

}

void vLaunch( void) {
    TaskHandle_t task;

    xTaskCreate(main_task, "MainThread", 2048, NULL, TASK_PRIORITY, &task);

    /* Start the tasks and timer running. */
    vTaskStartScheduler();
}

int main(void) {
    stdio_init_all();
    sleep_ms(2000);
    fs_init();
    tempSensorInit();

    printf("Starting\n");

    FILE *fp = fopen(DATA_FILE, "w");
	if (fp != NULL){
		fprintf(fp, "Timestamp, Celcius\n");
		fclose(fp);
	}

	vLaunch();

}
