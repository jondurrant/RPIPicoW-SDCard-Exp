/*
 * Copyright 2024, Hiroyuki OYAMA
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdio.h>
#include <string.h>
#include <hardware/clocks.h>
#include <hardware/flash.h>
#include "blockdevice/flash.h"
#include "blockdevice/sd.h"
#include "filesystem/fat.h"
#include "filesystem/littlefs.h"
#include "filesystem/vfs.h"

#define SDIO_SCK 10
#define SDIO_CMD 11 // TX
#define SDIO_D0 12 // RX
//#define SDIO_D1 13
//#define SDIO_D2 14
#define SDIO_D3 13 //CS


bool fs_init(void) {
    printf("Initialize custom file system\n");
    /*
    blockdevice_t *sd = blockdevice_sd_create(spi0,
                                              PICO_DEFAULT_SPI_TX_PIN,
                                              PICO_DEFAULT_SPI_RX_PIN,
                                              PICO_DEFAULT_SPI_SCK_PIN,
                                              PICO_DEFAULT_SPI_CSN_PIN,
                                              10 * MHZ,
                                              false);
  */
    blockdevice_t *sd = blockdevice_sd_create(spi1,
    		SDIO_CMD,
			SDIO_D0,
			SDIO_SCK,
			SDIO_D3,
		   10 * MHZ,
		   false);

    filesystem_t *fat = filesystem_fat_create();

    // Mount FAT SD card `/`

    int err = fs_mount("/", fat, sd);
    if (err == -1) {
        printf("format / with FAT\n");
        err = fs_format(fat, sd);
        if (err == -1) {
            printf("fs_format error: %s\n", strerror(errno));
            return false;
        }
        err = fs_mount("/", fat, sd);
        if (err == -1) {
            printf("fs_mount error: %s\n", strerror(errno));
            return false;
        }
    }


    return err == 0;
}
