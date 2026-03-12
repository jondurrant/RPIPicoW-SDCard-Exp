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
	FILE *fp = fopen(DATA_FILE, "a");
	if (fp != NULL){
		fprintf(fp, "%d, %.3f!\n",
				to_ms_since_boot (get_absolute_time())/ 1000,
				readOnboardTemperature()
				);
		fclose(fp);
	} else {
		printf("Open for A failed\n");
	}
}

int main(void) {
    stdio_init_all();
    sleep_ms(2000);
    fs_init();
    tempSensorInit();

    printf("Starting\n");

    FILE *fp = fopen(DATA_FILE, "w");
	if (fp != NULL){
		fprintf(fp, "Seconds, Celcius\n");
		fclose(fp);
	}

    for(;;){
    	printf("Read Sensor\n");
    	newReading();
    	sleep_ms(10000);
    }


}
