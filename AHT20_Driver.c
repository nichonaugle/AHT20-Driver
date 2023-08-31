#include <stdio.h>
#include <i2c_test.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

const char *TAG = "AHT20-driver";

/* Creates I2C driver in ESP32 */
esp_err_t AHT20_init() {
    esp_err_t err = ESP_OK;

    i2c_config_t config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,         
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,                    //check to see if project already has pullup resistors 
        .scl_pullup_en = GPIO_PULLUP_ENABLE,                    //check to see if project already has pullup resistors 
        .master.clk_speed = 100000,                             //100 kHz
        .clk_flags = 0,                                         
    };

    i2c_param_config(I2C_NUM_0, &config);
    err = i2c_driver_install(I2C_NUM_0, config.mode, 0, 0, 0);

    return err;
}

/* Runs Initialize Sensor Codes to calibrate sensor */
esp_err_t AHT20_calibrate() { 
    esp_err_t err = ESP_OK;

    /* Reset Device Before Calibration */
    AHT20_soft_reset();

    /* Create Byte Buffer and Send */
    uint8_t write_buffer[] = { INITIALIZE_CODE, INIT_A_CODE, INIT_B_CODE };
    err = i2c_master_write_to_device(I2C_NUM_0, DEVICE_ADDRESS, write_buffer, 3, pdMS_TO_TICKS(100));
    
    printf("Calibrating...");
    /* Waits Until Calibrated */
    while (AHT20_status_check() & STATUS_BUSY) {
        vTaskDelay(pdMS_TO_TICKS(10));
        printf(".");
    }

    /* Error Catching */
    if (err != ESP_OK) {
        printf("Calibration failed: %s\n", esp_err_to_name(err));
    }
    if (!(AHT20_status_check() & STATUS_CALIBRATED)) {
        printf("Calibratioon failed!");
        return err;
    }

    printf("\nCalibration Complete!");
    
    return err;
}

/* Status check if sensor is busy */
uint8_t AHT20_status_check() {

    /* Request Slave Status and Store */
    uint8_t status;
    esp_err_t err = i2c_master_read_from_device(I2C_NUM_0, DEVICE_ADDRESS, &status, 1, pdMS_TO_TICKS(100));
    
    /* Error Catching */
    if (err != ESP_OK) {
        printf("Soft reset failed: %s\n", esp_err_to_name(err));
        return 0xFF;
    }

    return status;
}

/* Initiates a Soft Reset */
esp_err_t AHT20_soft_reset() { 
    esp_err_t err = ESP_OK;

    /* Create Byte Buffer and Send */
    uint8_t write_buffer[] = { SOFT_RESET_CODE };
    err = i2c_master_write_to_device(I2C_NUM_0, DEVICE_ADDRESS, write_buffer, 1, pdMS_TO_TICKS(50)); //Only takes 20 ms

    /* Error Catching */
    if (err != ESP_OK) {
        printf("Soft reset failed: %s\n", esp_err_to_name(err));
    }

    return err;
}

/* Reads Temperature and Humidity and stores in local struct */
esp_err_t AHT20_read(AHT20_data *AHT20_data_struct) {
    esp_err_t err = ESP_OK;

    /* Create Byte Buffers */
    uint8_t read_buffer[6];                                                             // Initialize a 6-byte read buffer
    uint8_t write_buffer[] = { MEASUREMENT_CODE, TRIGGER_A_CODE, TRIGGER_B_CODE };      // Create Measurement byte buffer

    /* Initiate Reading Process */
    err = i2c_master_write_to_device(I2C_NUM_0, DEVICE_ADDRESS, write_buffer, 3, pdMS_TO_TICKS(100)); // Only takes 75ms

    /* Error Catching */
    if (err != ESP_OK) {
        printf("Writing to slave failed: %s\n", esp_err_to_name(err));
    }

    /* Begin Slave Communication and Store Data */
    err = i2c_master_read_from_device(I2C_NUM_0, DEVICE_ADDRESS, read_buffer, 6, pdMS_TO_TICKS(100));
    
    /* Error Catching */
    if (err != ESP_OK) {
        printf("Reading from slave failed: %s\n", esp_err_to_name(err));
    }

    /* Humidity Data Calculation (20 bits long) */
    uint32_t humidity_bit_data = read_buffer[1];
    humidity_bit_data <<= 8;
    humidity_bit_data |= read_buffer[2];
    humidity_bit_data <<= 4;
    humidity_bit_data |= (read_buffer[3] >> 4);
    /* Data Storage */
    AHT20_data_struct->humidity = ((float)humidity_bit_data * 100)/0x100000; // Try if no work: ((float)h_b_d*100) / 0x100000


    /* Temperature Data Calculation (20 bits long) */
    uint32_t temperature_bit_data = read_buffer[3] & 0x0F; // 0x0F = b00001111
    temperature_bit_data <<= 8;
    temperature_bit_data |= read_buffer[4];
    temperature_bit_data <<= 8;
    temperature_bit_data |= read_buffer[5];
    /* Data Storage */
    AHT20_data_struct->celsius_temperature = (((float)temperature_bit_data) * TEMPERATURE_RAW_TO_CELSIUS) - 50;
    AHT20_data_struct->farenheight_temperature = (AHT20_data_struct->celsius_temperature * 1.8) + 32;

    return err;
}
