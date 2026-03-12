add_executable(${NAME}
        main.cpp
        my_fs_init.c
        WifiHelper.cpp
        )



# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(${NAME} PUBLIC
     pico_stdlib 
     hardware_spi
     hardware_adc
     blockdevice_sd
  	 filesystem_fat
  	 
  	 pico_aon_timer
  	
  	 FreeRTOS-Kernel-Heap4 # FreeRTOS kernel and dynamic heap
     freertos_config
     LWIP_PORT
     pico_cyw43_arch_lwip_sys_freertos
     pico_lwip_sntp
    )

target_compile_definitions(${NAME} PRIVATE
    WIFI_SSID=\"$ENV{WIFI_SSID}\"
    WIFI_PASSWORD=\"$ENV{WIFI_PASSWORD}\"
    CYW43_HOST_NAME="DrJonEA"
)


# create map/bin/hex file etc.
pico_add_extra_outputs(${NAME})

# enable usb output, disable uart output
pico_enable_stdio_usb(${NAME} 1)
pico_enable_stdio_uart(${NAME} 1)
target_compile_definitions(${NAME} PRIVATE
    PICO_DEFAULT_UART_RX_PIN=17
    PICO_DEFAULT_UART_TX_PIN=16
)

pico_enable_filesystem(${NAME})
