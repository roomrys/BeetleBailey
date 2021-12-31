/* SPI Master example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

#include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
#include "esp_log.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// #include "esp_system.h"
#include "driver/spi_master.h"
// #include <conio.h>
// #include "soc/gpio_struct.h"
// #include "driver/gpio.h"

#define PIN_NUM_MISO 12
#define PIN_NUM_MOSI 13
#define PIN_NUM_CLK  14
#define PIN_NUM_CS   15

void app_main()
{
    printf("Running main");
    char tag[4] = {'l', 'o', 'g', '\0'};

    while(1){
        // while(!kbhit()){
            ESP_LOGD(tag, ">> testing spi app_main");

            // esp_err_t ret;

            spi_bus_config_t buscfg={
                .mosi_io_num=PIN_NUM_MOSI,                ///< GPIO pin for Master Out Slave In (=spi_d) signal, or -1 if not used.
                .miso_io_num=PIN_NUM_MISO,                ///< GPIO pin for Master In Slave Out (=spi_q) signal, or -1 if not used.
                .sclk_io_num=PIN_NUM_CLK,                ///< GPIO pin for Spi CLocK signal, or -1 if not used.
                .quadwp_io_num=-1,              ///< GPIO pin for WP (Write Protect) signal which is used as D2 in 4-bit communication modes, or -1 if not used.
                .quadhd_io_num=-1,              ///< GPIO pin for HD (HolD) signal which is used as D3 in 4-bit communication modes, or -1 if not used.
                .max_transfer_sz=0            ///< Maximum transfer size, in bytes. Defaults to 4092 if 0 when DMA enabled, or to 64 if DMA is disabled.
            // uint32_t flags;                 ///< Abilities of bus to be checked by the driver. Or-ed value of ``SPICOMMON_BUSFLAG_*`` flags.
            // int intr_flags;    /**< Interrupt flag for the bus to set the priority, and IRAM attribute, see
            //                      *  ``esp_intr_alloc.h``. Note that the EDGE, INTRDISABLED attribute are ignored
            //                      *  by the driver. Note that if ESP_INTR_FLAG_IRAM is set, ALL the callbacks of
            //                      *  the driver, and their callee functions, should be put in the IRAM.
            //                      */
            };
            ESP_LOGI(tag, "... Initializing bus.");
            ESP_ERROR_CHECK(spi_bus_initialize(HSPI_HOST, &buscfg, 1));

            spi_device_handle_t spi;
            spi_device_interface_config_t devcfg={
                .command_bits=4,           ///< Default amount of bits in command phase (0-16), used when ``SPI_TRANS_VARIABLE_CMD`` is not used, otherwise ignored.
                .address_bits=4,           ///< Default amount of bits in address phase (0-64), used when ``SPI_TRANS_VARIABLE_ADDR`` is not used, otherwise ignored.
                .dummy_bits=0,             ///< Amount of dummy bits to insert between address and data phase
                .mode=1,                   ///< SPI mode (0-3)
                .duty_cycle_pos=128,         ///< Duty cycle of positive clock, in 1/256th increments (128 = 50%/50% duty). Setting this to 0 (=not setting it) is equivalent to setting this to 128.
            // uint8_t cs_ena_pretrans;        ///< Amount of SPI bit-cycles the cs should be activated before the transmission (0-16). This only works on half-duplex transactions.
            // uint8_t cs_ena_posttrans;       ///< Amount of SPI bit-cycles the cs should stay active after the transmission (0-16)
                .clock_speed_hz=1*1000,             ///< Clock speed, divisors of 80MHz, in Hz. See ``SPI_MASTER_FREQ_*``.
                .input_delay_ns=25,             /**< Maximum data valid time of slave. The time required between SCLK and MISO
                valid, including the possible clock delay from slave to master. The driver uses this value to give an extra
                delay before the MISO is ready on the line. Leave at 0 unless you know you need a delay. For better timing
                performance at high frequency (over 8MHz), it's suggest to have the right value.
                */
                .spics_io_num=PIN_NUM_CS,               ///< CS GPIO pin for this device, or -1 if not used
            // uint32_t flags;                 ///< Bitwise OR of SPI_DEVICE_* flags
                .queue_size=40                 ///< Transaction queue size. This sets how many transactions can be 'in the air' (queued using spi_device_queue_trans but not yet finished using spi_device_get_trans_result) at the same time
            // transaction_cb_t pre_cb;   /**< Callback to be called before a transmission is started.
            //                              *
            //                              *  This callback is called within interrupt
            //                              *  context should be in IRAM for best
            //                              *  performance, see "Transferring Speed"
            //                              *  section in the SPI Master documentation for
            //                              *  full details. If not, the callback may crash
            //                              *  during flash operation when the driver is
            //                              *  initialized with ESP_INTR_FLAG_IRAM.
            //                              */
            // transaction_cb_t post_cb;  /**< Callback to be called after a transmission has completed.
            //                              *
            //                              *  This callback is called within interrupt
            //                              *  context should be in IRAM for best
            //                              *  performance, see "Transferring Speed"
            //                              *  section in the SPI Master documentation for
            //                              *  full details. If not, the callback may crash
            //                              *  during flash operation when the driver is
            //                              *  initialized with ESP_INTR_FLAG_IRAM.
            //                              */
            };
            ESP_LOGI(tag, "... Adding device bus.");
            ESP_ERROR_CHECK(spi_bus_add_device(HSPI_HOST, &devcfg, &spi));

            char data[1]= {0x97};
            
            spi_transaction_t spi_trans={
                .flags=0,                 ///< Bitwise OR of SPI_TRANS_* flags
                .cmd={0x05},                   /**< Command data, of which the length is set in the ``command_bits`` of spi_device_interface_config_t.
                                            *
                                            *  <b>NOTE: this field, used to be "command" in ESP-IDF 2.1 and before, is re-written to be used in a new way in ESP-IDF 3.0.</b>
                                            *
                                            *  Example: write 0x0123 and command_bits=12 to send command 0x12, 0x3_ (in previous version, you may have to write 0x3_12).
                                            */
                .addr={0x00},                  /**< Address data, of which the length is set in the ``address_bits`` of spi_device_interface_config_t.
                                            *
                                            *  <b>NOTE: this field, used to be "address" in ESP-IDF 2.1 and before, is re-written to be used in a new way in ESP-IDF3.0.</b>
                                            *
                                            *  Example: write 0x123400 and address_bits=24 to send address of 0x12, 0x34, 0x00 (in previous version, you may have to write 0x12340000).
                                            */
                .length=8,                  ///< Total data length, in bits
                .rxlength=0,                ///< Total data length received, should be not greater than ``length`` in full-duplex mode (0 defaults this to the value of ``length``).
            // void *user;                     ///< User-defined variable. Can be used to store eg transaction ID.
            // union {
                .tx_buffer=data,      ///< Pointer to transmit buffer, or NULL for no MOSI phase
                // uint8_t tx_data[4];         ///< If SPI_USE_TXDATA is set, data set here is sent directly from this variable.
            // };
            // union {
                .rx_buffer=data,            ///< Pointer to receive buffer, or NULL for no MISO phase. Written by 4 bytes-unit if DMA is used.
            //     uint8_t rx_data[4];         ///< If SPI_USE_RXDATA is set, data is received directly to this variable
            // };
            };        //the rx data should start from a 32-bit aligned address to get around dma issue.
            // //Initialize the SPI bus
            // ret=spi_bus_initialize(HSPI_HOST, &buscfg, 1);
            // ESP_ERROR_CHECK(ret);
            // //Attach the LCD to the SPI bus
            // ret=spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
            // ESP_ERROR_CHECK(ret);

            // while(true){
            ESP_LOGI(tag, "... Transmitting.");
            ESP_ERROR_CHECK(spi_device_transmit(spi, &spi_trans));

            ESP_LOGI(tag, "... Removing Device.");
            ESP_ERROR_CHECK(spi_bus_remove_device(spi));

            ESP_LOGI(tag, "... Freeing bus.");
            ESP_ERROR_CHECK(spi_bus_free(HSPI_HOST));
            // };

            ESP_LOGD(tag, "<< test_spi_task");
            // vTaskDelete(NULL);   //code gets hung-up on this line

            // for (int i = 1; i >= 0; i--) {
            //     ESP_LOGD(tag, "Restarting in %d seconds...\n", i);
            //     vTaskDelay(1000 / portTICK_PERIOD_MS);
            // }
            // ESP_LOGD(tag, "Restarting now.\n");
            // fflush(stdout);
            // esp_restart();
        // }
        // getch();
    }
}
