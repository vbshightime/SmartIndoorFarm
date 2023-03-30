#ifndef HARDWAREDEFS_H
#define HARDWAREDEFS_H



#define AP_MODE_SSID "Bytebeam-"

String formApSsidName(String deviceId) {
  return String(AP_MODE_SSID + deviceId);
}

#define SPIFF_OTA_PATH "/"

#define DEBUG_SERIAL 1 // should come from build system

/**
   @todo: We need to do this part later
*/
#define ESP_NOW      1
#define ESPNOW_CHANNEL                          1

//Comment out to disable display
#define OLED_DISPLAY 1

//batery max and min voltages
#define BATT_VOL_0                 3.0
#define BATT_VOL_100               4.2


#define UDP_PACKET_BROADCAST 1


#define DEVICE_SENSOR_TYPE  SensorTH
// hardware rev is tied to device type, they both form a combo that decies the firmware behaviour
#define HW_REV                          1 // poc using esp 12e/esp32, no lte modem

// firmware rev
#define FW_REV                          20

//missied Data point spiff file
#define MISS_POINT_STORE_FILE_NAME      "/missDataPoint.txt"

// wiring configuration
#define TEMP_SENSOR_PIN     32
#define BATTERY_VOL_PIN     36
#define SIG_PIN             26
#define CONFIG_PIN          25
#define VOLTAGE_DIV_PIN     5

//Display Configuration
#define SCREEN_WIDTH                            128 // OLED display width, in pixels
#define SCREEN_HEIGHT                           64 // OLED display height, in pixels


#define MILLI_SECS_MULTIPLIER                   1000
#define MICRO_SECS_MULITPLIER                   1000000
#define SECS_MULTIPLIER_DEEPSLEEP               900 //900
#define SECS_PORTAL_WAIT                        60 //120
#define HTTP_CONNEC_TIMEOUT_IN_MS               100

#define EEPROM_STORE_SIZE                       512
#define EEPROM_STORAGE_FORMAT_VERSION           "c1"
#define EEPROM_STARTING_ADDRESS                 0

#define WAN_WIFI_SSID_DEFAULT                    "Sarthak"
#define WAN_WIFI_PASS_DEFAULT                    "wireless18"
#define DEVICE_ID_DEFAULT                        "Bytebeam_1"



#define REF_PRESSURE                            1018.6  // hPa local QFF (official meteor-station reading)
#define OUT_TEMP                                17.2           // °C  measured local outdoor temp.
#define BAROMETER_ALTITUDE                      300  // meters ... map readings + barometer position

#define BATTERY_INITIAL_READING     0
#define INVALID_TEMP_READING        99
#define INVALID_HUMIDITY_READING    0
#define INVALID_GAS_READING        -1
#define INVALID_LIGHT_READING      -1
#define INVALID_MOISTURE_READING   -1
#define INVALID_CAP_READING        -1
#define INVALID_GYRO_READING       -1
#define INVALID_ACCEL_READING      -1
#define INVALID_ALTITUDE_READING   -1
#define INVALID_SEA_READING        -1
#define INVALID_BMP_TEMP_READING    99
#define INVALID_BMP_P_READING      -1
#define CCS_WARM_DURATION                       6000

#define MINIMUM_DEBOUNCE_TIME         250
#define MIN_TARGET_TEMP               4
#define MAX_TARGET_TEMP               20
#define MIN_TARGET_HUMID              20
#define MAX_TARGET_HUMID              60
#define MAX_WAKEUP_COUNT              2
#define MIN_WAKEUP_COUNT              0
#define CALIBRATION_LEVEL             0

#define CAP_SAMPLE_NUMBER             10

//Values corresponding to FDC
#define UPPER_BOUND  0X4000                 // max readout capacitance
#define LOWER_BOUND  (-1 * UPPER_BOUND)
#define CHANNEL 0                          // channel to be read
#define MEASURMENT 0                       // measurment channel




#ifdef DEBUG_SERIAL
#define DEBUG_PRINTF(...)           Serial.printf(__VA_ARGS__)
#define DEBUG_PRINTLN(...)          Serial.println(__VA_ARGS__)
#define DEBUG_PRINT(...)            Serial.print(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#define DEBUG_PRINTLN(...)
#define DEBUG_PRINT(...)
#endif


#endif // HARDWAREDEFS_H
