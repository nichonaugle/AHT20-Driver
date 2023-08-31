#include <esp_err.h>

//Data Lines
#define     I2C_MASTER_SDA_IO     GPIO_NUM_4
#define     I2C_MASTER_SCL_IO     GPIO_NUM_3
//Conversion Constants
#define     HUMIDITY_RAW_TO_PERCENT         0.00009536743164
#define     TEMPERATURE_RAW_TO_CELSIUS      0.0001907348633     //SUBTRACT 50 After Multiplying     

//Device I2C Code     
#define     DEVICE_ADDRESS      0x38    //0111000X
#define     DEVICE_ADDR_WRITE   0x70    //01110000
#define     DEVICE_ADDR_READ    0x71    //01110001
//Initializing Sensor Codes
#define     INITIALIZE_CODE     0xBE    //10111110
#define     INIT_A_CODE         0x08    //00001000
#define     INIT_B_CODE         0x00    //00000000
//Measuring Sensor Data Codes
#define     MEASUREMENT_CODE    0xAC    //10101100
#define     TRIGGER_A_CODE      0x33    //00110011
#define     TRIGGER_B_CODE      0x00    //00000000
//Soft Reset Code
#define     SOFT_RESET_CODE     0xBA    //10111010
//Status Codes
#define     STATUS_CALIBRATED    0X08    //00001000
#define     STATUS_BUSY          0x80    //10000000

/* Used to store AHT20 data */
typedef struct {
    float celsius_temperature;
    float farenheight_temperature;
    float humidity;
} AHT20_data;

/* Initializes I2C AHT20 on ESP32 channel 0 
 * @ 50kHz
 * GPIO internal pullups enabled 
 */
esp_err_t AHT20_init();

/* Calibrates the I2C AHT20 
 * Runs Reset and Calibration Command
 */
esp_err_t AHT20_calibrate();

/* Built in Status Checker */
uint8_t AHT20_status_check();

/* Resets AHT20 without power cycle*/
esp_err_t AHT20_soft_reset();

/* Reads Temperature and Humidty Values
 * Stores values in struct
 * Calculations completed in this function as well
 * Only need to call to refresh sensor data 
 */
esp_err_t AHT20_read(AHT20_data *AHT20_data_struct);
